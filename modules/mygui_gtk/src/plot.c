#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <math.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  lua_State *L;
  int pool_idx;
  double fontsize;
}Plot;
const float plot_colors[][3] = {{1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {1,0,1}, {0,1,1}, {1,1,1}};
/*
static void BtnOnClick(GtkWidget *obj, gpointer data){
  Plot *plot = data;
  lua_State *L = plot->L;
  int prev = lua_gettop(L);
  lua_settop(L, 0);
  lua_rawgeti(L, LUA_REGISTRYINDEX, mainwindow.poolidx);
    lua_getmetatable(L, -1);
      lua_getfield(L, -1, "pool");
        lua_rawgeti(L, -1, plot->pool_idx);
          lua_getfield(L, -1, "OnClick");
            lua_pushvalue(L, -2);
  if(lua_isfunction(L, -2)){
    lua_pcall(L, 1, 0, 0);
  }else{
    plot->was_clicked = 1;
  }
  lua_settop(L, prev);
}

static int L_BtnWasClicked(lua_State *L){
  int top = lua_gettop(L);
  if(!lua_istable(L, -1)){
    printf("Not table!\n");
    return 0;
  }
  lua_getmetatable(L, -1);
  if(!lua_istable(L, -1)){
    printf("Not metatable!\n");
    return 0;
  }
  
  lua_getfield(L, -1, "handle");
  if(!lua_islightuserdata(L, -1)){
    printf("Err2\n");
    return 0;
  }
  Plot *plot = (Plot*)lua_topointer(L, -1);
  char res = plot->was_clicked;
  plot->was_clicked = 0;

  lua_settop(L, top);
  lua_pushboolean(L, res);
  return 1;
}*/
//TODO: добавить палитры
//TODO: биндинг таблиц

static int L_Plot_draw(lua_State *L){
  int top = lua_gettop(L);
  if(!lua_istable(L, -1)){
    printf("Not table!\n");
    return 0;
  }
  lua_getmetatable(L, -1);
  if(!lua_istable(L, -1)){
    printf("Not metatable!\n");
    return 0;
  }
  lua_getfield(L, -1, "handle");
  if(!lua_islightuserdata(L, -1)){
    printf("Err2\n");
    return 0;
  }
  Plot *plot = (Plot*)lua_topointer(L, -1);
  if(GTK_IS_WIDGET(plot->obj))gtk_widget_queue_draw(plot->obj);
  lua_settop(L, top);
  return 0;
  
}

static int L_Plot_GC(lua_State *L){
  printf("Plot GC\n");
  int top = lua_gettop(L);
  if(!lua_istable(L, -1)){
    printf("Not table!\n");
    return 0;
  }
  lua_getmetatable(L, -1);
  if(!lua_istable(L, -1)){
    printf("Not metatable!\n");
    return 0;
  }
  lua_getfield(L, -1, "handle");
  if(!lua_islightuserdata(L, -1)){
    printf("Err2\n");
    return 0;
  }
  Plot *plot = (Plot*)lua_topointer(L, -1);
  if(GTK_IS_WIDGET(plot->obj))gtk_widget_destroy(plot->obj);
  if(plot)free(plot);
  lua_settop(L, top);
  return 0;
}

char plot_minmax(float *data, int num, size_t points, int coord, double *_min, double *_max){
  double min, max, cur;
  min = max = data[coord];
  size_t smax = num * points;
  for(size_t i=coord; i<smax; i+=num){
    cur = data[i];
    if(min > cur)min = cur;
    if(max < cur)max = cur;
  }
  *_min = min; *_max = max;
  return 0;
}
void cr_line(cairo_t *cr, int x1, int y1, int x2, int y2){
  cairo_move_to(cr, x1, y1);
  cairo_line_to(cr, x2, y2);
}
double plot_autorange(double *a, double *b){
  if(*b < *a){
    double temp = *b;
    *b = *a;
    *a = temp;
  }
  double pmax;
  int man, ipwr;
  
  pmax = log10(*b - *a);
  ipwr = (int)pmax;
  pmax -= ipwr;
  
  if(pmax < log10(2)){
    man = 2;
  }else if(pmax < log10(5)){
    man = 5;
  }else{
    man = 10;
  }
  
  int min = (int)(*a * pow(0.1, ipwr-1));
  int max = (int)(*b * pow(0.1, ipwr-1));
  int diff = min % man;
  if(min > 0){min -= diff;}else{min -= man+diff;}
  diff = max % man;
  if(max > 0){max += man-diff;}else{max -= diff;}
  
  *a = (min)*pow(10, ipwr-1);
  *b = (max)*pow(10, ipwr-1);

  return man * pow(10, ipwr-1);
}

void fixlua_geti(lua_State *L, int idx, int i){
  /*static char buffer[100]="";
  lua_rawgeti(L, idx, i);
  if(lua_isnil(L, -1)){
    lua_pop(L, 1);
    snprintf(buffer, 99, "%i", i);
    lua_getfield(L, idx, buffer);
  }*/
  lua_pushnumber(L, i);
  lua_gettable(L, idx-1);
}

static gboolean PlotOnDraw(GtkWidget *widget, GdkEventExpose *event, gpointer data){
  Plot *plot = data;
  lua_State *L = plot->L;
  int prev = lua_gettop(L);
  int xpos=-1, fmtlen=0, *ypos = NULL;
  lua_settop(L, 0);
  lua_rawgeti(L, LUA_REGISTRYINDEX, mainwindow.poolidx);
    lua_getmetatable(L, -1);
      lua_getfield(L, -1, "pool");
        lua_rawgeti(L, -1, plot->pool_idx);
          //--- вот тут добрались до this
          lua_getfield(L, -1, "format");
          if(lua_istable(L, -1)){ //формат на месте
            //читаем размер таблицы (количество столбцов данных)
            lua_len(L, -1);
            if(lua_isnumber(L, -1))fmtlen = lua_tonumber(L, -1);
            lua_pop(L, 1);
            //ищем столбец [0] или ['x']: если есть, используем в качестве икса, если нет - будет по номерам
            lua_rawgeti(L, -1, 0);
            if(lua_isnumber(L, -1)){
              xpos = lua_tonumber(L, -1);
              lua_pop(L, 1);
            }else{
              lua_getfield(L, -2, "x");
              if(lua_isnumber(L, -1)){
                xpos = lua_tonumber(L, -1);
              }
              lua_pop(L, 2);
            }
            if(xpos <= 0)xpos = -1;
            //если форматная таблица вообще есть (кроме иксового столбца)
            if(fmtlen != 0){
              ypos = (int*)malloc(sizeof(int)*fmtlen);
              if(ypos == NULL){
                fprintf(stderr, "Can not allocate %lu bytes!\n", (unsigned long)sizeof(int)*fmtlen);
                return 0;
              }
              //lua_pop(L, 1);
              for(int i=0; i<fmtlen; i++){
                lua_rawgeti(L, -1, i+1);
                if(!lua_isnumber(L, -1)){
                  fmtlen = i;
                  lua_pop(L, 1);
                  //TODO print error message
                  break;
                }
                ypos[i] = lua_tonumber(L, -1);
                lua_pop(L, 1);
              }
              /*printf("x=%i ; y=[", xpos);
              for(int i=0; i<fmtlen; i++){
                printf("%i\t", ypos[i]);
              }
              printf("]\n");
              //*/
            }
          }else{ //а вдруг вместо таблицы в формат подсунули только номер иксового столбца?
            if(lua_isnumber(L, -1))xpos = lua_tonumber(L, -1);
          }
          lua_pop(L, 1); //format
          
          //read data
          lua_getfield(L, -1, "data");
          if(!lua_istable(L, -1)){
            printf("Draw: data not table\n");
            if(ypos)free(ypos);
            return 0;
          }
            if(ypos == NULL){
              lua_pushnumber(L, 1);
              lua_gettable(L, -2);
              if(!lua_istable(L, -1)){
                lua_pop(L, 2);
                //TODO: обработка ошибки
                return 0;
              }
              lua_len(L, -1);
              fmtlen = lua_tonumber(L, -1);
              if(xpos != -1)fmtlen--;
              lua_pop(L, 2);
            }
            
            int data_len = 0;
            float *arr;
            //читаем размер таблицы (количество столбцов данных)
            lua_len(L, -1);
            if(lua_isnumber(L, -1))data_len = lua_tonumber(L, -1);
            lua_pop(L, 1);
            //printf("mat %i x %i\n", data_len, fmtlen);
            fmtlen++; //добавлям столбец иксов
            arr = (float*)malloc(sizeof(float)*fmtlen*data_len);
            if(arr == NULL){
              fprintf(stderr, "Can not allocate %lu bytes!\n", (unsigned long)sizeof(float)*fmtlen*data_len);
              if(ypos)free(ypos);
              return 0;
            }
            
            size_t data_pos = 0;
            //printf("Data = %s\n", lua_typename(L, lua_type(L, -1)));
            for(int i=0; i<data_len; i++){
              lua_pushnumber(L, i+1);
              lua_gettable(L, -2);
              //printf("Data [%i] = %s\n", i+1, lua_typename(L, lua_type(L, -1)));
              if(!lua_istable(L, -1)){
                data_len = i;
                lua_pop(L, 1);
                printf("Plot: data[%i] not table\n", i);
                //TODO print error message
                break;
              }
              if(xpos > 0){
                lua_rawgeti(L, -1, xpos);
                if(lua_isnumber(L, -1))arr[data_pos]=lua_tonumber(L, -1); else arr[data_pos] = i+1;
                lua_pop(L, 1);
              }else arr[data_pos] = i+1;
              for(int j=1; j<fmtlen; j++){
                int yidx;
                if(ypos != NULL){
                  yidx = ypos[j-1];
                }else{
                  if(xpos == -1){
                    yidx = j;
                  }else if(xpos <= j){
                    yidx = j+1;
                  }else{
                    yidx = j;
                  }
                }
               lua_rawgeti(L, -1, yidx);
                if(!lua_isnumber(L, -1)){
                  
                  printf("Plot: data[%i][%i(%i)] not number\n", i, j, yidx);
                  data_len = 0;
                  lua_pop(L, 1);
                  break;
                }
                arr[data_pos + j] = lua_tonumber(L, -1);
                lua_pop(L, 1);
              }
              lua_pop(L, 1);
              data_pos += fmtlen;
            }
            /*data_pos = 0;
            printf("data(%i)[%i]:\n", data_len, fmtlen);
            for(int i=0; i<data_len; i++){
              printf("%f [", arr[data_pos]);
              for(int j=1; j<fmtlen; j++){
                printf("%f\t", arr[data_pos+j]);
              }
              printf("]\n");
              data_pos += fmtlen;
            }*/
          
            if(ypos)free(ypos);
          lua_pop(L, 1);
        
  cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(widget));
  GdkRectangle rect;

  gdk_window_get_geometry(gtk_widget_get_window(widget), &rect.x, &rect.y, &rect.width, &rect.height);
  
  cairo_paint (cr);
  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
  cairo_fill(cr);
  
  
  double kx, bx, ky, by, dx, dy;
  double ymin, ymax, cmin, cmax;
  plot_minmax(arr, fmtlen, data_len, 1, &ymin, &ymax);
  for(int i=2; i<fmtlen; i++){
    plot_minmax(arr, fmtlen, data_len, i, &cmin, &cmax);
    if(cmin < ymin)ymin = cmin;
    if(cmax > ymax)ymax = cmax;
  }
  dy = plot_autorange(&ymin, &ymax);
  ky = -rect.height/(ymax-ymin); by = rect.height - ky*ymin;
  plot_minmax(arr, fmtlen, data_len, 0, &cmin, &cmax);
  dx = plot_autorange(&cmin, &cmax);
  kx = rect.width/(cmax-cmin); bx = -kx*cmin;
  
  //отрисовка кривых
  for(int line=1; line<fmtlen; line++){
    cairo_set_source_rgb(cr, plot_colors[line-1][0], plot_colors[line-1][1], plot_colors[line-1][2]);
    
    cairo_move_to(cr, bx + kx*arr[0], by + ky*arr[line]);
    size_t imax = data_len*fmtlen;
    for(size_t i=line; i<imax; i+=fmtlen){
      cairo_line_to(cr, bx + kx*arr[i-line], by + ky*arr[i]);
    }
    cairo_stroke(cr);
  }
  //отрисовка осей
  cairo_set_source_rgb(cr, 1, 1, 1);
  if(cmin <= 0 && cmax >= 0){
    cr_line(cr, bx, 0, bx, rect.height);
  }
  if(ymin <= 0 && ymax >= 0){
    cr_line(cr, 0, by, rect.width, by);
  }
  cairo_stroke(cr);
  
  cairo_set_line_width(cr, 1);
  double fontsize = plot->fontsize;
  if(fontsize < 0){
    if(rect.height < rect.width){
      fontsize *= -rect.height;
    }else{
      fontsize *= -rect.width;
    }
  }
  cairo_set_font_size(cr, fontsize);
  char buf[100];
  double x,y;
  if(ymin < 0 && ymax > 0)y = 0; else y = ymin;
  for(x = cmin; x<cmax; x+=dx){
    cr_line(cr,bx+kx*x,0, bx+kx*x,rect.height);
    sprintf(buf, "%g", x);
    cairo_move_to(cr, bx+kx*x, by+ky*y);
    cairo_show_text(cr, buf);
  }
  if(cmin < 0 && cmax > 0)x=0; else x=cmin;
  for(y = ymin; y<ymax; y+=dy){
    cr_line(cr,0,by+ky*y, rect.width,by+ky*y);
    sprintf(buf, "%g", y);
    cairo_move_to(cr, bx+kx*x, by+ky*y);
    cairo_show_text(cr, buf);
  }
  cairo_stroke(cr);
  
  cairo_destroy (cr);
  
  free(arr);
  
  lua_settop(L, prev);
  return 1;
}

