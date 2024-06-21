default:
	bear -- make app

app: main.cpp Makefile
	g++ main.cpp src/glad.c -I./include/ -o app -lSDL2

clean:
	-rm app
	-rm compile_commands.json
