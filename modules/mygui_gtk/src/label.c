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
  int x, y;
  int w, h;
}Label;


struct LabelIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  lbl->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(lbl->obj)), lbl->obj, lbl->x, lbl->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, lbl->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  lbl->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(lbl->obj)), lbl->obj, lbl->x, lbl->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, lbl->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  lbl->w = w;
  if((lbl->w <= 0) || (lbl->h <= 0))return 0;
  gtk_widget_set_size_request(lbl->obj, lbl->w, lbl->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  gtk_widget_get_size_request(GTK_WIDGET(lbl->obj), &(lbl->w), &(lbl->h));
  lua_pushnumber(L, lbl->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  lbl->h = lua_tonumber(L, tblindex+2);
  if((lbl->w <= 0) || (lbl->h <= 0))return 0;
  gtk_widget_set_size_request(lbl->obj, lbl->w, lbl->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  gtk_widget_get_size_request(GTK_WIDGET(lbl->obj), &(lbl->w), &(lbl->h));
  lua_pushnumber(L, lbl->h);
  return 1;
}
static int setter_text(lua_State *L, int tblindex){
  int top = lua_gettop(L);
  const char *text = "DEFAULT";
  if(lua_isstring(L, tblindex+2))text = lua_tostring(L, tblindex+2);
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  gtk_label_set_text(GTK_LABEL(lbl->obj), text);
  lua_settop(L, top);
  return 0;
}
static int getter_text(lua_State *L, int tblindex){
  Label *lbl = (Label*)read_handle(L, tblindex, NULL);
  const char *text = gtk_label_get_text(GTK_LABEL(lbl->obj));
  lua_pushstring(L, text);
  return 1;
}

struct LabelIntVariables label_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height",.setter = setter_height,.getter = getter_height},
  {.name = "text", .setter = setter_text, .getter = getter_text},
  {.name = "label", .setter = setter_text, .getter = getter_text},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_Labeliterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(label_intvars)){
    lua_pushstring(L, label_intvars[val].name); //index
    label_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_Labelpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_Labeliterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_Labelgetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(label_intvars); i++){
    if(strcmp(idx, label_intvars[i].name)==0){
      return label_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_Labelsetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(label_intvars); i++){
    if(strcmp(idx, label_intvars[i].name)==0){
      return label_intvars[i].setter(L, -3);
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

static int L_Lbl_GC(lua_State *L){
#ifdef DEBUG
  printf("Btn GC\n");
#endif
  int top = lua_gettop(L);
  Label *lbl = (Label*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(lbl->obj))gtk_widget_destroy(lbl->obj);
  free_index(L, lbl->pool_idx);
  free(lbl);
  lua_settop(L, top);
  return 0;
}

static int L_NewLbl(lua_State *L){
  const char *text="";
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  Label *lbl = (Label*)malloc(sizeof(Label));
  lbl->x=0; lbl->y=0;
  if(lua_gettop(L) >= 3){
    if(lua_isnumber(L, 2))lbl->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))lbl->y = lua_tonumber(L, 3);
  }
  if(lua_isstring(L, 4))text = lua_tostring(L, 4);
  
  lbl->pool_idx = mk_blank_table(L, lbl, L_Lbl_GC);
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_Labelsetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_Labelgetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_Labelpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  
  lbl->obj = gtk_label_new(text);
  gtk_label_set_line_wrap(GTK_LABEL(lbl->obj), 1);
  
  
  gtk_fixed_put(GTK_FIXED(cont), lbl->obj, lbl->x, lbl->y);
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
