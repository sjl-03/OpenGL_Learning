#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};
/*
 * This reads the shader program in 
 */
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    
    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n'; // this way if the type is vertex, it adds to index 0 etc
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); // &source[0] does the same things, pointer to source char 
    const int SOURCE_CODE_COUNT = 1;
    glShaderSource(id, SOURCE_CODE_COUNT, &src, nullptr);// specifies source for our shader
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        // char* message = (char*)alloca(length * sizeof(char)); this allows as to allocate on the stack dynamically
        // we want to do char message[length], since length is a variable not a constant, we do this
        // this can also solved by making it a heap and deleting it later like so, so assigning it to an unique pointer
        char* message = new char[length];

        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << 
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
            << " shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);

        delete[] message;

        return 0;
    }

    return id;
}

/*
 * Take in the shaderStrings and return an unique identifier
*/
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); // in OpenGL unsigned int here might also be referred as GLuint
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    const int NUM_POSITIONS = 3;
    float positions[NUM_POSITIONS * 2] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f,
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer); // this creates an id (unsigned int) for our 1 buffer and assign it to tghe variable buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // binding means I'm about to work on it
    glBufferData(GL_ARRAY_BUFFER, NUM_POSITIONS * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    // glBindBuffer(GL_ARRAY_BUFFER, 0); This is binding no 

    glEnableVertexAttribArray(0);
    const int ATTRIBUTE_INDEX = 0;
    glVertexAttribPointer(ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "Vertex" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "Fragment" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, NUM_POSITIONS); // draw call, draw the currently bound buffer
        // Note: OpenGL is a state machine, so the buffer currently bounded is the one the draw call use

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}