#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  int pool_idx;
  double fontsize;
  int x, y, w, h;
  float x_min, x_max, y_min, y_max; 
  int abs_x, abs_y;
  float rel_x, rel_y;
  float cur_size;
}Plot;
const float plot_colors[][3] = {{1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {1,0,1}, {0,1,1}, {1,1,1}};
//TODO: добавить палитры

struct PlotIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  plot->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(plot->obj)), plot->obj, plot->x, plot->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, plot->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  plot->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(plot->obj)), plot->obj, plot->x, plot->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, plot->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  plot->w = w;
  gtk_widget_set_size_request(plot->obj, plot->w, plot->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, plot->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  plot->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(plot->obj, plot->w, plot->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, plot->h);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(plot->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(plot->obj));
  lua_pushboolean(L, en);
  return 1;
}
static int setter_cursize(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  plot->cur_size = lua_tonumber(L, tblindex+2);
  return 0;
}
static int getter_cursize(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, plot->cur_size);
  return 1;
}
static int getter_cur_x(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, plot->rel_x);
  return 1;
}
static int getter_cur_y(lua_State *L, int tblindex){
  Plot *plot = (Plot*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, plot->rel_y);
  return 1;
}

struct PlotIntVariables plot_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
  {.name = "cur_size", .setter= setter_cursize, .getter = getter_cursize},
  {.name = "cur_x", .setter = NULL, .getter = getter_cur_x},
  {.name = "cur_y", .setter = NULL, .getter = getter_cur_y},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_Plotiterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(plot_intvars)){
    lua_pushstring(L, plot_intvars[val].name); //index
    plot_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_Plotpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_Plotiterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_Plotgetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(plot_intvars); i++){
    if(strcmp(idx, plot_intvars[i].name)==0){
      if(plot_intvars[i].getter == NULL)return 0;
      return plot_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_Plotsetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(plot_intvars); i++){
    if(strcmp(idx, plot_intvars[i].name)==0){
      if(plot_intvars[i].setter == NULL)return 0;
      return plot_intvars[i].setter(L, -3);
    }
  }
#if ALLOW_APPEND_TABLE
  lua_pushstring(L, idx);
  lua_pushvalue(L, -2);
  lua_rawset(L, -3-2);
#endif
  lua_pop(L, 3);
  return 0;
}

static int L_Plot_draw(lua_State *L){
  int top = lua_gettop(L);
  Plot *plot = (Plot*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(plot->obj))gtk_widget_queue_draw(plot->obj);
  lua_settop(L, top);
  return 0;
}

