CC=cl /nologo
LINK=link
SOURCES=vimopen.c
CFLAGS=
OBJECTS=$(SOURCES:.c=.obj)
LIBS=kernel32.lib shell32.lib user32.lib shlwapi.lib

all:
	$(CC) $(CFLAGS) $(SOURCES) /link $(LINK_FLAGS) $(LIBS)
clean:
	del $(OBJECTS) vimopen.exe
