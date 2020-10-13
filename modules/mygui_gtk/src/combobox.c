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
  struct{
    char *arr;
    size_t count;
    size_t maxwidth;
  }items;
}CbBox;

struct CbBoxIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  cbbox->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(cbbox->obj)), cbbox->obj, cbbox->x, cbbox->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, cbbox->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  cbbox->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(cbbox->obj)), cbbox->obj, cbbox->x, cbbox->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, cbbox->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  cbbox->w = w;
  gtk_widget_set_size_request(cbbox->obj, cbbox->w, cbbox->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, cbbox->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  cbbox->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(cbbox->obj, cbbox->w, cbbox->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, cbbox->h);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(cbbox->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(cbbox->obj));
  lua_pushboolean(L, en);
  return 1;
}
static int setter_selected(lua_State *L, int tblindex){
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  int sel = lua_tonumber(L, tblindex+2);
  gtk_combo_box_set_active(GTK_COMBO_BOX(cbbox->obj), sel-1);
  return 0;
}
static int getter_selected(lua_State *L, int tblindex){
  CbBox *cbbox = (CbBox*)read_handle(L, tblindex, NULL);
  int sel = gtk_combo_box_get_active(GTK_COMBO_BOX(cbbox->obj));
  lua_pushnumber(L, sel+1);
  lua_pushstring(L, &(cbbox->items.arr[sel*(cbbox->items.maxwidth)]));
  return 2;
}

struct CbBoxIntVariables combobox_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
  {.name = "selected", .setter = setter_selected, .getter = getter_selected},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_CbBoxiterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(combobox_intvars)){
    lua_pushstring(L, combobox_intvars[val].name); //index
    combobox_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_CbBoxpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_CbBoxiterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_CbBoxgetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(combobox_intvars); i++){
    if(strcmp(idx, combobox_intvars[i].name)==0){
      return combobox_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_CbBoxsetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(combobox_intvars); i++){
    if(strcmp(idx, combobox_intvars[i].name)==0){
      return combobox_intvars[i].setter(L, -3);
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

static void CbBoxOnChange(GtkWidget *obj, gpointer data){
  CbBox *cbbox = data;
  lua_State *L = gui.L;
  int prev = lua_gettop(L);
  read_self(L, cbbox->pool_idx);
  
  lua_getfield(L, -1, "OnChange");
  lua_pushvalue(L, -2);
  if(lua_isfunction(L, -2)){
    int val = gtk_combo_box_get_active(GTK_COMBO_BOX(cbbox->obj));
    lua_pushnumber(L, val+1);
    lua_pushstring(L, &(cbbox->items.arr[val*(cbbox->items.maxwidth)]));
    lua_pcall(L, 3, 0, 0);
    lua_pop(L, 1);
  }
  lua_settop(L, prev);
}

static int L_CbBox_SetItems(lua_State *L){
  int top = lua_gettop(L);
  CbBox *cbbox = (CbBox*)read_handle(L, 1, NULL);
  if(cbbox == NULL){printf("Call methods as METHODS, not single functions!\n"); return 0;}
  if(cbbox->items.arr != NULL)free(cbbox->items.arr);
  gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(cbbox->obj));
  cbbox->items.count=0; cbbox->items.maxwidth=0;
  if(!lua_istable(L, top)){printf("Items are not array of strings\n"); return 0;}
  lua_len(L, -1);
  size_t count;
  if(lua_isnumber(L, -1))cbbox->items.count = count = lua_tonumber(L, -1);
  lua_pop(L, 1); //pop len
  //ищем самую длинную строку, а заодно игнорируем те строки, которые не строки
  char *curstr;
  for(int i=0; i<cbbox->items.count; i++){
    lua_rawgeti(L, top, i+1);
    if(!lua_isstring(L, -1)){
      cbbox->items.count--;
      continue;
    }
    curstr = (char*)lua_tostring(L, -1);
    int len = strlen(curstr);
    if(len > cbbox->items.maxwidth)cbbox->items.maxwidth=len;
  }
  size_t w = ++cbbox->items.maxwidth;
  //выделяем память и заполняем ее новыми строками
  cbbox->items.arr = (char*)malloc(sizeof(char)*(cbbox->items.count)*w);
  if(cbbox->items.arr == NULL){
    printf("Can not allocate memory\n");
    return 0;
  }
  int num = 0;
  for(int i=0; i<cbbox->items.count; i++){
    lua_rawgeti(L, top, i+1);
    if(!lua_isstring(L, -1))continue;
    curstr = (char*)lua_tostring(L, -1);
    strcpy(&(cbbox->items.arr[num*w]), curstr);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbbox->obj), curstr);
    num++;
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(cbbox->obj), 0);
  return 0;
}

