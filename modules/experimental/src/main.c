#include "../measbot_modules.h"


int L_export_vals(lua_State *L){
  lua_createtable(L, 0, 0);
    lua_getfield(L, 1, "Var1");
    lua_setfield(L, 2, "Var1");
    lua_getfield(L, 1, "Var2");
    lua_setfield(L, 2, "Var2");
  return 1;
}

int L_import_vals(lua_State *L){
  lua_getfield(L, 2, "Var1");
  lua_setfield(L, 1, "Var1");
  lua_getfield(L, 2, "Var2");
  lua_setfield(L, 1, "Var2");
  return 0;
}

int save_table(FILE *pf, lua_State *L, int indent){
  
}

int L_to_file(lua_State *L){
  char *name = lua_tostring(L, -2);
  if(name == NULL){
    printf("tbl2file('filename', table)\n");
    return 0;
  }
  FILE *pf = fopen(name, "wt");
  
  return 0;
}

int L_from_file(lua_State *L){
  
  return 0;
}

const char* help(){
  static const char helpstring[] = "Test string";
  return helpstring;
}

int luaopen_experimental(lua_State *L){
  lua_pushcfunction(L, L_to_file);
  lua_setglobal(L, "tbl2file");
  lua_pushcfunction(L, L_from_file);
  lua_setglobal(L, "file2tbl");
  lua_createtable(L, 0, 0);
    lua_pushnumber(L, 123);
    lua_setfield(L, -2, "Var1");
    lua_pushstring(L, "TPATATA");
    lua_setfield(L, -2, "Var2");
    lua_pushcfunction(L, L_export_vals);
    lua_setfield(L, -2, "export");
    lua_pushcfunction(L, L_import_vals);
    lua_setfield(L, -2, "import");
    
    //lua_pushcfunction(L, L_OpenTty);
    //lua_setfield(L, -2, "OpenTTY");
    //lua_pushcfunction(L, L_Help);
    //lua_setfield(L, -2, "Help");
  return 1;
}