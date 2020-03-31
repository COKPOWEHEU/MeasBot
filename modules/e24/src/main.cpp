#include <lua5.2/lua.hpp>
#include <fstream>

#ifdef _WIN32
  #define FILE_PATH "C:/dev/e24/"
#else
  #define FILE_PATH "/opt/dev/e24/"
#endif

class channel1 {
  private:
    static double range;
  public:
    static int getVoltage(lua_State *L) {
      double voltage;
      std::fstream fs ((std::string)FILE_PATH + "adc1.adc", std::fstream::in);
      if(!fs.is_open()) {
        lua_pushnil(L);
        return 1;
      }

      fs >> voltage;
      fs.close();

      if((-range/2) > voltage || (range/2) < voltage) {
        lua_pushnil(L);
        return 1;
      }

      lua_pushnumber(L, voltage);
      return 1;
    }

    static int setRange(lua_State *L) {
      double editRange = luaL_checknumber(L, -1);

      if(editRange > 10 || editRange == (double)0) {
        range = 10;
      } else {
        range = editRange;
      }
      return 1;
    }
};

class channel2 {
 private:
    static double range;
  public:
    static int getVoltage(lua_State *L) {
      double voltage;
      std::fstream fs ((std::string)FILE_PATH + "adc2.adc", std::fstream::in);
      if(!fs.is_open()) {
        lua_pushnil(L);
        return 1;
      }

      fs >> voltage;
      fs.close();

      if((-range/2) > voltage || (range/2) < voltage) {
        lua_pushnil(L);
        return 1;
      }

      lua_pushnumber(L, voltage);
      return 1;
    }

    static int setRange(lua_State *L) {
      double editRange = luaL_checknumber(L, -1);

      if(editRange > 10 || editRange == (double)0) {
        range = 10;
      } else {
        range = editRange;
      }
      return 0;
    }
};

class channel3 {
  private:
    static double range;
  public:
    static int getVoltage(lua_State *L) {
      double voltage;
      std::fstream fs ((std::string)FILE_PATH + "adc3.adc", std::fstream::in);
      if(!fs.is_open()) {
        lua_pushnil(L);
        return 1;
      }

      fs >> voltage;
      fs.close();

      if((-range/2) > voltage || (range/2) < voltage) {
        lua_pushnil(L);
        return 1;
      }

      lua_pushnumber(L, voltage);
      return 1;
    }

    static int setRange(lua_State *L) {
      double editRange = luaL_checknumber(L, -1);

      if(editRange > 10 || editRange == (double)0) {
        range = 10;
      } else {
        range = editRange;
      }
      return 0;
    }
};

class channel4 {
  private:
    static double range;
  public:
    static int getVoltage(lua_State *L) {
      double voltage;
      std::fstream fs ((std::string)FILE_PATH + "adc4.adc", std::fstream::in);
      if(!fs.is_open()) {
        lua_pushnil(L);
        return 1;
      }

      fs >> voltage;
      fs.close();

      if((-range/2) > voltage || (range/2) < voltage) {
        lua_pushnil(L);
        return 1;
      }

      lua_pushnumber(L, voltage);
      return 1;
    }

    static int setRange(lua_State *L) {
      double editRange = luaL_checknumber(L, -1);

      if(editRange > 10 || editRange == (double)0) {
        range = 10;
      } else {
        range = editRange;
      }
      return 0;
    }
};

static int L_help(lua_State *L) {
  lua_pushstring(L, "This module is needed  to work with a voltmeter e24");
  return 1;
}

static int L_ReadADC(lua_State *L) {
  int nchannel = luaL_checkinteger(L, -1);
  double voltage;

  char path[22];
  sprintf(path, "%sadc%i.adc", FILE_PATH, nchannel);

  std::fstream fs (path, std::fstream::in);
  if(!fs.is_open()) {
    lua_pushnil(L);
    return 1;
  }

  fs >> voltage;
  fs.close();

  lua_pushnumber(L, voltage);
  return 1;
}

double channel1::range{10};
double channel2::range{10};
double channel3::range{10};
double channel4::range{10};

extern "C" int luaopen_e24(lua_State *L) {
  lua_newtable(L);

    lua_pushstring(L, "help");
    lua_pushcfunction(L, L_help);
    lua_rawset(L, -3);

    lua_pushstring(L, "readADC");
    lua_pushcfunction(L, L_ReadADC);
    lua_rawset(L, -3);

    lua_pushstring(L, "channel1");
    lua_newtable(L);
      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, channel1::getVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setRange");
      lua_pushcfunction(L, channel1::setRange);
      lua_rawset(L, -3);
    lua_rawset(L, -3);

    lua_pushstring(L, "channel2");
    lua_newtable(L);
      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, channel2::getVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setRange");
      lua_pushcfunction(L, channel2::setRange);
      lua_rawset(L, -3);
    lua_rawset(L, -3);

    lua_pushstring(L, "channel3");
    lua_newtable(L);
      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, channel3::getVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setRange");
      lua_pushcfunction(L, channel3::setRange);
      lua_rawset(L, -3);
    lua_rawset(L, -3);

    lua_pushstring(L, "channel4");
    lua_newtable(L);
      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, channel4::getVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setRange");
      lua_pushcfunction(L, channel4::setRange);
      lua_rawset(L, -3);
    lua_rawset(L, -3);

    return 1;
}
