#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <stdlib.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  int pool_idx;
  char was_clicked;
}ChkBox;

static int L_ChkBox_IsChecked(lua_State *L){
  int top = lua_gettop(L);
  ChkBox *btn = (ChkBox*)read_handle(L, -1, NULL);
  char res = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn->obj));

  lua_settop(L, top);
  lua_pushboolean(L, res);
  return 1;
}

static int L_ChkBox_SetChecked(lua_State *L){
  int top = lua_gettop(L);
  if(!lua_isboolean(L, -1))return 0;
  char res = lua_toboolean(L, -1);
  ChkBox *btn = (ChkBox*)read_handle(L, -2, NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btn->obj), res);

  lua_settop(L, top);
  return 0;
}

static int L_ChkBox_GC(lua_State *L){
  printf("Checkbox GC\n");
  int top = lua_gettop(L);
  ChkBox *btn = (ChkBox*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(btn->obj))gtk_widget_destroy(btn->obj);
  free_index(L, btn->pool_idx);
  free(btn);
  lua_settop(L, top);
  return 0;
}

static int L_NewChkBox(lua_State *L){
  int x=0, y=0;
  const char *caption="NONAME";
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  ChkBox *btn = (ChkBox*)malloc(sizeof(ChkBox));
  
  if(lua_gettop(L) >= 4){
    if(lua_isnumber(L, 2))x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))y = lua_tonumber(L, 3);
    if(lua_isstring(L, 4))caption = lua_tostring(L, 4);
  }
  
  btn->pool_idx = mk_blank_table(L, btn, L_ChkBox_GC);
  lua_pushcfunction(L, L_ChkBox_IsChecked);
  lua_setfield(L, -2, "IsChecked");
  lua_pushcfunction(L, L_ChkBox_SetChecked);
  lua_setfield(L, -2, "SetChecked");  
  
  btn->obj = gtk_check_button_new_with_label(caption);
  
  gtk_fixed_put(GTK_FIXED(cont), btn->obj, x, y);
  gtk_widget_show(btn->obj);
  return 1;
}

void checkbox_reg(lua_State *L){
  lua_pushcfunction(L, L_NewChkBox);
  lua_setfield(L, -2, "NewCheckBox");
#ifdef DEBUG
  printf("ChkBox registred\n");
#endif
}
