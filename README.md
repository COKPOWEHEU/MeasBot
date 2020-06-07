# MeasBot
Automatization system for measures by scientific devices

Инструкция по тестированию:
=== Linux ===
Запускаете ./prog64 [script]
Если script не указан, будет использован дефолтный data/script.lua

=== Windows ===
Копируете core\res\prog32.exe в mingw_dll\res32.exe с заменой
Копируете в каталог modules\lib\lib\dyn (возможно, понадобится заменить файл dyn каталогом с тем же именем) следующие файлы с заменой имени:
modules\e24\res\prog32.dll -> e24.dll
modules\mygui_gtk\res\prog32.dll -> mygui_gtk.dll
modules\tty\res\prog32.dll -> tty.dll
После чего запускаете mingw_dll\prog32.exe [script]
Если script не указан, будет использован дефолтный data\script.lua

Данный геморрой связан с тем, что программа находится в начальной стадии разработки и особого внимания эргономике не уделялось.
