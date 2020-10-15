EMPTY = $(subst O_O,,O_O)
target = all
arch := $(EMPTY)

ifneq ( $(arch), $(EMPTY) )
  rarch := arch=arch_$(arch)
endif

selected:
	@echo "Specify module and set variable 'target' to local target"
	@echo "  make [arch=arch_...] [target=...] <module>"
	@echo
	@echo "  <modules> are:"
	@grep ":$$" makefile
all: runtime gtk sr570 sr5105 sr830 lps305 e24
runtime:
	bash -c "cd core; make $(rarch) $(target) -j"
gtk:
	bash -c "cd modules/mygui_gtk; make $(rarch) $(target) -j"
sr570:
	bash -c "cd modules/sr570; make $(rarch) $(target) -j"
sr5105:
	bash -c "cd modules/sr5105; make $(rarch) $(target) -j"
sr830:
	bash -c "cd modules/sr830; make $(rarch) $(target) -j"	
lps305:
	bash -c "cd modules/lps305; make $(rarch) $(target) -j"
e24:
	bash -c "cd modules/e24; make $(rarch) $(target) -j"
