CC = gcc
CFLAGS = -W -Wall -Wextra
RM = rm -f
#RM = del /F
TARGET = test
#TARGET = test.exe
LIBS = -lncurses
#LIBS = -lpdcurses

.PHONY: all clean

all: $(TARGET)

$(TARGET): test.o libmenu.o
	$(CC) -o $@ test.o libmenu.o $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o

# Deps
test.o: libmenu.h
libmenu.o: libmenu.h
