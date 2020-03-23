#include "ttym.h"
#ifdef __cplusplus
extern "C"{
#endif

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
int luaopen_tty(lua_State*);
  
#ifdef __cplusplus
}
#endif

//TODO добавить количество стоп-битов, проверку четности
//TODO добавить список доступных портов

static int L_Help(lua_State *L){
  static const char helpstring[] = "Tty help string";
  lua_pushstring(L, helpstring);
  return 1;
}

static int L_print(lua_State *L){
  const char *str = "";
  size_t len = 0;
  if(lua_gettop(L) >= 2){
    if(lua_isstring(L, 2))str = lua_tolstring(L, 2, &len);
    lua_pop(L, 1);
  }
  
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      printf("Not metatable!\n");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      printf("Err2\n");
      return 0;
    }
    ttym_t tty = (ttym_t)lua_topointer(L, -1);
  lua_pop(L, 2);
  printf("print [%s]:%i\n", str, (int)len);
  int res = ttym_write(tty, (void*)str, len);
  
  lua_pushnumber(L, res);
  return 1;
}

static int L_gets(lua_State *L){
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      printf("Not metatable!\n");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      printf("Err2\n");
      return 0;
    }
    ttym_t tty = (ttym_t)lua_topointer(L, -1);
  lua_pop(L, 2);
  char str[100];
  char *ch = str;
  str[0] = 0;
  int dat;
  while(1){
    dat = ttym_readchar(tty);
    if(dat <= 0)break;
    if(dat == '\r' && ch == str)continue;
    if(dat == '\n' && ch[0] == '\r')ch--;
    ch[0] = dat;
    ch++;
  }
  ch[0] = 0;
  
  lua_pushstring(L, str);
  return 1;
}

static int L_timeout(lua_State *L){
  ssize_t timeout = 0;
  if(lua_gettop(L) >= 2){
    if(lua_isnumber(L, 2))timeout = lua_tonumber(L, 2);
    lua_pop(L, 1);
  }
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      printf("Not metatable!\n");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      printf("Err2\n");
      return 0;
    }
    ttym_t tty = (ttym_t)lua_topointer(L, -1);
  lua_pop(L, 2);
  ttym_timeout(tty, timeout);
  return 0;
}

static int L_Tty_GC(lua_State *L){
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      printf("Not metatable!\n");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      printf("Err2\n");
      return 0;
    }
    ttym_t tty = (ttym_t)lua_topointer(L, -1);
    ttym_close(tty);
  lua_pop(L, 2);
  return 0;
}

static int L_OpenTty(lua_State *L){
  const char *path = "";
  unsigned int baudrate = 0;
  if(lua_gettop(L) >= 3){
    if(lua_isstring(L, 2))path = lua_tostring(L, 2);
    if(lua_isnumber(L, 3))baudrate = lua_tonumber(L, 3);
  }
  lua_settop(L, 0);
  ttym_t tty = ttym_open((char*)path, baudrate);
  if(tty == NULL){
    //printf("Can not open\n");
    return 0;
  }
  ttym_timeout(tty, 100);
  
  lua_createtable(L, 0, 0);
    lua_createtable(L, 0, 0);
      lua_pushlightuserdata(L, tty);
      lua_setfield(L, -2, "handle");
      lua_pushcfunction(L, L_Tty_GC);
      lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
    lua_pushcfunction(L, L_print);
    lua_setfield(L, -2, "puts");
    lua_pushcfunction(L, L_gets);
    lua_setfield(L, -2, "gets");
    lua_pushcfunction(L, L_timeout);
    lua_setfield(L, -2, "timeout");
  return 1;
}

int luaopen_tty(lua_State *L){
  //при импорте вызываем внешнюю функцию OnImport (если она есть)
  lua_getglobal(L, "OnImport");
  if(lua_isfunction(L, -1)){
    lua_pushnumber(L, 42);
    lua_pcall(L, 1, 0, 0);
  }
  lua_createtable(L, 0, 0);
    lua_pushcfunction(L, L_OpenTty);
    lua_setfield(L, -2, "OpenTTY");
    lua_pushcfunction(L, L_Help);
    lua_setfield(L, -2, "Help");
  return 1;
}
