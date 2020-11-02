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
#include <stdlib.h>
int luaopen_timer(lua_State*);
  
#ifdef __cplusplus
}
#endif

static uint64_t timer_interval_ms = 100;
static unsigned int timer_cbc_num = 0;
static lua_State *LG = NULL;
static lua_State *L_glob = NULL;

static uint64_t get_time_ms(){
  struct timeval tv;
  uint64_t time_ms;
  gettimeofday(&tv,NULL);
  time_ms = tv.tv_usec/1000 + tv.tv_sec*1000;
  return time_ms;
}

static void Timer_init();
static void Timer_destroy();
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
#define MAX_TIMEOUT_ms 100
#define TIMER_DEFMASK 0
#define TIMER_HOOK_VAL 1
#define WorkaroundFunc(L) do{if(timer_flag){TimerHook(L, NULL); timer_flag=0;} }while(0)
#include <signal.h>
volatile char timer_flag = 0;
static struct itimerval timer = {{0,0},{0,0}}; //нет, это не две совы!
void (*prev_signal)(int) = NULL;

static void AlarmSignal(int sig){
  timer_flag = 1;
  lua_sethook(L_glob, TimerHook, LUA_MASKCOUNT, 1);
}

static void Timer_SetInterval(unsigned int time_ms){
  timer.it_interval.tv_sec = (time_ms / 1000);
  timer.it_interval.tv_usec = (time_ms % 1000)*1000;
  timer.it_value.tv_sec = timer.it_interval.tv_sec;
  timer.it_value.tv_usec = timer.it_interval.tv_usec;
  setitimer(ITIMER_REAL, &timer, NULL);
  timer_interval_ms = time_ms;
}

static void Timer_init(){
  Timer_SetInterval(timer_interval_ms);
  prev_signal = signal(SIGALRM, AlarmSignal);
}

