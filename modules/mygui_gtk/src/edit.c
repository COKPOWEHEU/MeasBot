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
  int x, y, w, h;
}Edit;

struct EditIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  ed->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(ed->obj)), ed->obj, ed->x, ed->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, ed->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  ed->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(ed->obj)), ed->obj, ed->x, ed->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, ed->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  ed->w = w;
  gtk_widget_set_size_request(ed->obj, ed->w, ed->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  gtk_widget_get_size_request(GTK_WIDGET(ed->obj), &(ed->w), &(ed->h));
  lua_pushnumber(L, ed->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  ed->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(ed->obj, ed->w, ed->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  gtk_widget_get_size_request(GTK_WIDGET(ed->obj), &(ed->w), &(ed->h));
  lua_pushnumber(L, ed->h);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(ed->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(ed->obj));
  lua_pushboolean(L, en);
  return 1;
}

static int setter_text(lua_State *L, int tblindex){
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  const char *text = lua_tostring(L, tblindex+2);
  gtk_entry_set_text((GtkEntry*)(ed->obj), text);
  return 0;
}

static int getter_text(lua_State *L, int tblindex){
  Edit *ed = (Edit*)read_handle(L, tblindex, NULL);
  const char *text = gtk_entry_get_text((GtkEntry*)(ed->obj));
  lua_pushstring(L, text);
  return 1;
}

struct EditIntVariables edit_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
  {.name = "text", .setter = setter_text, .getter = getter_text},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_Edititerator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(edit_intvars)){
    lua_pushstring(L, edit_intvars[val].name); //index
    edit_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_Editpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_Edititerator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_Editgetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(edit_intvars); i++){
    if(strcmp(idx, edit_intvars[i].name)==0){
      return edit_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_Editsetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(edit_intvars); i++){
    if(strcmp(idx, edit_intvars[i].name)==0){
      return edit_intvars[i].setter(L, -3);
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

int L_Ed_GC(lua_State *L){
#ifdef DEBUG
  printf("Edit GC\n");
#endif
  int top = lua_gettop(L);
  Edit *ed = read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(ed->obj))gtk_widget_destroy(ed->obj);
  free(ed);
  lua_settop(L, top);
  return 0;
}

static int L_NewEdit(lua_State *L){
  int x=0, y=0;
  const char *text="";
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  Edit *ed = (Edit*)malloc(sizeof(Edit));
  
  if(lua_gettop(L) >= 4){
    if(lua_isnumber(L, 2))x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))y = lua_tonumber(L, 3);
  }
  if(lua_isstring(L, 4))text = lua_tostring(L, 4);
  
  ed->pool_idx = mk_blank_table(L, ed, L_Ed_GC);
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_Editsetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_Editgetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_Editpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  
  ed->obj = gtk_entry_new();
  gtk_entry_set_text((GtkEntry*)(ed->obj), text);
  
  gtk_fixed_put(GTK_FIXED(cont), ed->obj, x, y);
  gtk_widget_show(ed->obj);
  gtk_widget_get_size_request(GTK_WIDGET(ed->obj), &(ed->w), &(ed->h));
  return 1;
}

void edit_reg(lua_State *L){
  lua_pushcfunction(L, L_NewEdit);
  lua_setfield(L, -2, "NewEdit");
#ifdef DEBUG
  printf("Edit registred\n");
#endif
}
