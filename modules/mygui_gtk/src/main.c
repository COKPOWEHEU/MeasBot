#include <stdlib.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>

#include "common.h"

struct Mainwindow mainwindow = {.runflag=1, .window=NULL, .fixed=NULL, .poolnum=0, .poolidx=0};

//bash -c "cd /media/data_ext/prog/gtk/modules ; make"

static const char helpstring[] = "Test string";

void showstack(lua_State *L){
  printf("---\n");
  int max = lua_gettop(L);
  for(int i=0; i<=max; i++){
    printf("[%i]: %s\n", -i, lua_typename(L, lua_type(L, -i)));
  }
}

int add_to_pool(lua_State *L, int idx){
  int res = 0;
  lua_rawgeti(L, LUA_REGISTRYINDEX, mainwindow.poolidx);
    lua_getmetatable(L, -1);
      lua_getfield(L, -1, "pool");
        if(idx > 0){
          lua_pushvalue(L, idx);
        }else{
          lua_pushvalue(L, -3+idx);
        }
        res = mainwindow.poolnum;
        lua_rawseti(L, -2, mainwindow.poolnum);
        mainwindow.poolnum++;
      lua_pop(L, 1); //getfield
    lua_pop(L, 1); //getmetatable
  lua_pop(L, 1); //geti
  return res;
}

int mk_blank_table(lua_State *L, void *handle, lua_CFunction gc){
  int res;
  lua_createtable(L, 0, 0); //создаем объект таблицы
    lua_createtable(L, 0, 0); //метатаблица
      lua_pushlightuserdata(L, handle);
      lua_setfield(L, -2, "handle");
      lua_pushcfunction(L, gc);
      lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
    res = add_to_pool(L, -1);
  return res;
}

void main_reg(lua_State *L){
#ifdef DEBUG
  printf("Main registred\n");
#endif
  //do nothing
}

int L_help(lua_State *L){
  lua_settop(L, 0);
  lua_pushstring(L, helpstring);
  return 1;
}

int L_update(lua_State *L){
  if(!lua_istable(L, -1)){
    printf("Update: Error!\n");
    return 0;
  }
  while(gtk_events_pending())gtk_main_iteration_do(0);
  usleep(0);
  
  lua_settop(L, 0);
  lua_pushboolean(L, mainwindow.runflag);
  return 1;
}

int L_test(lua_State *L){
  printf("lua: test\n");
  return 0;
}

void OnDestroy(GtkWidget *obj, gpointer data){
  mainwindow.runflag = 0;
}

int L_delay_ms(lua_State *L){
  if(!lua_isnumber(L, -1))return 0;
  size_t n = lua_tonumber(L, -1);
  if(n > 0)lua_gc(L, LUA_GCSTEP, 0);
  usleep(n*1000);
  return 0;
}

static int L_gc(lua_State *L){
  printf("GUI free\n");
  if(mainwindow.poolidx != LUA_NOREF){
    luaL_unref(L, LUA_REGISTRYINDEX, mainwindow.poolidx);
  }
  //TODO: допилить освобождение всего
  return 0;
}

int luaopen_mygui_gtk(lua_State *L){
  lua_settop(L, 0);
  
  //при импорте вызываем внешнюю функцию OnImport (если она есть)
  lua_getglobal(L, "OnImport");
  if(lua_isfunction(L, -1)){
    lua_pushnumber(L, 42);
    lua_pcall(L, 1, 0, 0);
  }
  gtk_init(0, NULL);
  
  //создаем основной объект данного модуля
  lua_createtable(L, 0, 0);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
    lua_pushcfunction(L, L_delay_ms);
    lua_setfield(L, -2, "delay_ms");
    mainwindow.runflag = 1;
    mainwindow.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(mainwindow.window), "Test GTK");
    //gtk_window_set_default_size(GTK_WINDOW(mainwindow.window), 800, 600);
    mainwindow.fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(mainwindow.window), mainwindow.fixed);
    g_signal_connect(G_OBJECT(mainwindow.window), "destroy", G_CALLBACK(OnDestroy), NULL);
  
    lua_pushcfunction(L, L_update);
    lua_setfield(L, -2, "update");
  
    lua_createtable(L, 0, 0); //meta
      lua_createtable(L, 0, 0); //pool
        lua_createtable(L, 0, 0); //pool-meta
          lua_pushstring(L, "v");
          lua_setfield(L, -2, "__mode");
        lua_setmetatable(L, -2);
      lua_setfield(L, -2, "pool");
   
      lua_pushcfunction(L, L_gc);
      lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
    
    mainwindow.poolidx = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, mainwindow.poolidx);
  
  REGFUNCS
  
  gtk_widget_show_all(mainwindow.window);
  
  return 1;
}
