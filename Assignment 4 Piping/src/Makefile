.PHONY: clean debug run

all: launcher.exe pager.exe wordsearch.exe wordgen.exe

launcher.exe: launcher.c
	gcc -o launcher.exe launcher.c

pager.exe: pager.c
	gcc -o pager.exe pager.c

wordsearch.exe: wordsearch.c
	gcc -o wordsearch.exe wordsearch.c

wordgen.exe:	wordgen.c
	gcc -o wordgen.exe wordgen.c

clean:
	rm -f *.exe *.stackdump *~

backup:
	test -d backups || mkdir backups
	cp *.c backups