static int L_NewPlot(lua_State *L){
  Plot *plot = (Plot*)malloc(sizeof(Plot));
  int x=0, y=0, w=100, h=100;
  if(lua_gettop(L) >= 5){
    if(lua_isnumber(L, 2))x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))y = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4))w = lua_tonumber(L, 4);
    if(lua_isnumber(L, 5))h = lua_tonumber(L, 5);
  }
  
  plot->pool_idx = mk_blank_table(L, plot, L_Plot_GC);
  lua_createtable(L, 0, 0);
  lua_setfield(L, -2, "data");
  lua_createtable(L, 0, 0);
  lua_setfield(L, -2, "format");
  lua_pushcfunction(L, L_Plot_draw);
  lua_setfield(L, -2, "Refresh");
  
  plot->L = L;
  plot->obj = gtk_drawing_area_new();
  gtk_widget_set_size_request(plot->obj, w, h);
  plot->fontsize = -0.03;
  
  gtk_fixed_put(GTK_FIXED(mainwindow.fixed), plot->obj, x, y);
  gtk_widget_show(plot->obj);
  g_signal_connect(G_OBJECT(plot->obj), "draw", G_CALLBACK(PlotOnDraw), plot);
  return 1;
}

void plot_reg(lua_State *L){
  lua_pushcfunction(L, L_NewPlot);
  lua_setfield(L, -2, "NewPlot");
#ifdef DEBUG
  printf("Plot registred\n");
#endif
}
