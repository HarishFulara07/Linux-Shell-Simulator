LIB = -lreadline -lcurses -lm

all: 	shell


shell:	shell.c parse.c parse.h
	gcc -std=c99 shell.c parse.c -o shell $(LIB)
	gcc -std=c99 basicInfoReader.c -o basicInfoReader
	gnome-terminal -x sh -c "./shell"

clean:
	rm -f shell *~
	rm -f printBasicInfo *~
	rm -f basicInfoReader *~
	rm -f userName *~
	rm -f hostName *~
	rm -f parse.h.gch *~
	rm -f pwd *~

