#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <math.h>
#include <stdlib.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  int pool_idx;
}TabCtl;

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
    //TODO: добавить индексацию по имени (поиск по элементам)
    printf("Index of tab must me integer\n");
    lua_pop(L, top);
    return 0;
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
    //TODO: добавить индексацию по имени (поиск по элементам)
    printf("Index of tab must me integer\n");
    lua_pop(L, top);
    return 0;
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
  TabCtl *tctl = (TabCtl*)malloc(sizeof(TabCtl));
  int x=0, y=0, w=100, h=100;
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  if(lua_gettop(L) >= 5){
    if(lua_isnumber(L, 2))x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))y = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4))w = lua_tonumber(L, 4);
    if(lua_isnumber(L, 5))h = lua_tonumber(L, 5);
  }
  
  tctl->obj = gtk_notebook_new();
  gtk_widget_set_size_request(tctl->obj, w, h);
  gtk_fixed_put(GTK_FIXED(cont), tctl->obj, x, y);
  
  tctl->pool_idx = mk_blank_table(L, tctl, L_TabCtl_GC);
  lua_getmetatable(L, -1);
    lua_createtable(L, 0, 0);
    lua_setfield(L, -2, "tab");
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
