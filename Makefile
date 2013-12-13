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
vpath main_traceroute bin/
vpath %.a lib/


main_traceroute : main_traceroute.o traceroute.o log.o | bin
	/usr/bin/gcc $(CFLAGS) -o $@ $(OPATH)*
	/bin/mv $@ bin/

main_traceroute.o : main_traceroute.c traceroute.h log.h
traceroute.o : traceroute.c traceroute.h log.h
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
