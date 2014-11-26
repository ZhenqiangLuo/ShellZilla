
shellZilla:        shellZilla.o
	cc -o shellZilla shellZilla.o -I/usr/include -lreadline -ltermcap

shellZilla.o:       shellZilla.c
	cc -g -c shellZilla.c
clean:
	rm -f shellZilla shellZilla.o