static int L_Plot_GC(lua_State *L){
#ifdef DEBUG
  printf("Plot GC\n");
#endif
  int top = lua_gettop(L);
  Plot *plot = (Plot*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(plot->obj))gtk_widget_destroy(plot->obj);
  if(plot){
    free_index(L, plot->pool_idx);
    free(plot);
  }
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
  if(*b - *a < 1e-100){
    if(*a < 1e-100){
      *a = -1; *b=1;
    }else{
      *a -= *b/10;
      *b += *b/10;
    }
  }
  double pmax;
  int man, ipwr;
  
  pmax = log10(*b - *a);
  ipwr = (int)pmax;
  pmax -= ipwr;
  
  if(pmax < log10(2)){
    if(ipwr < 0)ipwr--;
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
  if(max < 0){max -= diff;}else{max += man-diff;}
  
  *a = (min)*pow(10, ipwr-1);
  *b = (max)*pow(10, ipwr-1);

  return man * pow(10, ipwr-1);
}

void fixlua_geti(lua_State *L, int idx, int i){
  lua_pushnumber(L, i);
  lua_gettable(L, idx-1);
}

static gboolean PlotOnDraw(GtkWidget *widget, GdkEventExpose *event, gpointer data){
  Plot *plot = data;
  lua_State *L = gui.L;
  int prev = lua_gettop(L);
  int xpos=-1, fmtlen=0, *ypos = NULL;
  read_self(L, plot->pool_idx);
    lua_getfield(L, -1, "format");
    if(lua_istable(L, -1)){ //формат на месте
      //читаем размер таблицы (количество столбцов данных)
      lua_len(L, -1);
      if(lua_isnumber(L, -1))fmtlen = lua_tonumber(L, -1);
      lua_pop(L, 1); //pop len
      //ищем столбец [0] или ['x']: если есть, используем в качестве икса, если нет - будет по номерам
      lua_rawgeti(L, -1, 0); //
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
        }
      }else{ //а вдруг вместо таблицы в формат подсунули только номер иксового столбца?
        if(lua_isnumber(L, -1))xpos = lua_tonumber(L, -1);
      }
      lua_pop(L, 1); //format
//READ DATA
      //read data
      lua_getfield(L, -1, "data");
      if(!lua_istable(L, -1)){
        printf("Draw: data is not table\n");
        if(ypos)free(ypos);
        lua_settop(L, prev);
        return 1;
      }
        if(ypos == NULL){
          lua_pushnumber(L, 1);
          lua_gettable(L, -2); //доступ к data[1]
          if(!lua_istable(L, -1)){ //если оно не таблица, то ошибка
            lua_pop(L, 2);
            //TODO: обработка ошибки
            return 0;
          }
          lua_len(L, -1);
          fmtlen = lua_tonumber(L, -1);
          if(xpos != -1)fmtlen--;
          lua_pop(L, 2); //pop len ; data[0]
        }
            
        int data_len = 0;
        float *arr;
        //читаем размер таблицы (количество столбцов данных)
        lua_len(L, -1);
        if(lua_isnumber(L, -1))data_len = lua_tonumber(L, -1);
        lua_pop(L, 1); //pop len
        fmtlen++; //добавлям столбец иксов
        arr = (float*)malloc(sizeof(float)*fmtlen*data_len);
        if(arr == NULL){
          fprintf(stderr, "Can not allocate %lu bytes!\n", (unsigned long)sizeof(float)*fmtlen*data_len);
          if(ypos)free(ypos);
          return 0;
        }
        size_t data_pos = 0;
        for(int i=0; i<data_len; i++){
          lua_pushnumber(L, i+1);
          lua_gettable(L, -2); //data[i+1]
          if(lua_isnil(L, -1)){
            data_len = i;
            lua_pop(L, 1);
            break;
          }
          if(!lua_istable(L, -1)){
            data_len = i;
            lua_pop(L, 1);
            printf("Plot: data[%i] not table\n", i);
            //TODO print error message
            break;
          }
          //читаем data[i][xpos] либо, если xpos<0 или data[xpos]==nil, заполняем порядковыи номерами
          if(xpos > 0){
            lua_rawgeti(L, -1, xpos);
            if(lua_isnumber(L, -1))arr[data_pos]=lua_tonumber(L, -1); else arr[data_pos] = i+1;
            lua_pop(L, 1); //pos xpos
          }else arr[data_pos] = i+1;
          
          for(int j=1; j<fmtlen; j++){
            int yidx;
            
            //ищем номер следующего столбца либо из формата, либо по порядку
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
            lua_pop(L, 1); //pop data[i][yidx]
          }
          lua_pop(L, 1); //pop data[0]
          data_pos += fmtlen;
        }
          
        if(ypos)free(ypos);
      lua_pop(L, 1); //pop data
    lua_pop(L, 1); //self
        
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
  
  //сохранение размеров графика в приватные переменные
  plot->x_min = cmin; plot->x_max = cmax;
  plot->y_min = ymin; plot->y_max = ymax;
  plot->rel_x = plot->x_min + (plot->x_max - plot->x_min)*plot->abs_x / plot->w;
  plot->rel_y = plot->y_max - (plot->y_max - plot->y_min)*plot->abs_y / plot->h;
  
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
  double x,y, rx, ry, rd;
  if(ymin < 0 && ymax > 0)y = by; else y = by + ky*ymin;
  rx = bx + kx*cmin;
  rd = kx*dx;
  char *fmt;
  {
    float rng;
    if(fabs(cmin) > fabs(cmax))rng = fabs(cmin); else rng = fabs(cmax);
    if((rng <= 100 && rng >= 1) || rng < 1e-100)fmt = "%.1f"; else fmt = "%.1e";
  }
  char par=0;
  for(x = cmin;  x<cmax;  x+=dx, rx+=rd, par=!par){
    if(fabs(x) < dx/2)x = 0;
    cr_line(cr,rx,0, rx,rect.height);
    sprintf(buf, fmt, x);
    cairo_text_extents_t strprop;
    cairo_text_extents(cr, buf, &strprop);
    if(par && strprop.width > rd){
      cairo_move_to(cr, rx - strprop.width/4, y - 1.5*fontsize);
    }else{
      if(x == cmin)strprop.width = 0;
      cairo_move_to(cr, rx - strprop.width/4, y - 0.2*fontsize);
    }
    cairo_show_text(cr, buf);
  }
  
  if(cmin < 0 && cmax > 0)x = bx; else x = bx + kx*cmin;
  ry = by + ky*ymin;
  rd = ky*dy;
  {
    float rng;
    if(fabs(ymin) > fabs(ymax))rng = fabs(ymin); else rng = fabs(ymax);
    if((rng <= 100 && rng >= 1) || rng < 1e-100)fmt = "%.1f"; else fmt = "%.1e";
  }
  for(y = ymin; y<ymax+dy; y+=dy, ry+=rd){
    if(fabs(y) < dy/2)y = 0;
    cr_line(cr,0,ry, rect.width,ry);
    if(fabs(y) < dy/2)y = 0;
    sprintf(buf, fmt, y);
    cairo_move_to(cr, x, ry + 0.9*fontsize);
    cairo_show_text(cr, buf);
  }
  
  //если надо отрисовать курсор - отрисовываем
  if(plot->cur_size > 0){
    float size = plot->cur_size/2;
    if(rect.width > rect.height){
      size *= rect.height;
    }else{
      size *= rect.width;
    }
    cr_line(cr, plot->abs_x - size, plot->abs_y, plot->abs_x + size, plot->abs_y);
    cr_line(cr, plot->abs_x, plot->abs_y - size, plot->abs_x, plot->abs_y + size);
    sprintf(buf, "%.2g, %.2g", plot->rel_x, plot->rel_y);
    int tx = plot->abs_x, ty = plot->abs_y;
    cairo_text_extents_t strprop;
    cairo_text_extents(cr, buf, &strprop);
    if(tx + strprop.width > plot->w){
      tx -= strprop.width;
    }
    if(ty - strprop.height < 0){
      ty += strprop.height;
    }
    
    cairo_move_to(cr, tx, ty);
    cairo_show_text(cr, buf);
  }
  cairo_stroke(cr);
  
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
  
  //cairo_stroke(cr);
  
  cairo_destroy (cr);
  
  free(arr);
  lua_settop(L, prev);
  return 1;
}

