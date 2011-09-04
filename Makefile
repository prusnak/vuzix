NAME=sphere3dvr
CC=gcc
CFLAGS=$(shell pkg-config --cflags sdl gl glu SDL_image) -Wall
LDFLAGS=$(shell pkg-config --libs sdl gl glu SDL_image)

$(NAME): $(NAME).o
	$(CC) $(NAME).o -o $(NAME) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(NAME) *.o
