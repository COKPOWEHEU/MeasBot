EMPTY = $(subst O_O,,O_O)
target = all
arch := $(EMPTY)

ifneq ( $(arch), $(EMPTY) )
  rarch := arch=arch_$(arch)
endif

all:
	@echo "Specify module and set variable 'target' to local target"
	@echo "  make [arch=arch_...] [target=...] <module>"
	@echo
	@echo "  <modules> are:"
	@grep ":$$" makefile
core:
	bash -c "cd core; make $(rarch) $(target) -j"
gtk:
	bash -c "cd modules/mygui_gtk; make $(rarch) $(target) -j"