static int L_CbBox_GetItems(lua_State *L){
  CbBox *cbbox = (CbBox*)read_handle(L, 1, NULL);
  if(cbbox == NULL){printf("Call methods as METHODS, not single functions!\n"); return 0;}
  if(cbbox->items.arr == NULL)return 0;
  lua_newtable(L);
  for(int i=0,j=0; i<cbbox->items.count; i++,j+=cbbox->items.maxwidth){
    lua_pushstring(L, &(cbbox->items.arr[j]));
    lua_rawseti(L, -2, i+1);
  }
  return 1;
}

static int L_CbBox_GC(lua_State *L){
#ifdef DEBUG
  printf("Combobox GC\n");
#endif
  int top = lua_gettop(L);
  CbBox *cbbox = (CbBox*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(cbbox->obj))gtk_widget_destroy(cbbox->obj);
  free_index(L, cbbox->pool_idx);
  free(cbbox);
  lua_settop(L, top);
  return 0;
}

static int L_NewCbBox(lua_State *L){
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  CbBox *cbbox = (CbBox*)malloc(sizeof(CbBox));
  cbbox->x = 0; cbbox->y = 0;
  cbbox->items.arr=NULL; cbbox->items.count=0; cbbox->items.maxwidth=0;
  int items_arr = 0;
  int items_sel = 0;
  if(lua_gettop(L) >= 3){
    if(lua_isnumber(L, 2))cbbox->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))cbbox->y = lua_tonumber(L, 3);
  }
  if(lua_gettop(L) >= 4){
    if(lua_istable(L, 4))items_arr = 4;
  }
  if(lua_gettop(L) >= 5){
    if(lua_isnumber(L, 5))items_sel = lua_tonumber(L, 5);
  }
  
  cbbox->pool_idx = mk_blank_table(L, cbbox, L_CbBox_GC);
  lua_pushcfunction(L, L_CbBox_SetItems);
  lua_setfield(L, -2, "SetItems");
  lua_pushcfunction(L, L_CbBox_GetItems);
  lua_setfield(L, -2, "GetItems");
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_CbBoxsetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_CbBoxgetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_CbBoxpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);
  
  cbbox->obj = gtk_combo_box_text_new();
  
  if(items_arr > 0){
    int prev_init = lua_gettop(L);
    lua_pushcfunction(L, L_CbBox_SetItems);
    lua_pushvalue(L, -2); //Combobox object
    lua_pushvalue(L, items_arr); //input array
    lua_pcall(L, 2, 0, 0);
    lua_settop(L, prev_init);
  }
  if(items_sel > 0){
    gtk_combo_box_set_active(GTK_COMBO_BOX(cbbox->obj), items_sel-1);
  }
  
  gtk_fixed_put(GTK_FIXED(cont), cbbox->obj, cbbox->x, cbbox->y);
  gtk_widget_show(cbbox->obj);
  g_signal_connect(G_OBJECT(cbbox->obj), "changed", G_CALLBACK(CbBoxOnChange), cbbox);
  return 1;
}

void combobox_reg(lua_State *L){
  lua_pushcfunction(L, L_NewCbBox);
  lua_setfield(L, -2, "NewComboBox");
#ifdef DEBUG
  printf("ComboBox registred\n");
#endif
}
