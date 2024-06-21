#include <stdio.h>
#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <glad/glad.h>

struct screen {
    int w;
    int h;
};

struct screen screen;
SDL_Window *window;
SDL_GLContext gl_context;

GLuint vertex_array_object = 0; // VAO
GLuint vertex_buffer_object = 0; // VBO

// program object (for the shaders) - this is the shader program
GLuint graphics_pipeline_shader_program = 0;

 const std::string vertex_shader_source =
    "#version 410 core\n"
    "in vec4 position;\n"
    "void main() {\n"
    "    gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
    "}\n"
;

 const std::string fragment_shader_source =
    "#version 410 core\n"
    "out vec4 color;\n"
    "void main() {\n"
    "    color = vec4(0.0f, 0.5f, 1.0f, 1.0f);\n"
    "}\n"
;


GLuint compile_shader(GLuint type, const std::string& source) {
    GLuint shader_object;
    switch (type) {
        case GL_VERTEX_SHADER:
            shader_object = glCreateShader(type);
            break;

        case GL_FRAGMENT_SHADER:
            shader_object = glCreateShader(type);
            break;

        default:
            printf("ERROR!\n");
            exit(-23);
            break;
    }

    const char *src = source.c_str();
    glShaderSource(shader_object, 1, &src, nullptr);
    glCompileShader(shader_object);

    int result;
    glGetShaderiv(shader_object, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &length);
        char *error_messages = new char[length];
        glGetShaderInfoLog(shader_object, length, &length, error_messages);
        if (type == GL_VERTEX_SHADER) {
            printf("ERROR: GL_VERTEX_SHADER compilation failed!\n%s\n", error_messages);
        } else if (type == GL_FRAGMENT_SHADER) {
            printf("ERROR: GL_FRAGMENT_SHADER compilation failed!\n%s\n", error_messages);
        } else {
            printf("ERROR: unknown type!\n%s\n", error_messages);
        }
        delete[] error_messages;
        glDeleteShader(shader_object);
        return 0;
    }
    return shader_object;
}

GLuint create_shader_program(const std::string& vertex_shader_source,
                             const std::string& fragment_shader_source) {
    GLuint program_object = glCreateProgram();

    GLuint my_vertex_shader   = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint my_fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

    glAttachShader(program_object, my_vertex_shader);
    glAttachShader(program_object, my_fragment_shader);
    glLinkProgram(program_object);

    //error check
    glValidateProgram(program_object);
    // todo glDetachShader glDeleteShader
    return program_object;
}

int create_graphics_pipeline(void) {
    graphics_pipeline_shader_program = create_shader_program(vertex_shader_source, fragment_shader_source);
    return 0;
}

bool quit = false;

void print_open_gl_info(void) {
    printf(
        "Vendor: %s\n"
        "Renderer: %s\n"
        "Version: %s\n"
        "Shading Language: %s\n",
        glGetString(GL_VENDOR),
        glGetString(GL_RENDERER),
        glGetString(GL_VERSION),
        glGetString(GL_SHADING_LANGUAGE_VERSION)
    );
}

static int InitializeProgram(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init ERROR");
        return -1;
    }
    screen.h = 1080;
    screen.w = 1080;
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

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        printf("GLAD load failed!\n");
        return -1;
    }

    print_open_gl_info();

    printf("InitializeProgram Success!\n");
    return 0;
}

int VertexSpecification(void) {
    // lives on the CPU
    const std::vector<GLfloat> vertexPosition {
        //  x      y     z
        -0.8f, -0.8f, 0.0f, // vertex 1
         0.8f, -0.8f, 0.0f, // vertex 2
         0.0f,  0.8f, 0.0f  // vertex 3
    };

    // set up stuff on the GPU
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    // start generating our VBO
    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat), vertexPosition.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // unbind
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);

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
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0,0,screen.w, screen.h);
    glClearColor(1.f, 0.f, 1.f, 1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(graphics_pipeline_shader_program);
}

void Draw(void) {
    // bind the array (metadata about the buffer?) and enable attributes
    glBindVertexArray(vertex_array_object);
    // bind the actual data (buffer)
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glDrawArrays(GL_TRIANGLES,0,3);

    // stop using the current graphics pipeline - this is probably necessary when using multiple
    // graphics pipelines
    glUseProgram(0);
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
    // init SDL and start open GL
    if (InitializeProgram()) {
        return -1;
    }

    // define geometry
    if (VertexSpecification()) {
        return -1;
    }

    //
    if (create_graphics_pipeline()) {
        return -1;
    }

    while(MainLoop() == 0);

    CleanUp();
    return 0;
}
