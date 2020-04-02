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
}RadioBtn;

struct RadioBtnIntVariables{
  char *name;
  int (*setter)(lua_State *L, int tblindex);
  int (*getter)(lua_State *L, int tblindex);
};

static int setter_x(lua_State *L, int tblindex){
  float x = lua_tonumber(L, tblindex+2);
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  btn->x = x;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(btn->obj)), btn->obj, btn->x, btn->y);
  return 0;
}
static int getter_x(lua_State *L, int tblindex){
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->x);
  return 1;
}
static int setter_y(lua_State *L, int tblindex){
  float y = lua_tonumber(L, tblindex+2);
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  btn->y = y;
  gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(btn->obj)), btn->obj, btn->x, btn->y);
  return 0;
}
static int getter_y(lua_State *L, int tblindex){
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->y);
  return 1;
}
static int setter_width(lua_State *L, int tblindex){
  float w = lua_tonumber(L, tblindex+2);
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  btn->w = w;
  gtk_widget_set_size_request(btn->obj, btn->w, btn->h);
  return 0;
}
static int getter_width(lua_State *L, int tblindex){
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->w);
  return 1;
}
static int setter_height(lua_State *L, int tblindex){
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  btn->h = lua_tonumber(L, tblindex+2);
  gtk_widget_set_size_request(btn->obj, btn->w, btn->h);
  return 0;
}
static int getter_height(lua_State *L, int tblindex){
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  lua_pushnumber(L, btn->h);
  return 1;
}
static int setter_enabled(lua_State *L, int tblindex){
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  char en = lua_toboolean(L, tblindex+2);
  gtk_widget_set_sensitive(GTK_WIDGET(btn->obj), en);
  return 0;
}
static int getter_enabled(lua_State *L, int tblindex){
  RadioBtn *btn = (RadioBtn*)read_handle(L, tblindex, NULL);
  char en = gtk_widget_get_sensitive(GTK_WIDGET(btn->obj));
  lua_pushboolean(L, en);
  return 1;
}
static int setter_selected(lua_State *L, int tblindex){
  RadioBtn *btn = (RadioBtn*)read_handle(L, 1, NULL);
  gboolean state = lua_toboolean(L, -1);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btn->obj), state);
  return 0;
}
static int getter_selected(lua_State *L, int tblindex){
  RadioBtn *btn = (RadioBtn*)read_handle(L, 1, NULL);
  gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn->obj));
  lua_pushboolean(L, state);
  return 1;
}

struct RadioBtnIntVariables radiobtn_intvars[] = {
  {.name = "x", .setter = setter_x, .getter = getter_x},
  {.name = "y", .setter = setter_y, .getter = getter_y},
  {.name = "width", .setter = setter_width, .getter = getter_width},
  {.name = "height", .setter = setter_height, .getter = getter_height},
  {.name = "enabled", .setter = setter_enabled, .getter = getter_enabled},
  {.name = "selected", .setter = setter_selected, .getter = getter_selected},
};
#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

static int L_RadioBtniterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < ARR_COUNT(radiobtn_intvars)){
    lua_pushstring(L, radiobtn_intvars[val].name); //index
    radiobtn_intvars[val].getter(L, -3); //data
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_RadioBtnpairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_RadioBtniterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static int L_RadioBtngetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1);
  lua_pop(L, 1);
  for(int i=0; i<ARR_COUNT(radiobtn_intvars); i++){
    if(strcmp(idx, radiobtn_intvars[i].name)==0){
      return radiobtn_intvars[i].getter(L, -1);
    }
  }
  lua_pushnil(L);
  return 1;
}

#define ALLOW_APPEND_TABLE 1
static int L_RadioBtnsetter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2);
  for(int i=0; i<ARR_COUNT(radiobtn_intvars); i++){
    if(strcmp(idx, radiobtn_intvars[i].name)==0){
      return radiobtn_intvars[i].setter(L, -3);
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

static int L_RBtn_GC(lua_State *L){
  printf("RadioBtn GC\n");
  int top = lua_gettop(L);
  RadioBtn *btn = (RadioBtn*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(btn->obj))gtk_widget_destroy(btn->obj);
  free_index(L, btn->pool_idx);
  free(btn);
  lua_settop(L, top);
  return 0;
}

static int L_RBtn_Reattach(lua_State *L){
  RadioBtn *btn = (RadioBtn*)read_handle(L, 1, NULL);
  GSList *group = NULL;
  if(lua_istable(L, -1)){
    RadioBtn *sample = (RadioBtn*)read_handle(L, -1, NULL);
    printf("Reattach %p\n", sample);
    group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(sample->obj));
  }
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btn->obj), group);
  if(group == NULL)return 0;
  return 1;
}

