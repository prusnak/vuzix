NAME=spherevr
CC=gcc
CFLAGS=$(shell pkg-config --cflags sdl gl glu SDL_image) -Wall
LDFLAGS=$(shell pkg-config --libs sdl gl glu SDL_image)
OBJ=$(NAME).o vuzix.o

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(NAME) $(OBJ)
