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
  int x, y, w, h;
  double min, max, step;
}SpinEd;

struct SpinEdIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_min(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  spined->min = lua_tonumber(L, tblindex+2);
  gtk_spin_button_set_range(GTK_SPIN_BUTTON(spined->obj), spined->min, spined->max);
  return 0;
}
static int getter_min(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, spined->min);
  return 1;
}

static int setter_max(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  spined->max = lua_tonumber(L, tblindex+2);
  gtk_spin_button_set_range(GTK_SPIN_BUTTON(spined->obj), spined->min, spined->max);
  return 0;
}
static int getter_max(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  float max = spined->max;
  lua_pushnumber(L, max);
  return 1;
}
static int setter_val(lua_State *L, int tblindex){
  float val = lua_tonumber(L, tblindex+2);
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spined->obj), val);
  return 0;
}
static int getter_val(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  float val = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spined->obj));
  lua_pushnumber(L, val);
  return 1;
}

static int setter_step(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  spined->step = lua_tonumber(L, tblindex+2);
  gtk_spin_button_set_increments(GTK_SPIN_BUTTON(spined->obj), spined->step, 10*spined->step);
  return 0;
}
static int getter_step(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, spined->step);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(spined->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(spined->obj));
  lua_pushboolean(L, en);
  return 1;
}

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  spined->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(spined->obj)), spined->obj, spined->x, spined->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, spined->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  spined->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(spined->obj)), spined->obj, spined->x, spined->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, spined->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  spined->w = w;
  gtk_widget_set_size_request(spined->obj, spined->w, spined->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, spined->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  spined->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(spined->obj, spined->w, spined->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  SpinEd *spined = (SpinEd*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, spined->h);
  return 1;
}

struct SpinEdIntVariables spined_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "min", .setter = setter_min, .getter = getter_min},
  {.name = "max", .setter = setter_max, .getter = getter_max},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "value", .setter = setter_val, .getter = getter_val},
  {.name = "val", .setter = setter_val, .getter = getter_val},
  {.name = "step", .setter = setter_step, .getter = getter_step},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_SpinEditerator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(spined_intvars)){
    lua_pushstring(L, spined_intvars[val].name); //index
    spined_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_SpinEdpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_SpinEditerator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_SpinEdgetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(spined_intvars); i++){
    if(strcmp(idx, spined_intvars[i].name)==0){
      return spined_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_SpinEdsetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(spined_intvars); i++){
    if(strcmp(idx, spined_intvars[i].name)==0){
      return spined_intvars[i].setter(L, -3);
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

static int L_SpinEd_GC(lua_State *L){
  printf("SpinEd GC\n");
  int top = lua_gettop(L);
  SpinEd *spined = (SpinEd*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(spined->obj))gtk_widget_destroy(spined->obj);
  free_index(L, spined->pool_idx);
  free(spined);
  lua_settop(L, top);
  return 0;
}

static int L_NewSpinEd(lua_State *L){
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  SpinEd *spined = (SpinEd*)malloc(sizeof(SpinEd));
  spined->x = 0; spined->y = 0; spined->w = 100; spined->h = 10;
  
  if(lua_gettop(L) >= 3){
    if(lua_isnumber(L, 2))spined->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))spined->y = lua_tonumber(L, 3);
  }
  
  spined->pool_idx = mk_blank_table(L, spined, L_SpinEd_GC);
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_SpinEdsetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_SpinEdgetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_SpinEdpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  
  spined->obj = gtk_spin_button_new_with_range(0, 1, 0.1);
  gtk_fixed_put(GTK_FIXED(cont), spined->obj, spined->x, spined->y);
  gtk_widget_show(spined->obj);
  return 1;
}

void spinedit_reg(lua_State *L){
  lua_pushcfunction(L, L_NewSpinEd);
  lua_setfield(L, -2, "NewSpinEdit");
#ifdef DEBUG
  printf("SpinEd registred\n");
#endif
}
