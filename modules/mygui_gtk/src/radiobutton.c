#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include <stdlib.h>
#include "common.h"

typedef struct{
  GtkWidget *obj;
  int pool_idx;
}RadioButton;

static int L_RBtn_GC(lua_State *L){
  printf("RadioBtn GC\n");
  int top = lua_gettop(L);
  RadioButton *btn = (RadioButton*)read_handle(L, -1, NULL);
  if(GTK_IS_WIDGET(btn->obj))gtk_widget_destroy(btn->obj);
  free_index(L, btn->pool_idx);
  free(btn);
  lua_settop(L, top);
  return 0;
}

static int L_RBtn_Reattach(lua_State *L){
  RadioButton *btn = (RadioButton*)read_handle(L, 1, NULL);
  GSList *group = NULL;
  if(lua_istable(L, -1)){
    RadioButton *sample = (RadioButton*)read_handle(L, -1, NULL);
    printf("Reattach %p\n", sample);
    group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(sample->obj));
  }
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btn->obj), group);
  if(group == NULL)return 0;
  return 1;
}

static int L_RBtn_IsSelected(lua_State *L){
  RadioButton *btn = (RadioButton*)read_handle(L, 1, NULL);
  gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn->obj));
  lua_pushboolean(L, state);
  return 1;
}

static int L_RBtn_SetSelected(lua_State *L){
  RadioButton *btn = (RadioButton*)read_handle(L, 1, NULL);
  gboolean state = lua_toboolean(L, -1);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btn->obj), state);
  return 0;
}

static gint FindSelected(gconstpointer a, gconstpointer b){
  RadioButton *obj = (RadioButton*)a;
  if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(obj)) )return 0;
  return 1;
}
static int L_RBtn_Get_Selected(lua_State *L){
  //костыль: проходим по списку индексов в поиске нажатой кнопки чтобы вернуть указатель на нее
  int prev = lua_gettop(L);
  RadioButton *btn = (RadioButton*)read_handle(L, 1, NULL);
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
        RadioButton *btn = (RadioButton*)lua_topointer(L, -1);
      lua_pop(L, 2);
    if(btn->obj == obj){
      printf("OK\n");
      return 1;
    }
    lua_pop(L, 1);
  }
  lua_settop(L, prev);
  return 0;
}

static int L_RNewBtn(lua_State *L){
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
  RadioButton *btn = (RadioButton*)malloc(sizeof(RadioButton));
  
  if(lua_gettop(L) >= 4){
    if(lua_isnumber(L, 2))x = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))y = lua_tonumber(L, 3);
    if(lua_isstring(L, 4))caption = lua_tostring(L, 4);
  }
  GSList *group = NULL;
  //последний аргумент - к какому виджету привязаться
  if(lua_gettop(L) >= 5){
    if(lua_istable(L, 5)){
      RadioButton *sample = (RadioButton*)read_handle(L, 5, NULL);
      group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(sample->obj));
    }
  }
  
  btn->pool_idx = mk_blank_table(L, btn, L_RBtn_GC);
  lua_pushcfunction(L, L_RBtn_Reattach);
  lua_setfield(L, -2, "Reattach");
  lua_pushcfunction(L, L_RBtn_IsSelected);
  lua_setfield(L, -2, "IsSelected");
  lua_pushcfunction(L, L_RBtn_SetSelected);
  lua_setfield(L, -2, "SetSelected");
  lua_pushcfunction(L, L_RBtn_Get_Selected);
  lua_setfield(L, -2, "GetSelected");

  btn->obj = gtk_radio_button_new_with_label(group, caption);
  
  gtk_fixed_put(GTK_FIXED(cont), btn->obj, x, y);
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