static void Timer_destroy(){
  Timer_SetInterval(0);
  //signal(SIGALRM, prev_signal);
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
static void Timer_init(){
  lua_sethook(L_glob, TimerHook, TIMER_DEFMASK, TIMER_HOOK_VAL);
}
static void Timer_destroy(){
  lua_sethook(L_glob, TimerHook, 0, 0);
}
#endif

/*******************************************************************************************
 *******************************************************************************************
 *             Common functions
 *******************************************************************************************
 *******************************************************************************************
 */

typedef struct{
  uint64_t time_ms;
  uint64_t interval_ms;
}timeint_t;
#define TIMER_STEP 10
struct{
  timeint_t *time;
  uint64_t mintime_ms;
  unsigned int max;
}timer_cbc_times = {NULL, ~0, 0};

void TimerHook(lua_State *L, lua_Debug *ar){
  static uint64_t next_time = 0;
  uint64_t cur_time = get_time_ms();
  if(cur_time < next_time)return;
  next_time = cur_time + timer_interval_ms;
  
  lua_sethook(LG, TimerHook, TIMER_DEFMASK, TIMER_HOOK_VAL);
  
  if(timer_cbc_times.mintime_ms > cur_time)return;
  
  timer_cbc_times.mintime_ms = ~0ULL;
  
  int tt_top = lua_gettop(LG);
  for(int i=1; i<=timer_cbc_num; i++){
    if(timer_cbc_times.time[i-1].time_ms > cur_time)continue;
    timer_cbc_times.time[i-1].time_ms += timer_cbc_times.time[i-1].interval_ms;
    if(timer_cbc_times.mintime_ms > timer_cbc_times.time[i-1].time_ms){
      timer_cbc_times.mintime_ms = timer_cbc_times.time[i-1].time_ms;
    }
    
    lua_settop(LG, tt_top);
    
    lua_rawgeti(LG, 1, i);
    if(!lua_istable(LG, -1)){
      printf("[%i] is not table\n", i);
      continue;
    }
    lua_len(LG, 2);
    int nargs = lua_tonumber(LG, -1);
    lua_getfield(LG, 2, "func");
    for(int i=1; i<=nargs; i++){
      lua_rawgeti(LG, 2, i);
    }
    lua_pcall(LG, nargs, 0, 0);
  }
  lua_settop(LG, tt_top);
}

static int L_Help(lua_State *L){
  //TODO
  return 0;
}

//создаем новую coroutine, записываем ее в self.metatable.callbacks[i].thread
//в ту же таблицу в поле func кладем функцию
//потом по номерным индексам кладем аргументы
//а вот время будет храниться в глобальнм массиве
static int L_SetTimedCallback(lua_State *L){
  int tbl_start = 2;
  if(!lua_istable(L, 1)){
    tbl_start = 1;
  }
  if(!lua_isnumber(L, tbl_start)){
    fprintf(stderr, "Timer:SetTimedCallback( time, func, [args...] )\n");
    return 0;
  }
  double time = lua_tonumber(L, tbl_start);
  timer_cbc_num++;
  
  //realloc time buffer if needed
  if(timer_cbc_num >= timer_cbc_times.max){
    size_t newmax = timer_cbc_times.max + TIMER_STEP;
    void *temp = realloc(timer_cbc_times.time, sizeof(timeint_t)*newmax);
    if(temp == NULL){
      fprintf(stderr, "Timer: Can not allocate %lu bytes\n", (unsigned long)sizeof(timeint_t)*newmax);
      return 0;
    }
    timer_cbc_times.time = temp;
    timer_cbc_times.max = newmax;
  }
  timer_cbc_times.time[timer_cbc_num-1].interval_ms = time*1000;
  timer_cbc_times.time[timer_cbc_num-1].time_ms = timer_cbc_times.time[timer_cbc_num-1].interval_ms + get_time_ms();
  if(timer_cbc_times.time[timer_cbc_num-1].time_ms < timer_cbc_times.mintime_ms){
    timer_cbc_times.mintime_ms = timer_cbc_times.time[timer_cbc_num-1].time_ms;
  }
  
  int top = lua_gettop(L);
  
  lua_createtable(LG, 0, 0);
    lua_newthread(LG);
    lua_setfield(LG, -2, "thread");
    lua_pushvalue(L, tbl_start+1); //func
    lua_xmove(L, LG, 1);
    lua_setfield(LG, -2, "func");
    tbl_start++;
    for(int i=1; i<=top-tbl_start+1; i++){
      lua_pushvalue(L, i+tbl_start);
      lua_xmove(L, LG, 1);
      lua_rawseti(LG, -2, i);
    }
  lua_rawseti(LG, 1, timer_cbc_num);

  lua_settop(L, 0);
  
  lua_pushnumber(L, timer_cbc_num);
  return 1;
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

static int L_GetSysTime(lua_State *L){
  lua_pushnumber(L, get_time_ms() * 0.001);
  return 1;
}

static int L_Test(lua_State *L){
  printf("Test\n");
  return 0;
}

static int L_GC(lua_State *L){
  if(timer_cbc_times.time != NULL){
    free(timer_cbc_times.time);
    timer_cbc_times.time = NULL;
    timer_cbc_times.max = 0;
  }
  Timer_destroy();
  printf("Destroy\n");
  return 0;
}

int luaopen_timer(lua_State *L){
  if(LG != NULL){ //если объект уже создан, созвращаем ссылку на него же
    lua_getfield(LG, 1, "handle");
    lua_xmove(LG, L, 1);
    return 1;
  }
  L_glob = L;
  Timer_init();
  
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
        lua_pushvalue(L, -3); //main table
        lua_setfield(L, -2, "handle");
      lua_setfield(L, -2, "callbacks");
      LG = lua_newthread(L);
      lua_setfield(L, -2, "timer_thread");
        lua_getfield(L, -1, "callbacks");
        lua_xmove(L, LG, 1);
    lua_setmetatable(L, -2);
    lua_pushcfunction(L, L_Help);
    lua_setfield(L, -2, "Help");
    lua_pushcfunction(L, L_SetTimedCallback);
    lua_setfield(L, -2, "SetTimedCallback"); //create callback each ??? secs
    lua_pushcfunction(L, L_SetTimerQuantum);
    lua_setfield(L, -2, "SetTimerQuantum"); //set update interval of timer
    lua_pushcfunction(L, L_Sleep);
    lua_setfield(L, -2, "Sleep"); //sleep ??? secs
    lua_pushcfunction(L, L_GetSysTime); //return time from OS start
    lua_setfield(L, -2, "GetSysTime");
    
    lua_pushcfunction(L, L_Test);
    lua_setfield(L, -2, "test");
  return 1;
}
