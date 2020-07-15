#
# TransFig makefile
#

all: list.tex 

# translation into latex

list.tex: list.fig Makefile
	fig2dev -L latex list.fig > list.tex
clean::
	rm -f list.tex

