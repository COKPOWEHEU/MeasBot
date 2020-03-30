#include <stdlib.h>
#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include "common.h"


typedef struct{
  int lua_self;
}Test;

static int L_Tst_GC(lua_State *L){
  printf("Test GC\n");
  lua_getmetatable(L, -1);
  lua_getfield(L, -1, "handle");
  if(!lua_islightuserdata(L, -1)){
    printf("GC: Err2\n");
    return 0;
  }
  Test *btn = (Test*)lua_topointer(L, -1);
  free_index(L, btn->lua_self);
  free(btn);
  lua_settop(L, 0);
  return 0;
}

static int L_index(lua_State *L){
  showstack(L);
  printf("---index\n");
  lua_pushstring(L, "Index");
  return 1;
}
static int L_newindex(lua_State *L){
  showstack(L);
  printf("---newindex\n");
  lua_pushstring(L, "Index");
  return 1;
}

static int L_NewTest(lua_State *L){
  int res = 0;
  Test *btn = (Test*)malloc(sizeof(Test));
  lua_settop(L, 0);
  
  lua_settop(L, 0);
    res = mk_blank_table(L, btn, L_Tst_GC);
    /*lua_getmetatable(L, -1);
      lua_pushcfunction(L, L_index);
      lua_setfield(L, -2, "__index");
      lua_pushcfunction(L, L_newindex);
      lua_setfield(L, -2, "__newindex");
    lua_setmetatable(L, -2);*/
    
    lua_pushnumber(L, 123);
    lua_setfield(L, -2, "field"); //какие-то общедоступные поля
    
    
  btn->lua_self = res;
  return 1;  
}

void test_reg(lua_State *L){
  lua_pushcfunction(L, L_NewTest);
  lua_setfield(L, -2, "NewTest");
#ifdef DEBUG
  printf("Test registred\n");
#endif
}
