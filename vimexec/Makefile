CC=cl /nologo
LINK=link
SOURCES=
CFLAGS=
OBJECTS=$(SOURCES:.c=.obj)
LIBS=kernel32.lib shell32.lib
all: vimexec vipe
vimexec: vimexec.c
	$(CC) $(CFLAGS) /DVIM_CONSOLE vimexec.c /link $(LINK_FLAGS) $(LIBS) /OUT:vimexec.exe  
	$(CC) $(CFLAGS) /DVIM_GUI vimexec.c /link $(LINK_FLAGS) $(LIBS) /OUT:gvimexec.exe
vipe: vipe.c
	$(CC) $(CFLAGS) /DVIM_CONSOLE vipe.c /link $(LINK_FLAGS) $(LIBS) /OUT:vipe.exe  
	$(CC) $(CFLAGS) /DVIM_GUI vipe.c /link $(LINK_FLAGS) $(LIBS) /OUT:gvipe.exe
clean:
	del *.o *.exe
