CC = g++
CFLAGS = -Wall -std=c++17 -O3
EXEC_NAME = sfml-labyrinthe
INCLUDES = -I/usr/include/
LIBS = -L/usr/lib/ -lsfml-system -lsfml-window -lsfml-graphics
OBJ_FILES = main.o
INSTALL_DIR = /usr/bin

all: $(EXEC_NAME)

clean:
	rm $(EXEC_NAME) $(OBJ_FILES)

$(EXEC_NAME): $(OBJ_FILES)
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

install:
	cp $(EXEC_NAME) $(INSTALL_DIR)