static gint FindSelected(gconstpointer a, gconstpointer b){
  RadioBtn *obj = (RadioBtn*)a;
  if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(obj)) )return 0;
  return 1;
}
static int L_RBtn_Get_Selected(lua_State *L){
  //костыль: проходим по списку индексов в поиске нажатой кнопки чтобы вернуть указатель на нее
  int prev = lua_gettop(L);
  RadioBtn *btn = (RadioBtn*)read_handle(L, 1, NULL);
  GSList *group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btn->obj));
  GSList *res = g_slist_find_custom(group, NULL, FindSelected);
  GtkWidget *obj = res->data;
  if(obj == NULL)return 0;
  
  lua_CFunction gc;
  
  lua_rawgeti(L, LUA_REGISTRYINDEX, gui.poolidx); //gui
    lua_getmetatable(L, -1); //mainwindow.metatable
      lua_getfield(L, -1, "pool"); //pool
  for(int i=0; i<gui.poolnum; i++){
        lua_rawgeti(L, -1, i); //elem
          if(!lua_istable(L, -1)){
            lua_pop(L, 1);
            continue;
          }
          lua_getmetatable(L, -1);
            lua_getfield(L, -1, "__gc");
              if(lua_iscfunction(L, -1)){
                gc = lua_tocfunction(L, -1);
              }else{
                gc = NULL;
              }
        lua_pop(L, 2);
    //ЫЫЫ =8-] я проверяю совпадает ли тип элемента по адресу сборщика мусора
    if(gc != L_RBtn_GC){
      lua_pop(L, 1);
      continue;
    }
    lua_getmetatable(L, -1);
      lua_getfield(L, -1, "handle");
        RadioBtn *btn = (RadioBtn*)lua_topointer(L, -1);
      lua_pop(L, 2);
    if(btn->obj == obj){
      return 1;
    }
    lua_pop(L, 1);
  }
  lua_settop(L, prev);
  return 0;
}

static int L_RNewBtn(lua_State *L){
  const char *caption="NONAME";
  //получаем объект родительского окна
  if(lua_gettop(L) < 1){
    printf("Call function as METHOD!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  GtkWidget *cont = read_container(L, 1, NULL);
  RadioBtn *btn = (RadioBtn*)malloc(sizeof(RadioBtn));
  btn->x=0; btn->y=0;
  if(lua_gettop(L) >= 4){
    if(lua_isnumber(L, 2))btn->x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))btn->y = lua_tonumber(L, 3);
    if(lua_isstring(L, 4))caption = lua_tostring(L, 4);
  }
  GSList *group = NULL;
  //последний аргумент - к какому виджету привязаться
  if(lua_gettop(L) >= 5){
    if(lua_istable(L, 5)){
      RadioBtn *sample = (RadioBtn*)read_handle(L, 5, NULL);
      group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(sample->obj));
    }
  }
  
  btn->pool_idx = mk_blank_table(L, btn, L_RBtn_GC);
  lua_pushcfunction(L, L_RBtn_Reattach);
  lua_setfield(L, -2, "Reattach");
  lua_pushcfunction(L, L_RBtn_Get_Selected);
  lua_setfield(L, -2, "GetSelected");
  lua_getmetatable(L, -1);
    lua_pushcfunction(L, L_RadioBtnsetter);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, L_RadioBtngetter);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, L_RadioBtnpairs);
    lua_setfield(L, -2, "__pairs");
  lua_setmetatable(L, -2);

  btn->obj = gtk_radio_button_new_with_label(group, caption);
  
  gtk_fixed_put(GTK_FIXED(cont), btn->obj, btn->x, btn->y);
  gtk_widget_show(btn->obj);
  return 1;
}

void radiobutton_reg(lua_State *L){
  lua_pushcfunction(L, L_RNewBtn);
  lua_setfield(L, -2, "NewRadioButton");
#ifdef DEBUG
  printf("Radio button registred\n");
#endif
}
