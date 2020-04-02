#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <stdlib.h>
#include "common.h"
/*
typedef struct{
  GtkWidget *obj;
  int pool_idx;
  int x, y, w, h;
}Progress;

struct IntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_min(lua_State *L, int tblindex){
  float min = lua_tonumber(L, tblindex+2);
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  gtk_level_bar_set_min_value(GTK_LEVEL_BAR(pb->obj), min);
  printf("Set min = %f\n", min);
  return 0;
}
static int getter_min(lua_State *L, int tblindex){
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  float min = gtk_level_bar_get_min_value(GTK_LEVEL_BAR(pb->obj));
  lua_pushnumber(L, min);
  return 1;
}

static int setter_max(lua_State *L, int tblindex){
  float max = lua_tonumber(L, tblindex+2);
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  gtk_level_bar_set_max_value(GTK_LEVEL_BAR(pb->obj), max);
  printf("Set max = %f\n", max);
  return 0;
}
static int getter_max(lua_State *L, int tblindex){
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  float max = gtk_level_bar_get_max_value(GTK_LEVEL_BAR(pb->obj));
  lua_pushnumber(L, max);
  return 1;
}
static int setter_val(lua_State *L, int tblindex){
  float val = lua_tonumber(L, tblindex+2);
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  gtk_level_bar_set_value(GTK_LEVEL_BAR(pb->obj), val);
  return 0;
}
static int getter_val(lua_State *L, int tblindex){
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  float val = gtk_level_bar_get_value(GTK_LEVEL_BAR(pb->obj));
  lua_pushnumber(L, val);
  return 1;
}
static int setter_frac(lua_State *L, int tblindex){
  float val = lua_tonumber(L, tblindex+2);
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  float min = gtk_level_bar_get_min_value(GTK_LEVEL_BAR(pb->obj));
  float max = gtk_level_bar_get_max_value(GTK_LEVEL_BAR(pb->obj));
  gtk_level_bar_set_value(GTK_LEVEL_BAR(pb->obj), min + val*(max-min));
  return 0;
}
static int getter_frac(lua_State *L, int tblindex){
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  float val = gtk_level_bar_get_value(GTK_LEVEL_BAR(pb->obj));
  float min = gtk_level_bar_get_min_value(GTK_LEVEL_BAR(pb->obj));
  float max = gtk_level_bar_get_max_value(GTK_LEVEL_BAR(pb->obj));
  lua_pushnumber(L, (val-min)/(max-min));
  return 1;
}

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  pb->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(pb->obj)), pb->obj, pb->x, pb->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, pb->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  pb->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(pb->obj)), pb->obj, pb->x, pb->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, pb->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  pb->w = w;
  gtk_widget_set_size_request(pb->obj, pb->w, pb->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, pb->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  float h = lua_tonumber(L, tblindex+2);
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  pb->h = h;
  gtk_widget_set_size_request(pb->obj, pb->w, pb->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  Progress *pb = (Progress*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, pb->h);
  return 1;
}

struct IntVariables progressbar_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "min", .setter = setter_min, .getter = getter_min},
  {.name = "max", .setter = setter_max, .getter = getter_max},
  {.name = "value", .setter = setter_val, .getter = getter_val},
  {.name = "val", .setter = setter_val, .getter = getter_val},
  {.name = "fraction", .setter = setter_frac, .getter = getter_frac},
  {.name = "frac", .setter = setter_frac, .getter = getter_frac},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_iterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(progressbar_intvars)){
    lua_pushstring(L, progressbar_intvars[val].name); //index
    progressbar_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_pairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_iterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_getter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(progressbar_intvars); i++){
    if(strcmp(idx, progressbar_intvars[i].name)==0){
      return progressbar_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_setter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(progressbar_intvars); i++){
    if(strcmp(idx, progressbar_intvars[i].name)==0){
      return progressbar_intvars[i].setter(L, -3);
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

static int L_Progress_GC(lua_State *L){
  printf("Progress GC\n");
  int top = lua_gettop(L);
  Progress *pb = (Progress*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(pb->obj))gtk_widget_destroy(pb->obj);
  free_index(L, pb->pool_idx);
  free(pb);
  lua_settop(L, top);
  return 0;
}

static int L_NewProgress(lua_State *L){
  //int x=0, y=0, w=100, h=10;
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  Progress *pb = (Progress*)malloc(sizeof(Progress));
  pb->x = 0; pb->y = 0; pb->w = 100; pb->h = 10;
  
  if(lua_gettop(L) >= 5){
    if(lua_isnumber(L, 2))pb->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))pb->y = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4))pb->w = lua_tonumber(L, 4);
    if(lua_isnumber(L, 5))pb->h = lua_tonumber(L, 5);
  }
  
  pb->pool_idx = mk_blank_table(L, pb, L_Progress_GC);
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_setter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_getter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_pairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  
  pb->obj = gtk_level_bar_new();
  gtk_widget_set_size_request(pb->obj, pb->w, pb->h);
  gtk_fixed_put(GTK_FIXED(cont), pb->obj, pb->x, pb->y);
  gtk_widget_show(pb->obj);
  return 1;
}

void progressbar_reg(lua_State *L){
  lua_pushcfunction(L, L_NewProgress);
  lua_setfield(L, -2, "NewProgressBar");
#ifdef DEBUG
  printf("Progress registred\n");
#endif
}
*/
void progressbar_reg(lua_State *L){
#warning PROGRESSBAR DOESNT WORK
}
