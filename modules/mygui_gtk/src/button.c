#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <stdlib.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  lua_State *L;
  int pool_idx;
  char was_clicked;
}Button;


static void BtnOnClick(GtkWidget *obj, gpointer data){
  Button *btn = data;
  lua_State *L = btn->L;
  int prev = lua_gettop(L);
  //lua_settop(L, 0);
  read_self(L, btn->pool_idx);
          
  lua_getfield(L, -1, "OnClick");
  lua_pushvalue(L, -2);
  if(lua_isfunction(L, -2)){
    lua_pcall(L, 1, 0, 0);
    lua_pop(L, 1);
  }else{
    btn->was_clicked = 1;
    lua_pop(L, 3);
  }
  lua_settop(L, prev);
}

static int L_BtnWasClicked(lua_State *L){
  int top = lua_gettop(L);
  Button *btn = (Button*)read_handle(L, -1, NULL);
  char res = btn->was_clicked;
  btn->was_clicked = 0;

  lua_settop(L, top);
  lua_pushboolean(L, res);
  return 1;
}

static int L_Btn_GC(lua_State *L){
  printf("Btn GC\n");
  int top = lua_gettop(L);
  Button *btn = (Button*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(btn->obj))gtk_widget_destroy(btn->obj);
  free(btn);
  lua_settop(L, top);
  return 0;
}

static int L_NewBtn(lua_State *L){
  Wnd *wnd = NULL;
  int x=0, y=0;
  const char *caption="NONAME";
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  wnd = (Wnd*)read_handle(L, 1, NULL);
  Button *btn = (Button*)malloc(sizeof(Button));
  
  if(lua_gettop(L) >= 4){
    if(lua_isnumber(L, 2))x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))y = lua_tonumber(L, 3);
    if(lua_isstring(L, 4))caption = lua_tostring(L, 4);
  }
  
  btn->pool_idx = mk_blank_table(L, btn, L_Btn_GC);
  lua_pushcfunction(L, L_BtnWasClicked);
  lua_setfield(L, -2, "WasClicked");
  
  btn->L = L;
  btn->obj = gtk_button_new_with_label(caption);
  btn->was_clicked = 0;
  
  gtk_fixed_put(GTK_FIXED(wnd->fixed), btn->obj, x, y);
  gtk_widget_show(btn->obj);
  g_signal_connect(G_OBJECT(btn->obj), "clicked", G_CALLBACK(BtnOnClick), btn);
  return 1;
}

void button_reg(lua_State *L){
  lua_pushcfunction(L, L_NewBtn);
  lua_setfield(L, -2, "NewButton");
#ifdef DEBUG
  printf("Button registred\n");
#endif
}
