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
}Label;

static int L_Lbl_GC(lua_State *L){
  printf("Btn GC\n");
  int top = lua_gettop(L);
  Label *lbl = (Label*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(lbl->obj))gtk_widget_destroy(lbl->obj);
  free_index(L, lbl->pool_idx);
  free(lbl);
  lua_settop(L, top);
  return 0;
}

static int L_SetText(lua_State *L){
  int top = lua_gettop(L);
  const char *text = "DEFAULT";
  if(lua_isstring(L, -1))text = lua_tostring(L, -1);
  Label *lbl = (Label*)read_handle(L, -2, NULL);
  gtk_label_set_text(GTK_LABEL(lbl->obj), text);
  lua_settop(L, top);
  return 0;
}

static int L_GetText(lua_State *L){
  Label *lbl = (Label*)read_handle(L, -1, NULL);
  const char *text = gtk_label_get_text(GTK_LABEL(lbl->obj));
  lua_pushstring(L, text);
  return 1;
}

static int L_NewLbl(lua_State *L){
  Wnd *wnd = NULL;
  int x=0, y=0;
  const char *text="NONAME";
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  wnd = (Wnd*)read_handle(L, 1, NULL);
  Label *lbl = (Label*)malloc(sizeof(Label));
  
  if(lua_gettop(L) >= 4){
    if(lua_isnumber(L, 2))x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))y = lua_tonumber(L, 3);
    if(lua_isstring(L, 4))text = lua_tostring(L, 4);
  }
  
  lbl->pool_idx = mk_blank_table(L, lbl, L_Lbl_GC);
  lua_pushcfunction(L, L_SetText);
  lua_setfield(L, -2, "SetText");
  lua_pushcfunction(L, L_GetText);
  lua_setfield(L, -2, "GetText");
  
  lbl->L = L;
  lbl->obj = gtk_label_new(text);
  
  gtk_fixed_put(GTK_FIXED(wnd->fixed), lbl->obj, x, y);
  gtk_widget_show(lbl->obj);
  return 1;
}

void label_reg(lua_State *L){
  lua_pushcfunction(L, L_NewLbl);
  lua_setfield(L, -2, "NewLabel");
#ifdef DEBUG
  printf("Label registred\n");
#endif
}
