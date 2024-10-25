neon-dash: main.c
	gcc -o neon-dash `pkg-config --cflags raylib` main.c `pkg-config --libs raylib`
