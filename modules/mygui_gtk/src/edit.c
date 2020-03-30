#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <stdlib.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  int pool_idx;
}Edit;

int L_Ed_GC(lua_State *L){
  int top = lua_gettop(L);
  Edit *ed = read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(ed->obj))gtk_widget_destroy(ed->obj);
  free(ed);
  lua_settop(L, top);
  return 0;
}

static int L_Ed_SetText(lua_State *L){
  const char *text;
  if(!lua_isstring(L, -1)){
    printf("Not string\n");
    return 0;
  }
  text = lua_tostring(L, -1);
  if(!lua_istable(L, -2)){
    printf("Edit:Err\n");
    return 0;
  }
  luaL_getmetafield(L, -2, "handle");
  if(!lua_islightuserdata(L, -1)){
    printf("Edit:Err2\n");
    return 0;
  }
  Edit *ed = (Edit*)lua_topointer(L, -1);
  
  gtk_entry_set_text((GtkEntry*)(ed->obj), text);
  lua_pop(L, 2);
  return 0;
}

static int L_Ed_GetText(lua_State *L){
  if(!lua_istable(L, -1)){
    printf("Edit:Err\n");
    return 0;
  }
  lua_getmetatable(L, -1);
  lua_getfield(L, -1, "handle");
  if(!lua_islightuserdata(L, -1)){
    printf("Edit:Err2\n");
    return 0;
  }
  Edit *ed = (Edit*)lua_topointer(L, -1);
  const char *text = gtk_entry_get_text((GtkEntry*)(ed->obj));
  lua_pushstring(L, text);
  return 1;
}

static int L_NewEdit(lua_State *L){
  Wnd *wnd = NULL;
  int x=0, y=0;
  const char *text="";
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  wnd = (Wnd*)read_handle(L, 1, NULL);
  Edit *ed = (Edit*)malloc(sizeof(Edit));
  
  if(lua_gettop(L) >= 4){
    if(lua_isnumber(L, 2))x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))y = lua_tonumber(L, 3);
    if(lua_isstring(L, 4))text = lua_tostring(L, 4);
  }
  
  ed->pool_idx = mk_blank_table(L, ed, L_Ed_GC);
  lua_pushcfunction(L, L_Ed_GetText);
  lua_setfield(L, -2, "GetText");
  lua_pushcfunction(L, L_Ed_SetText);
  lua_setfield(L, -2, "SetText");
  
  ed->obj = gtk_entry_new();
  gtk_entry_set_text((GtkEntry*)(ed->obj), text);
  
  gtk_fixed_put(GTK_FIXED(wnd->fixed), ed->obj, x, y);
  gtk_widget_show(ed->obj);
  
  return 1;
}

void edit_reg(lua_State *L){
  lua_pushcfunction(L, L_NewEdit);
  lua_setfield(L, -2, "NewEdit");
#ifdef DEBUG
  printf("Edit registred\n");
#endif
}
