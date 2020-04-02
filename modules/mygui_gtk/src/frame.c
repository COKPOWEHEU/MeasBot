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
  GtkWidget *fixed;
  int x, y, w, h;
}Frame;

struct FrameIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  frame->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(frame->obj)), frame->obj, frame->x, frame->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, frame->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  frame->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(frame->obj)), frame->obj, frame->x, frame->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, frame->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  frame->w = w;
  gtk_widget_set_size_request(frame->obj, frame->w, frame->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, frame->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  frame->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(frame->obj, frame->w, frame->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, frame->h);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(frame->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(frame->obj));
  lua_pushboolean(L, en);
  return 1;
}
static int setter_label(lua_State *L, int tblindex){
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  const char *label = NULL;
  if(lua_isstring(L, tblindex+2))label = lua_tostring(L, tblindex+2);
  gtk_frame_set_label(GTK_FRAME(frame->obj), label);
  return 0;
}
static int getter_label(lua_State *L, int tblindex){
  Frame *frame = (Frame*)read_handle(L, tblindex, NULL);
  const char *label = gtk_frame_get_label(GTK_FRAME(frame->obj));
  lua_pushstring(L, label);
  return 1;
}

struct FrameIntVariables frame_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
  {.name = "label", .setter = setter_label, .getter = getter_label},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_Frameiterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(frame_intvars)){
    lua_pushstring(L, frame_intvars[val].name); //index
    frame_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_Framepairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_Frameiterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_Framegetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(frame_intvars); i++){
    if(strcmp(idx, frame_intvars[i].name)==0){
      return frame_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_Framesetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(frame_intvars); i++){
    if(strcmp(idx, frame_intvars[i].name)==0){
      return frame_intvars[i].setter(L, -3);
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
  const char *text = NULL;//"";
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  Frame *frame = (Frame*)malloc(sizeof(Frame));
  frame->x=0; frame->y=0; frame->w=100; frame->h=100;
  if(lua_gettop(L) >= 5){
    if(lua_isnumber(L, 2))frame->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))frame->y = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4))frame->w = lua_tonumber(L, 4);
    if(lua_isnumber(L, 5))frame->h = lua_tonumber(L, 5);
  }
  if(lua_gettop(L) >= 6){
    if(lua_isstring(L, 6))text = lua_tostring(L, 6);
  }
  
  frame->obj = gtk_frame_new(text);
  frame->fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(frame->obj), frame->fixed);
  gtk_widget_set_size_request(frame->obj, frame->w, frame->h);
  gtk_fixed_put(GTK_FIXED(cont), frame->obj, frame->x, frame->y);
  
  frame->pool_idx = mk_blank_table(L, frame, L_Frame_GC);
  lua_getmetatable(L, -1);
    lua_pushlightuserdata(L, frame->fixed);
    lua_setfield(L, -2, "gtk_container");
    lua_pushcfunction(L, L_Framesetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_Framegetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_Framepairs);
    lua_setfield(L, -2, "__pairs");
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
