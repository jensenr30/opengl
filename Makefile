default:
	bear -- make app

app: main.cpp Makefile src/camera.cpp
	g++ main.cpp src/glad.c src/camera.cpp -g3 -I./include/ -I./extern/glm/ -o app -lSDL2

clean:
	-rm app
	-rm compile_commands.json
