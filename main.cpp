#include <stdio.h>

#include <SDL2/SDL.h>

struct screen {
    int w;
    int h;
};

struct screen screen;
SDL_Window *window;
SDL_GLContext gl_context;

bool quit = false;

static int InitializeProgram(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init ERROR");
        return -1;
    }
    screen.h = 1080;
    screen.w = 1920;
    window = SDL_CreateWindow("OpenGL Window", 0,0, screen.w, screen.h, SDL_WINDOW_OPENGL);
    if (!window) {
        printf("Failed to make window\n");
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        printf("gl context error\n");
        return -1;
    }

    return 0;
}

static void CleanUp(void) {
    SDL_DestroyWindow(window);
    // todo why don't you ask me later?
}

void UserInput(void) {
    SDL_Event e;

    while(SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            printf("Goodbye!\n");
            quit = true;
        }
    }
}

void PreDraw(void) {

}

void Draw(void) {

}

static int MainLoop(void) {
    if (quit) {
        return 1;
    }
    static int counter = 0;
    printf("MainLoop!\n");
    UserInput();
    PreDraw();
    Draw();
    SDL_GL_SwapWindow(window);
    SDL_Delay(16);
    counter++;
    if(counter >= 3) {
        // quit = true;
    }
    return 0;
}

int main(void) {
    if (InitializeProgram()) {
        return -1;
    }

    while(MainLoop() == 0);

    CleanUp();
    return 0;
}
