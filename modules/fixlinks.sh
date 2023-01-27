#!/bin/bash

echo "Remove so, dll, lib"
rm -rf so
rm -rf dll
rm -rf lib

echo "make new so, dll, lib"
mkdir so
mkdir dll
mkdir lib
ln -s "../so" "lib/lib\dyn"
mkdir lib/lib
ln -s "../../dll" "lib/lib/dyn"

echo "make links"
modnames=`find ./ -name prog*.so`

for i in $modnames ; do
  #вырезать имя (последовательность символов без слешей) начиная от "./"
  name=`echo $i | sed 's/\.\/\([^\/]*\)\/.*/\1/g'`
  path="../"$name"/res/prog64.so"
  echo "ln -s "$path" so/"$name".so"
  ln -s $path "./so/"$name".so"
  path="../"$name"/res/prog32.dll"
  echo "ln -s "$path" dll/"$name".dll"
  ln -s $path "./dll/"$name".dll"
done