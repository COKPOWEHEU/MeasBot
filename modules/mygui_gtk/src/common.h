#ifndef __COMMON_H__
#define __COMMON_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include "window.h"

struct Mainwindow{
  char runflag;
  GtkWidget *window;
  GtkWidget *fixed;
  unsigned int poolnum; //количество компонентов в пуле
  int poolidx; //идентификатор таблицы пула
};
extern struct Mainwindow mainwindow;

void showstack(lua_State *L);

//добавляет таблицу с вершины стека в глобальный пул (стек не меняет) и возвращает индекс
int add_to_pool(lua_State *L, int idx);

//создает шаблон таблицы (с юзердатой и мусорщиком в метатаблице), добавляет в пул и возвращает индекс в нем
int mk_blank_table(lua_State *L, void *handle, lua_CFunction gc);

//чтение handle из таблицы
void* read_handle(lua_State *L, int index, int *err);
//и из глобального пула
int read_self(lua_State *L, int pool_idx);
#define ERR_OK 0
#define ERR_NOT_TABLE -1
#define ERR_NOT_METATABLE -2
#define ERR_NOT_HANDLE -3
#define ERR_NOT_CONTENT -4


//небольшой ХАК мейкфайла: прототипы всех методов регистрации компонентов
REGPROTOS

#ifdef __cplusplus
}
#endif
#endif
