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
}Button;

struct ButtonIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  btn->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(btn->obj)), btn->obj, btn->x, btn->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  btn->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(btn->obj)), btn->obj, btn->x, btn->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  btn->w = w;
  gtk_widget_set_size_request(btn->obj, btn->w, btn->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  gtk_widget_get_size_request(GTK_WIDGET(btn->obj), &(btn->w), &(btn->h));
  lua_pushnumber(L, btn->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  btn->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(btn->obj, btn->w, btn->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  gtk_widget_get_size_request(GTK_WIDGET(btn->obj), &(btn->w), &(btn->h));
  lua_pushnumber(L, btn->h);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(btn->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  Button *btn = (Button*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(btn->obj));
  lua_pushboolean(L, en);
  return 1;
}

struct ButtonIntVariables button_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_Buttoniterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(button_intvars)){
    lua_pushstring(L, button_intvars[val].name); //index
    button_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_Buttonpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_Buttoniterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_Buttongetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(button_intvars); i++){
    if(strcmp(idx, button_intvars[i].name)==0){
      return button_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_Buttonsetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(button_intvars); i++){
    if(strcmp(idx, button_intvars[i].name)==0){
      return button_intvars[i].setter(L, -3);
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

static void BtnOnClick(GtkWidget *obj, gpointer data){
  Button *btn = data;
  lua_State *L = gui.L;
  int prev = lua_gettop(L);
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
#ifdef DEBUG
  printf("Btn GC\n");
#endif
  int top = lua_gettop(L);
  Button *btn = (Button*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(btn->obj))gtk_widget_destroy(btn->obj);
  free_index(L, btn->pool_idx);
  free(btn);
  lua_settop(L, top);
  return 0;
}

static int L_NewBtn(lua_State *L){
  const char *caption="NONAME";
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  Button *btn = (Button*)malloc(sizeof(Button));
  btn->x = 0; btn->y = 0;
  if(lua_gettop(L) >= 3){
    if(lua_isnumber(L, 2))btn->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))btn->y = lua_tonumber(L, 3);
  }
  if(lua_isstring(L, 4))caption = lua_tostring(L, 4);
  
  btn->pool_idx = mk_blank_table(L, btn, L_Btn_GC);
  lua_pushcfunction(L, L_BtnWasClicked);
  lua_setfield(L, -2, "WasClicked");
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_Buttonsetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_Buttongetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_Buttonpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  
  btn->obj = gtk_button_new_with_label(caption);
  btn->was_clicked = 0;
  
  gtk_fixed_put(GTK_FIXED(cont), btn->obj, btn->x, btn->y);
  gtk_widget_show(btn->obj);
  gtk_widget_get_size_request(GTK_WIDGET(btn->obj), &(btn->w), &(btn->h));
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
