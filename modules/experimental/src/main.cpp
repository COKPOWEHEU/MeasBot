#include "measbot_modules.h"

static const char helpstring[] = "Test string";

int import(lua_State *l){
  return 100500;
}
const char* help(){
  return helpstring;
}

