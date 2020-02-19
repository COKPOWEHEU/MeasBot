#ifndef __MEASBOT_MODULES__
#define __MEASBOT_MODULES__

#ifdef __cplusplus
extern "C"{
#endif

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
  
int import(lua_State *l);
const char* help(void);
  
#ifdef __cplusplus
}
#endif

#endif
