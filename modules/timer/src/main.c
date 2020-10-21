#ifdef __cplusplus
extern "C"{
#endif
  
#include "portability.h"
#include <inttypes.h>

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
  
#include <unistd.h>
#include <sys/time.h>
int luaopen_timer(lua_State*);
  
#ifdef __cplusplus
}
#endif

static lua_State *LS = NULL;

static uint64_t get_time_ms(){
  struct timeval tv;
  uint64_t time_ms;
  gettimeofday(&tv,NULL);
  time_ms = tv.tv_usec/1000 + tv.tv_sec*1000;
  return time_ms;
}

static void Timer_init();

/*******************************************************************************************
 *******************************************************************************************
 *             LINUX
 *******************************************************************************************
 *******************************************************************************************
 */
#ifdef linux
#include <signal.h>
static struct itimerval timer = {{0,0},{0,0}}; //нет, это не две совы!

static void AlarmSignal(int sig){
  printf("OnTimer\n");
}
static void Timer_init(){
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 100000; //100 ms
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 100000;
  setitimer(ITIMER_REAL, &timer, NULL);
  signal(SIGALRM, AlarmSignal);
}


/*******************************************************************************************
 *******************************************************************************************
 *             WINDOWS
 *******************************************************************************************
 *******************************************************************************************
 */
#elif defined(WIN32)
static void Timer_init(){
  
}
#endif

/*******************************************************************************************
 *******************************************************************************************
 *             Common functions
 *******************************************************************************************
 *******************************************************************************************
 */
static int L_Help(lua_State *L){
  //TODO
  return 0;
}

static int L_SetTimedCallback(lua_State *L){
  //TODO
  return 0;
}

static int L_SetTimerQuantum(lua_State *L){
  //TODO
  return 0;
}

static int L_Sleep(lua_State *L){
  int top = lua_gettop(L);
  double time = 0;
  if(!lua_isnumber(L, -1)){
    fprintf(stderr, "Timer.Sleep( time_s )\n");
    return 0;
  }
  time = lua_tonumber(L, -1);
  uint64_t cur_time = get_time_ms();
  uint64_t av_time = cur_time + time*1000;
  while(cur_time < av_time){
    printf("Continue %f\n", (av_time - cur_time)*0.001);
    if(av_time - cur_time > 1000){
      usleep(1000000);
    }else{
      usleep((av_time - cur_time)*1000);
    }
    cur_time = get_time_ms();
  }
  return 0;
}

static int L_GetTime(lua_State *L){
  //TODO
  return 0;
}

int luaopen_timer(lua_State *L){
  Timer_init();
  
  //при импорте вызываем внешнюю функцию OnImport (если она есть)
  lua_getglobal(L, "OnImport");
  if(lua_isfunction(L, -1)){
    lua_pushnumber(L, 42);
    lua_pcall(L, 1, 0, 0);
  }
  lua_createtable(L, 0, 0);
    lua_pushcfunction(L, L_Help);
    lua_setfield(L, -2, "Help");
    lua_pushcfunction(L, L_SetTimedCallback);
    lua_setfield(L, -2, "SetTimedCallback"); //create callback each ??? secs
    lua_pushcfunction(L, L_SetTimerQuantum);
    lua_setfield(L, -2, "SetTimerQuantum"); //set update interval of timer
    lua_pushcfunction(L, L_Sleep);
    lua_setfield(L, -2, "Sleep"); //sleep ??? secs
    lua_pushcfunction(L, L_GetTime);
    lua_setfield(L, -2, "GetTime");
  return 1;
}
