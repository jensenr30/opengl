#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>

using std::ifstream;

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "triangle.h"

struct screen {
    int w;
    int h;
};

struct screen screen;
SDL_Window *window;
SDL_GLContext gl_context;

GLuint vertex_array_object = 0; // VAO
GLuint vertex_buffer_object = 0; // VBO
// index buffer object - used to store the list of indicies that we want to draw as verticies
GLuint gIndexBufferObject = 0;

// program object (for the shaders) - this is the shader program
GLuint graphics_pipeline_shader_program = 0;

float offset_x = 0.0f;
float offset_y = 0.0f;
float offset_z = -0.5f;

float angle_x = 0;
float angle_y = 0;
float angle_z = 0;

float scale = 1.0f;

void gl_clear_errors(void) {
    while(glGetError() != GL_NO_ERROR) {
    }
}

bool gl_get_error(const char *file, int line) {
    while(GLenum error = glGetError()) {
        printf("%s:%d\n", file, line);
        printf("OpenGL ERROR: %d\n", error);
        return true;
    }
    return false;
}

#define glcheck(x) gl_clear_errors(); x; gl_get_error(__FILE__, __LINE__);

std::string load_shader_from_file(const std::string& filename) {
    std::string shader_source = "";
    std::string line = "";
    std::ifstream file(filename.c_str());

    if (file.is_open()) {
        while(std::getline(file, line)) {
            shader_source += line + '\n';
        }
        file.close();
    }
    return shader_source;
}

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

    std::string vertex_shader_source   = load_shader_from_file("vertex_shader.vert");
    std::string fragment_shader_source = load_shader_from_file("fragment_shader.frag");

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
    screen.w = 1920;
    screen.h = 1080;
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

    int error = SDL_GL_SetSwapInterval(-1); // -1 = vsync
    if (error) {
        printf("error: %d\n", error);
        const char *msg = SDL_GetError();
        printf(msg);
    }

    printf("InitializeProgram Success!\n");
    return 0;
}

int VertexSpecification(void) {
    // lives on the CPU
    std::vector<GLfloat> vertexPositionAndColor {
        //  x      y     z
        0,0, 0.f,                               // vertex 1
                                                            1.0f, 0.0f, 0.0f, // color 1
        1,0, 0.f,                               // vertex 2
                                                            0.0f, 1.0f, 0.0f, // color 2
        equilateral_x, equilateral_y, 0.f,      // vertex 3
                                                            0.0f, 0.0f, 1.0f, // color 3
        equilateral_x + 1, equilateral_y ,0.f,  // vertex 4
                                                            1.0f, 1.0f, 0.0f, // color 4
    };

    // scale the verticies so they fit on the screen
    for (int i = 0; i < vertexPositionAndColor.size(); i++) {
        int p = i % 6;
        // if this is vertex data,
        if (p == 0 || p == 1 || p == 2) {
            // apply an offset
            vertexPositionAndColor[i] *= 0.5;

        }
    }

    // set up stuff on the GPU
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    // start generating our VBO
    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertexPositionAndColor.size() * sizeof(GLfloat), vertexPositionAndColor.data(), GL_STATIC_DRAW);

    // index definition indexing into the vertex data
    const std::vector<GLuint> index_buffer_data {0,1,2,  2,1,3};

    // setup the index buffer object
    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    // populate our index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size()*sizeof(GLuint),
                 index_buffer_data.data(), GL_STATIC_DRAW);

    // vertex data
    glEnableVertexAttribArray(0);
    const int stride = 6 * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    // color data
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(GLfloat)));

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
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_UP]) {
        offset_y += 0.01;
        // printf("UP\n");
    }
    if (state[SDL_SCANCODE_DOWN]) {
        offset_y -= 0.01;
        // printf("DOWN\n");
    }
    if (state[SDL_SCANCODE_LEFT]) {
        offset_x -= 0.01;
        // printf("LEFT\n");
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        offset_x += 0.01;
        // printf("RIGHT\n");
    }
    if (state[SDL_SCANCODE_PAGEDOWN]) {
        offset_z += 0.01;
        printf("offset_z: %f\n", offset_z);
    }
    if (state[SDL_SCANCODE_PAGEUP]) {
        offset_z -= 0.01;
        printf("offset_z: %f\n", offset_z);
    }
    if (state[SDL_SCANCODE_W]) {
        angle_x += 2*pi / 100;
    }
    if (state[SDL_SCANCODE_S]) {
        angle_x -= 2*pi / 100;
    }
    if (state[SDL_SCANCODE_A]) {
        angle_y -= 2*pi / 100;
    }
    if (state[SDL_SCANCODE_D]) {
        angle_y += 2*pi / 100;
    }
    if (state[SDL_SCANCODE_Q]) {
        angle_z += 2*pi / 100;
    }
    if (state[SDL_SCANCODE_E]) {
        angle_z -= 2*pi / 100;
    }
    if (state[SDL_SCANCODE_Z]) {
        scale *= 0.99f;
    }
    if (state[SDL_SCANCODE_C]) {
        scale *= 1/0.99f;
    }
}

