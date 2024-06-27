#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  int pool_idx;
  char was_clicked;
  int x, y, w, h;
  float min, max, step, val;
}Scroller;

struct ScrollerIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  scrl->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(scrl->obj)), scrl->obj, scrl->x, scrl->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, scrl->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  scrl->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(scrl->obj)), scrl->obj, scrl->x, scrl->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, scrl->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  scrl->w = w;
  gtk_widget_set_size_request(scrl->obj, scrl->w, scrl->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  gtk_widget_get_size_request(GTK_WIDGET(scrl->obj), &(scrl->w), &(scrl->h));
  lua_pushnumber(L, scrl->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  scrl->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(scrl->obj, scrl->w, scrl->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  gtk_widget_get_size_request(GTK_WIDGET(scrl->obj), &(scrl->w), &(scrl->h));
  lua_pushnumber(L, scrl->h);
  return 1;
}
static int setter_min(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  scrl->min = lua_tonumber(L, tblindex+2);
  gtk_range_set_range(GTK_RANGE(scrl->obj), scrl->min, scrl->max);
  return 0;
}
static int getter_min(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, scrl->min);
  return 1;
}
static int setter_max(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  scrl->max = lua_tonumber(L, tblindex+2);
  gtk_range_set_range(GTK_RANGE(scrl->obj), scrl->min, scrl->max);
  return 0;
}
static int getter_max(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, scrl->max);
  return 1;
}
static int setter_step(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  scrl->step = lua_tonumber(L, tblindex+2);
  gtk_range_set_increments(GTK_RANGE(scrl->obj), scrl->step, (scrl->max - scrl->min)/10);
  return 0;
}
static int getter_step(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, scrl->step);
  return 1;
}
static int setter_val(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  scrl->val = lua_tonumber(L, tblindex+2);
  gtk_range_set_value(GTK_RANGE(scrl->obj), scrl->val);
  return 0;
}
static int getter_val(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, scrl->val);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(scrl->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(scrl->obj));
  lua_pushboolean(L, en);
  return 1;
}
static int setter_showval(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  char sh = lua_toboolean(L, tblindex+2);
  gtk_scale_set_draw_value(GTK_SCALE(scrl->obj), sh);
  return 0;
}
static int getter_showval(lua_State *L, int tblindex){
  Scroller *scrl = (Scroller*)read_handle(L, tblindex, NULL);
  char sh = gtk_scale_get_draw_value(GTK_SCALE(scrl->obj));
  lua_pushboolean(L, sh);
  return 1;
}

struct ScrollerIntVariables scroller_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "min", .setter = setter_min, .getter = getter_min},
  {.name = "max", .setter = setter_max, .getter = getter_max},
  {.name = "step", .setter = setter_step, .getter = getter_step},
  {.name = "value", .setter = setter_val, .getter = getter_val},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
  {.name = "show_val", .setter = setter_showval, .getter = getter_showval},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_Scrolleriterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(scroller_intvars)){
    lua_pushstring(L, scroller_intvars[val].name); //index
    scroller_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_Scrollerpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_Scrolleriterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_Scrollergetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(scroller_intvars); i++){
    if(strcmp(idx, scroller_intvars[i].name)==0){
      return scroller_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_Scrollersetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(scroller_intvars); i++){
    if(strcmp(idx, scroller_intvars[i].name)==0){
      return scroller_intvars[i].setter(L, -3);
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

static gboolean ScrlOnChange(GtkWidget *obj, GtkScrollType scroll, gdouble value, gpointer data){
  Scroller *scrl = data;
  lua_State *L = gui.L;
  int prev = lua_gettop(L);
  read_self(L, scrl->pool_idx);
  if(value < scrl->min)value=scrl->min;
  if(value > scrl->max)value=scrl->max;
  scrl->val = value;
  lua_getfield(L, -1, "OnChange");
  lua_pushvalue(L, -2);
  if(lua_isfunction(L, -2)){
    lua_pushnumber(L, scrl->val);
    lua_pcall(L, 2, 0, 0);
    lua_pop(L, 1);
  }else{
    lua_pop(L, 3);
    //printf("Not found\n");
  }
  lua_settop(L, prev);
  return 0;
}

static int L_Btn_GC(lua_State *L){
#ifdef DEBUG
  printf("Scroller GC\n");
#endif
  int top = lua_gettop(L);
  Scroller *scrl = (Scroller*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(scrl->obj))gtk_widget_destroy(scrl->obj);
  free_index(L, scrl->pool_idx);
  free(scrl);
  lua_settop(L, top);
  return 0;
}

static int L_NewScrl(lua_State *L){
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  Scroller *scrl = (Scroller*)malloc(sizeof(Scroller));
  scrl->x = 0; scrl->y = 0; scrl->w = 100; scrl->h = 20;
  if(lua_gettop(L) >= 3){
    if(lua_isnumber(L, 2))scrl->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))scrl->y = lua_tonumber(L, 3);
  }
  if(lua_gettop(L) >= 5){
    if(lua_isnumber(L, 4))scrl->w = lua_tonumber(L, 4);
    if(lua_isnumber(L, 5))scrl->h = lua_tonumber(L, 5);
  }
  scrl->pool_idx = mk_blank_table(L, scrl, L_Btn_GC);
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_Scrollersetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_Scrollergetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_Scrollerpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  
  scrl->min=0; scrl->max=100; scrl->step=1; scrl->val=0;
  scrl->obj = gtk_scale_new_with_range(0, scrl->min, scrl->max, scrl->step);
  gtk_widget_set_size_request(GTK_WIDGET(scrl->obj), scrl->w, scrl->h);
  
  gtk_fixed_put(GTK_FIXED(cont), scrl->obj, scrl->x, scrl->y);
  gtk_widget_show(scrl->obj);
  g_signal_connect(G_OBJECT(scrl->obj), "change-value", G_CALLBACK(ScrlOnChange), scrl);
  gtk_widget_get_size_request(GTK_WIDGET(scrl->obj), &(scrl->w), &(scrl->h));
  return 1;
}
void scroller_reg(lua_State *L){
  lua_pushcfunction(L, L_NewScrl);
  lua_setfield(L, -2, "NewScroller");
#ifdef DEBUG
  printf("Scroller registred\n");
#endif
}
