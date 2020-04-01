resdir = res
srcdir = src
builddir = build

progname := $(resdir)/prog
files = main window button radiobutton checkbox edit spinedit plot label tabcontrol progressbar test

regfuncs = $(addsuffix _reg(L);,$(files))
regproto = $(addprefix void ,$(addsuffix _reg(lua_State*);,$(files)))

CFLAGS  = -Os -Wall -MD -MP -MT $(builddir)/$(*F).$(arch).o -MF $(builddir)/dep/$(@F).mk
#-D_REENTERAN
CFLAGS += -I./ -fPIC -DREGFUNCS="$(regfuncs)" -DREGPROTOS="$(regproto)"
CFLAGS += -DDEBUG
CXXFLAGS = $(CFLAGS) -std=c++11

LDFLAGS = -lm -fPIC -shared
#-lSDL2_ttf -lSDL2_image -lSDL2_net -lSDL2_mixer

# Выбор архитектуры
arch := arch_lin64
#arch := arch_win32

# Выбор режима
mode := mode_debug
#mode := mode_release



##############################################################################################
##############################################################################################
#  Реализации
##############################################################################################
##############################################################################################

CC = $(COMP_PREFIX)gcc$(COMP_SUFFIX)
CXX= $(COMP_PREFIX)g++$(COMP_SUFFIX)
AR = $(COMP_PREFIX)gcc-ar$(COMP_SUFFIX)
RLIB=$(COMP_PREFIX)gcc-ranlib$(COMP_SUFFIX)

#разделитель слов в одной записи
_flagdiv_ = _.~№!string_to_split_parameters_of_different_architectures!№~._
_divider_ = _.~№!I_hope_there_will_not_be_this_string_in_flags_or_file_names!№~._
#немного черной магии чтобы получить символ пробела
SPACE := $(subst O_O, ,O_O)

compiler =	$(_flagdiv_)arch_win32 i686-w64-mingw32- -win32\
		$(_flagdiv_)arch_win64 x86_64-w64-mingw32- -win32\
		
		
spec_cflags  =	$(_flagdiv_)arch_lin32 -m32 -isystem /usr/include
spec_cflags +=	$(_flagdiv_)arch_lin64 `pkg-config --cflags gtk+-3.0`
spec_cflags +=	$(_flagdiv_)arch_win32 -I../../mingw/ -I../../mingw/include/cairo `PKG_CONFIG_SYSROOT_DIR=../../mingw PKG_CONFIG_PATH=../../mingw/lib/pkgconfig i686-w64-mingw32-pkg-config --cflags gtk+-3.0` -I../../mingw/mingw32
		
spec_ldflags  =	$(_flagdiv_)arch_lin32 -m32 
spec_ldflags +=	$(_flagdiv_)arch_lin64 -llua5.2 `pkg-config --libs gtk+-3.0`
spec_ldflags +=	$(_flagdiv_)arch_win32 -static-libgcc -static-libstdc++ -lgdi32 -mconsole -mwindows -lmingw32 `PKG_CONFIG_PATH=../../mingw/lib/pkgconfig i686-w64-mingw32-pkg-config --define-variable=prefix=../../mingw --libs gtk+-3.0` -L ../../mingw/mingw32 -llua52

res_flags =	$(_flagdiv_)mode_debug -gdwarf-2

prognames =	arch_lin32$(_divider_)32.so\
		arch_lin64$(_divider_)64.so\
		arch_win32$(_divider_)32.dll\
		arch_win64$(_divider_)64.dll\

#Еще черная магия с подстановками. Рассмотрим на примере CFLAGS с добавлением псевдо-переменных
# VAR_A = $(subst $(SPACE),$(_divider_),$(spec_cflags)) - заменяем в исходной строке пробелы на $(_divider_). Теперь пробелов в строке нет
# VAR_B = $(subst $(_flagdiv_), ,$(VAR_A)) - в полученной строке заменяем $(_flagdiv_) на пробелы. Теперь переменная разделена пробелами на длинные строки с arch_ в начале
# VAR_C = $(filter $(arch)$(_divider_)%,$(VAR_B)) - выделяем из переменной строки, начинающиеся на $(arch), то есть на заданную архитектуру. Например, на arch_lin32. Теперь строка содержит только нужные для данной архитектуры параметры,но они все еще разделены $(_divider_)'ами
# VAR_D = $(subst $(_divider_), ,$(VAR_C)) - меняем $(_divider_)'ы на пробелы. Теперь параметры разделены как надо, пробелами. Но в начале все еще есть $(arch), который компилятору не нужен
# CFLAGS = $(filter-out $(arch),$(VAR_D)) - отбрасываем $(arch)
#Теперь переменная содержит только нужные флаги
COMP_PREFIX = $(word 2,$(subst $(_divider_), ,$(filter $(arch)$(_divider_)%,$(subst $(_flagdiv_), ,$(subst $(SPACE),$(_divider_),$(compiler))))))
COMP_SUFFIX = $(word 3,$(subst $(_divider_), ,$(filter $(arch)$(_divider_)%,$(subst $(_flagdiv_), ,$(subst $(SPACE),$(_divider_),$(compiler))))))
CFLAGS += $(filter-out $(arch),$(subst $(_divider_), ,$(filter $(arch)$(_divider_)%,$(subst $(_flagdiv_), ,$(subst $(SPACE),$(_divider_),$(spec_cflags))))))
CFLAGS += $(filter-out $(mode),$(subst $(_divider_), ,$(filter $(mode)$(_divider_)%,$(subst $(_flagdiv_), ,$(subst $(SPACE),$(_divider_),$(res_flags))))))
LDFLAGS := $(filter-out $(arch),$(subst $(_divider_), ,$(filter $(arch)$(_divider_)%,$(subst $(_flagdiv_), ,$(subst $(SPACE),$(_divider_),$(spec_ldflags)))))) $(LDFLAGS)
#тут это извращение уже не нужно, параметры простые
resname := $(progname)$(word 2,$(subst $(_divider_), ,$(filter $(arch)$(_divider_)%,$(prognames))))

objects = $(addprefix $(builddir)/,$(addsuffix .$(arch).o,$(files)))

.PHONY: clean cleanall

all:	$(objects)
	echo $(LDFLAGS)
	mkdir -p $(resdir)
	$(CXX) $(objects) $(LDFLAGS) -o $(resname)
remake: clean all
test:
	echo
	echo $(CFLAGS)
	echo
clean:
	rm -f $(objects)
	rm -f $(resname)
	rm -rf $(builddir)/dep
release:	allarch
	rm -rf $(builddir)
cleanall:
	rm -rf $(builddir)
	rm -f $(progname)*
allarch:
	make -r -j 10 arch=arch_lin64
	make -r -j 10 arch=arch_win32

$(builddir)/%.$(arch).o: $(srcdir)/%.c
	mkdir -p $(builddir)
	$(CC) -c $(CFLAGS) $< -o $@
$(builddir)/%.$(arch).o: $(srcdir)/%.cpp
	mkdir -p $(builddir)
	$(CXX) -c $(CXXFLAGS) $< -o $@

-include $(shell mkdir -p $(builddir)/dep) $(wildcard $(builddir)/dep/*)