void PreDraw(void) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0,0,screen.w, screen.h);
    glClearColor(0.5f, 0.f, 0.3f, 1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(graphics_pipeline_shader_program);

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(offset_x, offset_y, offset_z));
    // retrieve the location of our model matrix

    transform           = glm::rotate(transform, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
    transform           = glm::rotate(transform, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
    transform           = glm::rotate(transform, angle_z, glm::vec3(0.0f, 0.0f, 1.0f));
    transform           = glm::scale(transform,  glm::vec3(scale,scale,scale));

    GLint model_matrix_location
        = glGetUniformLocation(graphics_pipeline_shader_program, "model_matrix");
    if (model_matrix_location >= 0) {
        glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, &transform[0][0]);
        // printf("model_matrix_location: %d\n", model_matrix_location);
    } else {
        printf("ERROR getting location of uniform\n");
        exit(-23);
    }

    glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
    GLint ortho_location = glGetUniformLocation(graphics_pipeline_shader_program, "ortho");
    if (ortho_location >= 0) {
        glUniformMatrix4fv(ortho_location, 1, GL_FALSE, &ortho[0][0]);
    } else {
        printf("ORTHO LOCATION FAIL OPEN GL FAILURE U U U URURURURU /s \n");
        exit(-42);
    }

    // glm::mat4 perspective = glm::perspective((float)pi/4, (float)screen.w/(float)screen.h, 0.1f, 10.0f);
    // GLint perspective_location = glGetUniformLocation(graphics_pipeline_shader_program, "perspective");
    // if (perspective_location >= 0) {
    //     glUniformMatrix4fv(perspective_location, 1, GL_FALSE, &perspective[0][0]);
    // } else {
    //     printf("PERSPECTIVE LICATION FAIL OPEN GL FAILURE U U U URURURURU /s \n");
    //     exit(-42);
    // }
}

void Draw(void) {
    // bind the array (metadata about the buffer?) and enable attributes
    glBindVertexArray(vertex_array_object);
    // bind the actual data (buffer)
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    // old way of drawing vertices
    // glDrawArrays(GL_TRIANGLES,0,6);

    glcheck(glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0));

    // stop using the current graphics pipeline - this is probably necessary when using multiple
    // graphics pipelines
    glUseProgram(0);
}

static int MainLoop(void) {
    static uint32_t last_frame_print_ms = 0;
    static uint32_t last_frame_count = 0;
    static uint32_t frame_counter = 0;

    if (quit) {
        return 1;
    }
    UserInput();
    PreDraw();
    Draw();
    SDL_GL_SwapWindow(window);
    frame_counter++;
    uint32_t time_ms = SDL_GetTicks();
    if (time_ms - last_frame_print_ms >= 1000) {
        printf("FPS: %f\n", (frame_counter - last_frame_count)*1000.0/(float)(time_ms - last_frame_print_ms));
        last_frame_print_ms = time_ms;
        last_frame_count = frame_counter;
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

    if (create_graphics_pipeline()) {
        return -1;
    }

    while(MainLoop() == 0);

    CleanUp();
    return 0;
}
