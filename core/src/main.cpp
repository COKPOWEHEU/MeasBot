#include <stdio.h>
#include "portability.h"

#ifdef cplusplus
extern "C"{
#endif
  
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
  
typedef int (*mbimport)(lua_State *l);
typedef const char* (*mbhelp)(void);
  
#ifdef cplusplus
}
#endif

int test(lua_State *luaVM){
  printf("Test\n");
  return 1;
}
static void hook(lua_State *l, lua_Debug *ar) {
 // if(!run)
    luaL_error(l, "Program was closed!");
}

int main(int argc, char **argv){
  void *lib;
  mbimport imp = NULL;
  mbhelp hlp = NULL;
  int res;
  lua_State *g_LuaVM = NULL;
  
  lib = DynLoad("../modules/experimental" DynSuffix);
  if(lib == NULL){printf("Can not load library\n"); return 0;}
  imp = (mbimport)DynFunc(lib, "import");
  hlp = (mbhelp)DynFunc(lib, "help");
  if(imp == NULL){
    printf("Can not load func 'import'\n");
  }else{
    printf("%i\n", imp(NULL));
  }
  if(hlp == NULL){
    printf("Can not load func 'help'\n");
  }else{
    printf("%s\n", hlp());
  }
  DynClose(lib);
  return 0;
  
  g_LuaVM = luaL_newstate();

  lua_register(g_LuaVM, "test", test);
  
  res = luaL_loadfile(g_LuaVM, "data/script.lua");
  if(res != 0){
    printf("Error load script\n"); lua_close(g_LuaVM); return -1;
  }//*/
  //lua_sethook(g_LuaVM, hook, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT|LUA_MASKLINE, 1000);
  res = lua_pcall(g_LuaVM, 0, LUA_MULTRET, 0);
  /*
#define L g_LuaVM
    luaL_dofile(L, "data/script.lua");
    luaL_openlibs(L);
    lua_getglobal(L, "func");
    lua_call(L,0,0);
    lua_pcall(L, 0, 0, 0);
*/
  
  switch(res){
    case LUA_OK: printf("Success\n"); break;
    case LUA_ERRRUN: printf("Runtime arror\n"); break;
    case LUA_ERRMEM: printf("Memory error\n"); break;
    case LUA_ERRERR: printf("error while running the message handler\n"); break;
    case LUA_ERRGCMM: printf("garbage collector error \n"); break;
    default: printf("UNKNOWN ERROR\n");
  }
  
  lua_close(g_LuaVM);
}
