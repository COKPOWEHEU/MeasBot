#ifndef __WINDOW_H_GTK__
#define __WINDOW_H_GTK__

typedef struct{
  GtkWidget *obj;
  lua_State *L;
  char runflag;
  int pool_idx;
  GtkWidget *fixed;
}Wnd;

#endif
