out: main.c
	gcc -o out `pkg-config --cflags raylib` main.c `pkg-config --libs raylib`
