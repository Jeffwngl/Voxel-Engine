#include "config.h"
#include "shapes/triangle.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

static std::string loadShader(const std::string& filepath) { // load shader
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
    }
    std::string str;
    std::string content;
    while (std::getline(file, str)) {
        content.append(str + "\n");
    }

    return content;
}

static unsigned int compileShader(unsigned int type, const std::string& source) { // compile shader
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // error handling
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "::COMPILATION_FAILED\n" <<
        infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader) { // create shader program and linking
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    //error handling
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
        infoLog << std::endl;
        glDeleteProgram(shaderProgram);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return shaderProgram;
}

int main() {
    char buffer[FILENAME_MAX];
    _getcwd(buffer, FILENAME_MAX);
    std::cout << "Current working directory: " << buffer << std::endl;
    // SHADER SOURCE DEFINITIONS
    // const char *vertexShaderSource = "#version 330 core\n"
    //     "layout (location = 0) in vec3 aPos;\n"
    //     "void main()\n"
    //     "{\n"
    //     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    //     "}\0";
    // const char *fragmentShaderSource = "#version 330 core\n"
    //     "out vec4 FragColor;\n"
    //     "void main()\n"
    //     "{\n"
    //     "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    //     "}\n\0";

    std::string vertexShaderSource = loadShader("../../src/shaders/vertex.txt"); // working directory is in debug
    std::string fragmentShaderSource = loadShader("../../src/shaders/fragment.txt");

    // GLFW initialization
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "GLFW cannot start." << std::endl;
        return -1;
    }

    // GLFW version and profile control
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Voxel Engine", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // call on every window resize

    // GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // find functions for Open GL
        std::cout << "Glad cannot start." << std::endl; 
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 640, 480);


    // SHADER

    // // vertex shader
    // unsigned int vertexShader;
    // vertexShader = glCreateShader(GL_VERTEX_SHADER); // returns a unique identifier
    // glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // attach shader source to shader object
    // glCompileShader(vertexShader); // compiles shader source strings to machine code

    // int success;
    // char infoLog[512];
    // glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    // if (!success) {
    //     glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
    //     infoLog << std::endl;
    // }

    // // fragment shader
    // unsigned int fragmentShader;
    // fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // glCompileShader(fragmentShader);

    // if (!success) {
    //     glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
    //     infoLog << std::endl;
    // }

    // // shader program (linking) to use in OPENGL
    // unsigned int shaderProgram;
    // shaderProgram = glCreateProgram();
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram(shaderProgram);

    // glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    // if (!success) {
    //     glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
    //     infoLog << std::endl;
    // }

    // function ver

    unsigned int shaderProgram = createShader(vertexShaderSource, fragmentShaderSource);

    // use shader
    glUseProgram(shaderProgram);
    // glDeleteShader(fragmentShader);
    // glDeleteShader(vertexShader);


    // VERTEX BUFFER AND ARRAY

    unsigned int VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // generate ID for vertex buffer store this id in VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind ID to vertex buffer object
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    
    // vertex attributes and linking
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0); // tells opengl how to interpret each vertex
    glEnableVertexAttribArray(0); // enables vertex attribute array 

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) { // keeps window up

        // key inputs
        processInput(window); // checks esc

        // rendering inputs
        glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // resets color buffer to defined color

        // triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // buffers and event calls
        glfwPollEvents(); // checks keyboard or mouse events
        glfwSwapBuffers(window);
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

// esc close key input
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// resizing window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}