#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  int pool_idx;
  char was_clicked;
  int x, y, w, h;
}ChkBox;

struct ChkBoxIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  btn->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(btn->obj)), btn->obj, btn->x, btn->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  btn->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(btn->obj)), btn->obj, btn->x, btn->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  btn->w = w;
  gtk_widget_set_size_request(btn->obj, btn->w, btn->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  btn->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(btn->obj, btn->w, btn->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->h);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(btn->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(btn->obj));
  lua_pushboolean(L, en);
  return 1;
}
static int setter_checked(lua_State *L, int tblindex){
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  char res = lua_toboolean(L, tblindex+2);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btn->obj), res);
  return 0;
}
static int getter_checked(lua_State *L, int tblindex){
  ChkBox *btn = (ChkBox*)read_handle(L, tblindex, NULL);
  char res = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn->obj));
  lua_pushboolean(L, res);
  return 1;
}

struct ChkBoxIntVariables chkbox_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
  {.name = "checked", .setter = setter_checked, .getter = getter_checked},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_ChkBoxiterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(chkbox_intvars)){
    lua_pushstring(L, chkbox_intvars[val].name); //index
    chkbox_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_ChkBoxpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_ChkBoxiterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_ChkBoxgetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(chkbox_intvars); i++){
    if(strcmp(idx, chkbox_intvars[i].name)==0){
      return chkbox_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_ChkBoxsetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(chkbox_intvars); i++){
    if(strcmp(idx, chkbox_intvars[i].name)==0){
      return chkbox_intvars[i].setter(L, -3);
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

static int L_ChkBox_GC(lua_State *L){
#ifdef DEBUG
  printf("Checkbox GC\n");
#endif
  int top = lua_gettop(L);
  ChkBox *btn = (ChkBox*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(btn->obj))gtk_widget_destroy(btn->obj);
  free_index(L, btn->pool_idx);
  free(btn);
  lua_settop(L, top);
  return 0;
}

static int L_NewChkBox(lua_State *L){
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
  btn->x = 0; btn->y = 0;
  if(lua_gettop(L) >= 4){
    if(lua_isnumber(L, 2))btn->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))btn->y = lua_tonumber(L, 3);
    if(lua_isstring(L, 4))caption = lua_tostring(L, 4);
  }
  
  btn->pool_idx = mk_blank_table(L, btn, L_ChkBox_GC);
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_ChkBoxsetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_ChkBoxgetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_ChkBoxpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  
  btn->obj = gtk_check_button_new_with_label(caption);
  
  gtk_fixed_put(GTK_FIXED(cont), btn->obj, btn->x, btn->y);
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
