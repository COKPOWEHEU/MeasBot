#include <stdlib.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  int pool_idx;
  GtkWidget *fixed;
}Wnd;

struct WndIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_enabled(lua_State *L, int tblindex){
  Wnd *wnd = (Wnd*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(wnd->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  Wnd *wnd = (Wnd*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(wnd->obj));
  lua_pushboolean(L, en);
  return 1;
}
static int setter_resizable(lua_State *L, int tblindex){
  Wnd *wnd = (Wnd*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_window_set_resizable(GTK_WINDOW(wnd->obj), en);
  return 0;
}
static int getter_resizable(lua_State *L, int tblindex){
  Wnd *wnd = (Wnd*)read_handle(L, tblindex, NULL);
  char en = gtk_window_get_resizable(GTK_WINDOW(wnd->obj));
  lua_pushboolean(L, en);
  return 1;
}
static int setter_visible(lua_State *L, int tblindex){
  Wnd *wnd = (Wnd*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_visible(GTK_WIDGET(wnd->obj), en);
  return 0;
}
static int getter_visible(lua_State *L, int tblindex){
  Wnd *wnd = (Wnd*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_visible(GTK_WIDGET(wnd->obj));
  lua_pushboolean(L, en);
  return 1;
}

struct WndIntVariables wnd_intvars[] = {
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
  {.name = "resizable", .setter = setter_resizable, .getter = getter_resizable},
  {.name = "visible", .setter = setter_visible, .getter = getter_visible},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_Wnditerator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(wnd_intvars)){
    lua_pushstring(L, wnd_intvars[val].name); //index
    wnd_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_Wndpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_Wnditerator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_Wndgetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(wnd_intvars); i++){
    if(strcmp(idx, wnd_intvars[i].name)==0){
      return wnd_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_Wndsetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(wnd_intvars); i++){
    if(strcmp(idx, wnd_intvars[i].name)==0){
      return wnd_intvars[i].setter(L, -3);
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

static int L_Wnd_GC(lua_State *L){
#ifdef DEBUG
  printf("Window GC\n");
#endif
  int top = lua_gettop(L);
  Wnd *wnd = (Wnd*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(wnd->obj))gtk_widget_destroy(wnd->obj);
  free_index(L, wnd->pool_idx);
  free(wnd);
  lua_settop(L, top);
  return 0;
}

GtkWidget* Wnd_GetHandle(lua_State *L, int tblindex){
  int top = lua_gettop(L);
  Wnd *wnd = (Wnd*)read_handle(L, tblindex, NULL);
  lua_settop(L, top);
  if(wnd == NULL)return NULL;
  return wnd->obj;
}

gboolean Wnd_OnDelete(GtkWidget *widget, GdkEvent *event, gpointer data){
  //if(data == gui.wndmain)return 0;
  if(data == gui.wndmain)gui.openedwindows--;
  Wnd *wnd = data;
  gtk_widget_hide(GTK_WIDGET(wnd->obj));
  return 1;
}

void Wnd_OnShow(GtkWidget *obj, gpointer data){
  Wnd *wnd = data;
  gui.openedwindows++;
  lua_State *L = gui.L;
  int prev = lua_gettop(L);
  read_self(L, wnd->pool_idx);
  lua_getfield(L, -1, "OnShow");
  lua_pushvalue(L, -2);
  if(lua_isfunction(L, -2)){
    lua_pcall(L, 1, 0, 0);
    lua_pop(L, 1);
  }else{
    lua_pop(L, 3);
  }
  lua_settop(L, prev);
}

void Wnd_OnHide(GtkWidget *obj, gpointer data){
  Wnd *wnd = data;
  gui.openedwindows--;
  lua_State *L = gui.L;
  int prev = lua_gettop(L);
  read_self(L, wnd->pool_idx);
  lua_getfield(L, -1, "OnHide");
  lua_pushvalue(L, -2);
  if(lua_isfunction(L, -2)){
    lua_pcall(L, 1, 0, 0);
    lua_pop(L, 1);
  }else{
    lua_pop(L, 3);
  }
  lua_settop(L, prev);
}

static int L_Show(lua_State *L){
  char newstate = 1;
  int top = lua_gettop(L);
  if(lua_isboolean(L, 2))newstate = lua_toboolean(L, 2);
  Wnd *wnd = (Wnd*)read_handle(L, 1, NULL);
  if(newstate){
    gtk_widget_show(wnd->obj);
    gtk_widget_show(wnd->fixed);
  }else{
    gtk_widget_hide(wnd->obj);
  }
  lua_settop(L, top);
  return 0;
}

static int L_Hide(lua_State *L){
  int top = lua_gettop(L);
  Wnd *wnd = (Wnd*)read_handle(L, -1, NULL);
  gtk_widget_hide(wnd->obj);
  lua_settop(L, top);
  return 0;
}

static int L_NewWnd(lua_State *L){
  Wnd *wnd = (Wnd*)malloc(sizeof(Wnd));
  int w=0, h=0;
  const char *caption="NONAME";
  //NewWindow(w, h, caption)
  if(lua_gettop(L) >= 3){
    if(lua_isnumber(L, 2))w = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))h = lua_tonumber(L, 3);
  }
  if(lua_gettop(L) >= 4){
    if(lua_isstring(L, 4))caption = lua_tostring(L, 4);
  }
  
  wnd->obj = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(wnd->obj), caption);
  if(w > 0 && h > 0)gtk_window_set_default_size(GTK_WINDOW(wnd->obj), w, h);
  wnd->fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(wnd->obj), wnd->fixed);
  
  
  wnd->pool_idx = mk_blank_table(L, wnd, L_Wnd_GC);
  
  lua_getmetatable(L, -1);
    lua_pushlightuserdata(L, wnd->fixed);
    lua_setfield(L, -2, "gtk_container");
    lua_pushcfunction(L, L_Wndsetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_Wndgetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_Wndpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  lua_pushcfunction(L, L_Show);
  lua_setfield(L, -2, "Show");
  lua_pushcfunction(L, L_Hide);
  lua_setfield(L, -2, "Hide");
  REGFUNCS
  
  
  g_signal_connect(G_OBJECT(wnd->obj), "show", G_CALLBACK(Wnd_OnShow), wnd);
  g_signal_connect(G_OBJECT(wnd->obj), "hide", G_CALLBACK(Wnd_OnHide), wnd);
  g_signal_connect(G_OBJECT(wnd->obj), "delete-event", G_CALLBACK(Wnd_OnDelete), wnd);
  
  gtk_widget_show(wnd->obj);
  gtk_widget_show(wnd->fixed);
  return 1;
}

void window_reg(lua_State *L){
  lua_pushcfunction(L, L_NewWnd);
  lua_setfield(L, -2, "NewWindow");
#ifdef DEBUG
  printf("Window registred\n");
#endif
}
