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
  int x, y, w, h;
}TabCtl;

struct TabCtlIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  tctl->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(tctl->obj)), tctl->obj, tctl->x, tctl->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, tctl->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  tctl->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(tctl->obj)), tctl->obj, tctl->x, tctl->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, tctl->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  tctl->w = w;
  gtk_widget_set_size_request(tctl->obj, tctl->w, tctl->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, tctl->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  tctl->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(tctl->obj, tctl->w, tctl->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, tctl->h);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(tctl->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  TabCtl *tctl = (TabCtl*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(tctl->obj));
  lua_pushboolean(L, en);
  return 1;
}

struct TabCtlIntVariables tctl_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_TabCtliterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(tctl_intvars)){
    lua_pushstring(L, tctl_intvars[val].name); //index
    tctl_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_TabCtlpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_TabCtliterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_TabCtl_default_getter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(tctl_intvars); i++){
    if(strcmp(idx, tctl_intvars[i].name)==0){
      return tctl_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_TabCtl_default_setter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(tctl_intvars); i++){
    if(strcmp(idx, tctl_intvars[i].name)==0){
      return tctl_intvars[i].setter(L, -3);
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

static int L_TabCtl_GC(lua_State *L){
#ifdef DEBUG
  printf("TabControl GC\n");
#endif
  int top = lua_gettop(L);
  TabCtl *tctl = (TabCtl*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(tctl->obj))gtk_widget_destroy(tctl->obj);
  if(tctl){
    free_index(L, tctl->pool_idx);
    free(tctl);
  }
  lua_settop(L, top);
  return 0;
}
//tab[idx] = val;
static int L_SetTab(lua_State *L){
  int top = lua_gettop(L);
  if(!lua_isnumber(L, -2)){
    return L_TabCtl_default_setter(L);
  }
  int num = lua_tonumber(L, -2);
  
  TabCtl *tctl = read_handle(L, -3, NULL);
  
  const char *text = NULL;
  if(lua_isstring(L, -1)){
    text = lua_tostring(L, -1);
  }else if(lua_istable(L, -1)){
    lua_getfield(L, -1, "label");
    if(lua_isstring(L, -1))text = lua_tostring(L, -1);
    lua_pop(L, 1);
    if(text == NULL){
      lua_getfield(L, -1, "text");
      if(lua_isstring(L, -1))text = lua_tostring(L, -1);
      lua_pop(L, 1);
    }
  }else if(lua_isnil(L, -1)){
    //TODO проверить не является ли num последним
    gtk_notebook_remove_page(GTK_NOTEBOOK(tctl->obj), num);
    printf("Delete [%i]\n", num);
    lua_settop(L, top);
    return 0;
  }
  lua_pop(L, 2);
  lua_getmetatable(L, -1);
    lua_getfield(L, -1, "tab"); //real 'tab'
      lua_rawgeti(L, -1, num);
        if(lua_istable(L, -1)){ //элемент уже существует
          printf("Tab [%i] already exist\n", num);
          lua_settop(L, top);
          return 0;
        }
        lua_pop(L, 1);
      lua_createtable(L, 0, 0);
        lua_pushstring(L, text);
        lua_setfield(L, -2, "text");
        REGFUNCS //добавляем все элементы, доступные для окна
        lua_createtable(L, 0, 0);
          GtkWidget *label = gtk_label_new(text);
          GtkWidget *fixed = gtk_fixed_new();
          gtk_notebook_insert_page(GTK_NOTEBOOK(tctl->obj), fixed, label, num);
          gtk_widget_show_all(tctl->obj);
          //lua_pushnumber(L, 100);
          lua_pushlightuserdata(L, fixed);
          lua_setfield(L, -2, "gtk_container");
        lua_setmetatable(L, -2);
      lua_rawseti(L, -2, num);
      printf("Tab [%i] = %s created\n", num, text);
  lua_settop(L, top);
  return 0;
}

// return tab[idx]
static int L_GetTab(lua_State *L){
  int top = lua_gettop(L);
  if(!lua_isnumber(L, -1)){
    return L_TabCtl_default_getter(L);
  }
  int num = lua_tonumber(L, -1);
  lua_getmetatable(L, -2);
    lua_getfield(L, -1, "tab"); //real 'tab'
      lua_rawgeti(L, -1, num);
        if(lua_isnil(L, -1)){ //такого элемента не существует
          lua_settop(L, top);
          printf("tab [%i] does not exist\n", num);
          return 0;
        }
        return 1;
  
  lua_settop(L, top);
  lua_pushstring(L, "Tab");
  return 1;
}

static int L_NewTabCtl(lua_State *L){
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  TabCtl *tctl = (TabCtl*)malloc(sizeof(TabCtl));
  tctl->x=0; tctl->y=0; tctl->w=100; tctl->h=100;
  GtkWidget *cont = read_container(L, 1, NULL);
  if(lua_gettop(L) >= 5){
    if(lua_isnumber(L, 2))tctl->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))tctl->y = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4))tctl->w = lua_tonumber(L, 4);
    if(lua_isnumber(L, 5))tctl->h = lua_tonumber(L, 5);
  }
  
  tctl->obj = gtk_notebook_new();
  gtk_widget_set_size_request(tctl->obj, tctl->w, tctl->h);
  gtk_fixed_put(GTK_FIXED(cont), tctl->obj, tctl->x, tctl->y);
  
  tctl->pool_idx = mk_blank_table(L, tctl, L_TabCtl_GC);
  lua_getmetatable(L, -1);
    lua_createtable(L, 0, 0);
    lua_setfield(L, -2, "tab");
    lua_pushcfunction(L, L_TabCtlpairs);
    lua_setfield(L, -2, "__pairs");
    lua_pushcfunction(L, L_SetTab);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_GetTab);
    lua_setfield(L, -2, "__index");
  lua_setmetatable(L, -2);
  
  gtk_widget_show(tctl->obj);
  return 1;
}

void tabcontrol_reg(lua_State *L){
  lua_pushcfunction(L, L_NewTabCtl);
  lua_setfield(L, -2, "NewTabControl");
#ifdef DEBUG
  printf("TabControl registred\n");
#endif
}
