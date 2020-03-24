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
#include "window.h"

static int L_Wnd_GC(lua_State *L){
  printf("Window GC\n");
  return 0;
}

void Wnd_OnDestroy(GtkWidget *obj, gpointer data){
  Wnd *wnd = data;
  wnd->runflag = 0;
  lua_State *L = wnd->L;
  int prev = lua_gettop(L);
  lua_settop(L, 0);
  read_self(L, wnd->pool_idx);
  lua_getfield(L, -1, "OnDestroy");
  lua_pushvalue(L, -2);
  if(lua_isfunction(L, -2)){
    lua_pcall(L, 1, 0, 0);
    lua_pop(L, 1);
  }else{
    lua_pop(L, 3);
  }
  lua_settop(L, prev);
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
  
  wnd->pool_idx = mk_blank_table(L, wnd, L_Wnd_GC);
  
  wnd->L = L;
  wnd->runflag = 1;
  wnd->obj = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(wnd->obj), caption);
  if(w > 0 && h > 0)gtk_window_set_default_size(GTK_WINDOW(wnd->obj), w, h);
  wnd->fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(wnd->obj), wnd->fixed);
  g_signal_connect(G_OBJECT(wnd->obj), "destroy", G_CALLBACK(Wnd_OnDestroy), wnd);
  
  REGFUNCS
  
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
