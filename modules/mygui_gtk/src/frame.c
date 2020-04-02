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
  GtkWidget *fixed;
}Frame;

static int L_Frame_GC(lua_State *L){
#ifdef DEBUG
  printf("TabControl GC\n");
#endif
  int top = lua_gettop(L);
  Frame *frame = (Frame*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(frame->obj))gtk_widget_destroy(frame->obj);
  if(frame){
    free_index(L, frame->pool_idx);
    free(frame);
  }
  lua_settop(L, top);
  return 0;
}

static int L_NewFrame(lua_State *L){
  Frame *frame = (Frame*)malloc(sizeof(Frame));
  int x=0, y=0, w=100, h=100;
  const char *text = NULL;//"";
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
  if(lua_gettop(L) >= 6){
    if(lua_isstring(L, 6))text = lua_tostring(L, 6);
  }
  
  frame->obj = gtk_frame_new(text);
  frame->fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(frame->obj), frame->fixed);
  gtk_widget_set_size_request(frame->obj, w, h);
  gtk_fixed_put(GTK_FIXED(cont), frame->obj, x, y);
  
  frame->pool_idx = mk_blank_table(L, frame, L_Frame_GC);
  lua_getmetatable(L, -1);
    lua_pushlightuserdata(L, frame->fixed);
    lua_setfield(L, -2, "gtk_container");
  lua_setmetatable(L, -2);
  REGFUNCS //добавляем все элементы, доступные для окна
  
  gtk_widget_show(frame->obj);
  gtk_widget_show(frame->fixed);
  return 1;
}

void frame_reg(lua_State *L){
  lua_pushcfunction(L, L_NewFrame);
  lua_setfield(L, -2, "NewFrame");
#ifdef DEBUG
  printf("TabControl registred\n");
#endif
}
