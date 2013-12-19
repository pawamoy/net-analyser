ifeq ($(DEBUG),yes)
	CFLAGS = -g
else
	CFLAGS =
endif

CFLAGS += -std=gnu99 -pedantic -Werror -W -Wall -Wextra -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual

OPATH = obj/

vpath %.c src/
vpath %.h include/
vpath %.o obj/
vpath main_% bin/
vpath main bin/
vpath %.a lib/

main : main.o netanalyser.o traceroute.o ping.o common.o log.o | bin
	/usr/bin/gcc $(CFLAGS) -o $@ $(OPATH)main.o $(OPATH)netanalyser.o $(OPATH)traceroute.o $(OPATH)ping.o $(OPATH)common.o $(OPATH)log.o
	/bin/mv $@ bin/

main_ping : main_ping.o ping.o common.o log.o | bin
	/usr/bin/gcc $(CFLAGS) -o $@ $(OPATH)main_ping.o $(OPATH)ping.o $(OPATH)common.o $(OPATH)log.o
	/bin/mv $@ bin/

main_traceroute : main_traceroute.o traceroute.o common.o log.o | bin
	/usr/bin/gcc $(CFLAGS) -o $@ $(OPATH)main_traceroute.o $(OPATH)traceroute.o $(OPATH)common.o $(OPATH)log.o
	/bin/mv $@ bin/

main.o : main.c netanalyser.h traceroute.h ping.h common.h log.h
main_traceroute.o : main_traceroute.c traceroute.h common.h log.h
main_ping.o : main_ping.c ping.h common.h log.h
netanalyser.o : netanalyser.c netanalyser.h traceroute.h ping.h common.h log.h
traceroute.o : traceroute.c traceroute.h log.h common.h
ping.o : ping.c ping.h common.h log.h
common.o : common.c common.h
log.o : log.c log.h

%.o : | obj
	/usr/bin/gcc $(CFLAGS) -c $<
	/bin/mv $@ $(OPATH)

doc :
	/usr/bin/doxygen Doxyfile

obj : 
	/bin/mkdir obj

lib : 
	/bin/mkdir lib

bin : 
	/bin/mkdir bin

clean : 
	/bin/rm -rf obj bin
	
cleanlog :
	/bin/rm Log*
	
cleanall : clean
	/bin/rm -rf doc

archive : 
	/bin/tar -f projetRP.tar.gz -cvz src/*.c include/*.h Makefile Doxyfile