static gboolean PlotMouse(GtkWidget *widget, GdkEventExpose *event, gpointer data){
  Plot *plot = (Plot*)data;
  GdkEventButton *bt = (GdkEventButton*)event;
  //printf("On Press (%f %f) %i\n", bt->x, bt->y, bt->button); //1=left, 2=mid, 3=right
  plot->abs_x = bt->x;
  plot->abs_y = bt->y;
  plot->rel_x = plot->x_min + (plot->x_max - plot->x_min)*plot->abs_x / plot->w;
  plot->rel_y = plot->y_max - (plot->y_max - plot->y_min)*plot->abs_y / plot->h;
  return 1;
}

static int L_NewPlot(lua_State *L){
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  Plot *plot = (Plot*)malloc(sizeof(Plot));
  plot->x=0; plot->y=0; plot->w=100; plot->h=100;
  plot->cur_size = -0.1;
  plot->x_min=0; plot->x_max=1;
  plot->y_min=0; plot->y_max=1;
  if(lua_gettop(L) >= 5){
    if(lua_isnumber(L, 2))plot->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))plot->y = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4))plot->w = lua_tonumber(L, 4);
    if(lua_isnumber(L, 5))plot->h = lua_tonumber(L, 5);
  }
  
  plot->pool_idx = mk_blank_table(L, plot, L_Plot_GC);
  lua_createtable(L, 0, 0);
  lua_setfield(L, -2, "data");
  lua_createtable(L, 0, 0);
  lua_setfield(L, -2, "format");
  lua_pushcfunction(L, L_Plot_draw);
  lua_setfield(L, -2, "Refresh");
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_Plotsetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_Plotgetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_Plotpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  
  plot->obj = gtk_drawing_area_new();
  gtk_widget_set_size_request(plot->obj, plot->w, plot->h);
  plot->fontsize = -0.03;
  
  gtk_fixed_put(GTK_FIXED(cont), plot->obj, plot->x, plot->y);
  gtk_widget_show(plot->obj);
  g_signal_connect(G_OBJECT(plot->obj), "draw", G_CALLBACK(PlotOnDraw), plot);
  
  gtk_widget_add_events(plot->obj, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK);
  g_signal_connect(G_OBJECT(plot->obj), "motion-notify-event", G_CALLBACK(PlotMouse), plot);
  g_signal_connect(G_OBJECT(plot->obj), "button_press_event",  G_CALLBACK(PlotMouse), plot);
  return 1;
}

void plot_reg(lua_State *L){
  lua_pushcfunction(L, L_NewPlot);
  lua_setfield(L, -2, "NewPlot");
#ifdef DEBUG
  printf("Plot registred\n");
#endif
}
