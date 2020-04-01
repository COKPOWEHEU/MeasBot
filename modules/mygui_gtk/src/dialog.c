#include <stdlib.h>
#include <gtk/gtk.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include "common.h"
#include "portability.h"

static int L_OpenFileDialog(lua_State *L){
  const char *dlgname = "Open File";
  if(lua_isstring(L, -1))dlgname = lua_tostring(L, -1);
  GtkWidget *dialog = gtk_file_chooser_dialog_new(dlgname,
                                      NULL, //parent window
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

  gint res = gtk_dialog_run (GTK_DIALOG (dialog));
  if(res == GTK_RESPONSE_ACCEPT){
    char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    lua_pushstring(L, filename);
    g_free (filename);
  }else{
    lua_pushnil(L);
  }
  gtk_widget_destroy (dialog);
  return 1;
}
static int L_SaveFileDialog(lua_State *L){
  const char *dlgname = "Save File";
  if(lua_isstring(L, -1))dlgname = lua_tostring(L, -1);
  GtkWidget *dialog = gtk_file_chooser_dialog_new(dlgname,
                                      NULL, //parent window
                                      GTK_FILE_CHOOSER_ACTION_SAVE,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Save"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

  gint res = gtk_dialog_run (GTK_DIALOG (dialog));
  if(res == GTK_RESPONSE_ACCEPT){
    char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    lua_pushstring(L, filename);
    g_free (filename);
  }else{
    lua_pushnil(L);
  }
  gtk_widget_destroy (dialog);
  return 1;
}
static int L_PathDialog(lua_State *L){
  const char *dlgname = "Open Path";
  if(lua_isstring(L, -1))dlgname = lua_tostring(L, -1);
  GtkWidget *dialog = gtk_file_chooser_dialog_new(dlgname,
                                      NULL, //parent window
                                      GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

  gint res = gtk_dialog_run (GTK_DIALOG (dialog));
  if(res == GTK_RESPONSE_ACCEPT){
    char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    lua_pushstring(L, filename);
    g_free (filename);
  }else{
    lua_pushnil(L);
  }
  gtk_widget_destroy (dialog);
  return 1;
}

void dialog_reg(lua_State *L){
#ifdef linux //TODO: в винде пока какие-то проблемы
  lua_pushcfunction(L, L_OpenFileDialog);
  lua_setfield(L, -2, "OpenFileDialog");
  lua_pushcfunction(L, L_SaveFileDialog);
  lua_setfield(L, -2, "SaveFileDialog");
  lua_pushcfunction(L, L_PathDialog);
  lua_setfield(L, -2, "PathDialog");
#ifdef DEBUG
  printf("Dialog registred\n");
#endif
#endif
}
