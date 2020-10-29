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

static lua_State *LG = NULL;

static uint64_t get_time_ms(){
  struct timeval tv;
  uint64_t time_ms;
  gettimeofday(&tv,NULL);
  time_ms = tv.tv_usec/1000 + tv.tv_sec*1000;
  return time_ms;
}

static void Timer_init(lua_State *LG);
static void Timer_destroy(lua_State *LG);
static void TimerHook(lua_State *L, lua_Debug *ar);
static void Timer_SetInterval(unsigned int time_ms);

static int L_Test(lua_State *L);

/*******************************************************************************************
 *******************************************************************************************
 *             LINUX
 *******************************************************************************************
 *******************************************************************************************
 */

#ifdef linux
#define MAX_TIMEOUT_ms 1000
#define TIMER_DEFMASK 0
#define TIMER_HOOK_VAL 1
#define WorkaroundFunc(L)
#include <signal.h>
static struct itimerval timer = {{0,0},{0,0}}; //нет, это не две совы!
void (*prev_signal)(int) = NULL;

static void AlarmSignal(int sig){
  lua_sethook(LG, TimerHook, LUA_MASKCOUNT, 1);
}

static void Timer_SetInterval(unsigned int time_ms){
  timer.it_interval.tv_sec = (time_ms / 1000);
  timer.it_interval.tv_usec = (time_ms % 1000)*1000;
  timer.it_value.tv_sec = timer.it_interval.tv_sec;
  timer.it_value.tv_usec = timer.it_interval.tv_usec;
  setitimer(ITIMER_REAL, &timer, NULL);
}

static void Timer_init(lua_State *LG){
  Timer_SetInterval(100);
  prev_signal = signal(SIGALRM, AlarmSignal);
}

static void Timer_destroy(lua_State *LG){
  Timer_SetInterval(0);
  signal(SIGALRM, prev_signal);
}

/*******************************************************************************************
 *******************************************************************************************
 *             WINDOWS
 *******************************************************************************************
 *******************************************************************************************
 */
#elif defined(WIN32)
#define MAX_TIMEOUT_ms 100
#define TIMER_DEFMASK LUA_MASKCOUNT
#define TIMER_HOOK_VAL 100
#define WorkaroundFunc(L) TimerHook(L, NULL)
static void Timer_SetInterval(unsigned int time_ms){
  //disabled
}
static void Timer_init(lua_State *LG){
  lua_sethook(LG, TimerHook, TIMER_DEFMASK, TIMER_HOOK_VAL);
}
static void Timer_destroy(lua_State *LG){
  lua_sethook(LG, TimerHook, 0, 0);
}
#endif

/*******************************************************************************************
 *******************************************************************************************
 *             Common functions
 *******************************************************************************************
 *******************************************************************************************
 */
typedef struct timer_callback{
  lua_State *func;
  uint64_t period_ms;
  uint64_t next_time_ms;
}timer_callback_t;

static struct{
  timer_callback_t *arr;
  uint64_t next_time_ms;
  unsigned int max;
  unsigned int count;
}timer_pool = {NULL, (1ULL<<63), 0, 0};

//TODO: придумать как вызывать функции по таймеру. Lua_newthread?

static lua_State *Lfunc = NULL;

void TimerHook(lua_State *L, lua_Debug *ar){
  //static uint64_t prev_time = 0;
  uint64_t cur_time = get_time_ms();
  lua_sethook(LG, TimerHook, TIMER_DEFMASK, TIMER_HOOK_VAL);
  printf("Hook\n");
  if( cur_time < timer_pool.next_time_ms )return;
  timer_pool.next_time_ms = (1ULL << 63);
  for(int i=0; i<timer_pool.count; i++){
    if( cur_time < timer_pool.arr[i].next_time_ms )continue;
    timer_pool.arr[i].next_time_ms += timer_pool.arr[i].period_ms;
    if( timer_pool.arr[i].next_time_ms < timer_pool.next_time_ms ){
      timer_pool.next_time_ms = timer_pool.arr[i].next_time_ms;
    }
    int nargs = lua_gettop(Lfunc);
    for(int i=1; i<nargs; i++){
      lua_pushvalue(Lfunc, i);
    }
    lua_pcall(Lfunc, nargs-2, 0, 0);
    lua_settop(Lfunc, nargs);
  }
  
  /*if(cur_time - prev_time > 1000){
    //printf("1s\n");
    prev_time = cur_time;
    if(Lfunc != NULL){
      
      int nargs = lua_gettop(Lfunc);
      //printf("Nargs = %i\n", nargs);
      for(int i=1; i<nargs; i++){
        lua_pushvalue(Lfunc, i);
      }

      int res = lua_pcall(Lfunc, nargs-2, 0, 0);
      //printf("res=%i\n", res);
      //printf("LFunc top = %i\n", lua_gettop(Lfunc));
      lua_settop(Lfunc, nargs);
      
    }
  }*/
}

static int L_Help(lua_State *L){
  //TODO
  return 0;
}

static int L_SetTimedCallback(lua_State *L){
  
  //TODO
  /*if(Lfunc != NULL)return 0;
  int top = lua_gettop(L); // func + args = 1+N
  printf("Set callback top = %i\n", top);
  Lfunc = lua_newthread(L);
  //кладет на вершину стека thread
  lua_xmove(L, Lfunc, top+1);
  lua_xmove(Lfunc, L, 1);
  //копирует весь переданный стек: (func, [args]) в Lfunc
  return 1;*/
}

static int L_SetTimerQuantum(lua_State *L){
  int top = lua_gettop(L);
  double time = 0;
  if(!lua_isnumber(L, -1)){
    fprintf(stderr, "Timer.SetTimerQuantum( time_s )\n");
    return 0;
  }
  time = lua_tonumber(L, -1);
  Timer_SetInterval(time * 1000);
  lua_pop(L, top);
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
    if(av_time - cur_time > MAX_TIMEOUT_ms){
      usleep(MAX_TIMEOUT_ms * 1000);
    }else{
      usleep((av_time - cur_time)*1000);
    }
    WorkaroundFunc(L);
    cur_time = get_time_ms();
  }
  lua_pop(L, top);
  return 0;
}

static int L_GetTime(lua_State *L){
  //TODO
  return 0;
}

static int L_Test(lua_State *L){
  printf("Test\n");
  return 0;
}

static int L_GC(lua_State *L){
  //TODO
  Timer_destroy(LG);
  LG = NULL;
  printf("Timer deleted\n");
  return 0;
}

int luaopen_timer(lua_State *L){
  if(LG != NULL)return 0; //объект таймера должен быть только один
  LG = L;
  Timer_init(L);
  
  //при импорте вызываем внешнюю функцию OnImport (если она есть)
  lua_getglobal(L, "OnImport");
  if(lua_isfunction(L, -1)){
    lua_pushnumber(L, 42);
    lua_pcall(L, 1, 0, 0);
  }
  lua_createtable(L, 0, 0);
    lua_createtable(L, 0, 0);
      lua_pushcfunction(L, L_GC);
      lua_setfield(L, -2, "__gc");
      lua_createtable(L, 0, 0);
      lua_setfield(L, -2, "callbacks");
    lua_setmetatable(L, -2);
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
    
    lua_pushcfunction(L, L_Test);
    lua_setfield(L, -2, "test");
  return 1;
}
