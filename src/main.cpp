// clangd-format off
#include <glad/glad.h>  // 这个头文件得放在最前面
// clangd-format on

#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "stb_image.h"

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector
// pointing to the right so we initially rotate a bit to the left.
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;  // time between current frame and last frame
float lastFrame = 0.0f;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react
// accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// settings
constexpr uint32_t SCR_WIDTH = 800;
constexpr uint32_t SCR_HEIGHT = 600;

void triagnle()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    const char *vertexShaderSource =
        "#version 330 core\n"                    // 版本声明
        "layout (location = 0) in vec3 aPos;\n"  // layout(locatioon = 0): shader
                                                 // 前一阶段的输入变量和后一阶段的输出变量是通过location来确定关系的
                                                 // in 关键字,
                                                 // 表示该变量为输入变量，来自前一阶段的输入 vec3
                                                 // 表示长度为 3 的向量类型 aPos 变量名
        "void main()\n"  // 会为每个 vertex 执行一次 main() 函数
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"  // out 关键字，表示该变量为输出变量，作为下一阶段的输入
                                 // vec4 表示为长度为 4 的向量类型
                                 // FragColor 变量名
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    /*
    glCreateShader: 创建一个 shader object
    {shader类型}
    返回非零 id
    */
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    /*
    glShaderSource: 设置 shader object 源码
    {id, 数组元素个数，数组首地址，数组元素长度}
    */
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    /*
    glCompileShader: 编译 shader 中保存的源码
    {id}
    */
    glCompileShader(vertexShader);
    // check for shader compile errors
    int32_t success;
    char infoLog[512];
    /*
    glGetShaderiv: 查询 shader object 的参数
    {id, 参数名，出参}
    */
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        /*
        glGetShaderInfoLog: 查询 shader 的 log
        {id, 读取的最大长度上限保护，从log读到实际长度，buffer 地址}
        */
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    // glCreateProgram: 创建一个 program object
    // "A program object is an object to which shader objects can be attached. This provides a
    // mechanism to specify the shader objects that will be linked to create a program."
    // "程序对象是可被 shader object attach 的 object. 它提供了一个机制，用于指明哪些 shader object
    // 被链接为一个程序"
    uint32_t shaderProgram = glCreateProgram();
    /*
    glAttachShader: attach shader object 到 program object
    {program id, shader id}
    */
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram: 执行链接操作
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    /*
    glDeleteShader: 删除 shader object
    如果 shader 此时已 attach 到 program object，则仅标记为删除，直到它不 attack 到任何 program
    object 才删除
    */
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        0.5f,  0.5f,  0.0f,  // 右上角
        0.5f,  -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f,  // 左下角
        -0.5f, 0.5f,  0.0f   // 左上角
    };

    uint32_t indices[] = {
        // 注意索引从0开始!
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形
        0, 1, 3,  // 第一个三角形
        1, 2, 3   // 第二个三角形
    };

    uint32_t Vbo, Vao, Ebo;  // vertex buffer object, vertext array object
    // glGenVertexArrays: 生成 vertex array object name, 此时仅仅生成一个未使用的 id
    // {生成的 id 数量，出参}
    glGenVertexArrays(1, &Vao);
    // glGenBuffers: 生成 buffer object id, 此时仅仅生成一个未使用的 id
    // {生成的 id 数量，出参}
    glGenBuffers(1, &Vbo);
    glGenBuffers(1, &Ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure
    // vertex attributes(s).
    // glBindVertexArray: 绑定 vertex array object
    glBindVertexArray(Vao);

    // glBindBuffer: 给 buffer object name(id) 绑定指定类型的 buffer object
    // {buffer type, id}
    // 同一个 buffer object id 可以同时绑定多个 buffer object，只要它们是不同的缓冲类型
    // 绑定后，对该缓冲类型的 buffer object 的操作，都是对其绑定的 buffer object name 的操作
    // 我简单理解是把 GL_ARRAY_BUFFER 赋值为 Vbo, 之后使用 GL_ARRAY_BUFFER，都会使用属于 Vbo 的内存
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);
    /*
    glBufferData: 创建并初始化 buffer object 的存储空间
    {buffer 类型，存储数据长度，数据首地址，使用方式}
    因为上一步 glBindBuffer 已经为 GL_ARRAY_BUFFER 绑定了 id 为 Vbo
    所以该函数本次操作的就是 Vbo，而无需传参 Vbo
    */
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /*
    glVertexAttribPointer: 定义如何解析 vertex 属性
    {location index, 与 shader 源码中 location = 0 的值对应
    每个顶点属性的成分个数，例如每个顶点有 x,y,z 三个成分，就是 3
    数据类型，
    是否标准化/归一化，
    步长，两个连续 vertex 属性起始位置之间的长度
    偏移, 认为当前绑定的 GL_ARRAY_BUFFER (当前是 Vbo 的)的第一个 vertext
    属性从缓冲区首地址+偏移开始}
    这个函数告诉了 opengl，location = 0 的变量 aPos 如何从当前的 GL_ARRAY_BUFFER 接收数据
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // glEnableVertexAttribArray :启用 vertex attribute array
    // {location index}  对应 shader 源码中的 location = 0 字段
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex
    // attribute's bound vertex buffer object so afterwards we can safely unbind
    // 请注意，这是允许的，对 glVertexAttribPointer 的调用已将 VBO
    // 注册为顶点属性的绑定的 vertex buffer object，
    // 因此在glVertexAttribPointer之后我们可以安全地解除解除 GL_ARRAY_BUFFER 与 Vbo 绑定
    // 传参为 0, 表示解除 GL_ARRAY_BUFFER 与 Vbo 的绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS
    // stored in the VAO; keep the EBO bound.
    // 当 VAO 激活时，不要解绑 EBO，因为绑定的 EBO 被存储在 VAO 中；需保持 EBO 绑定
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but
    // this rarely happens. Modifying other VAOs requires a call to glBindVertexArray anyways so we
    // generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // 您可以解除绑定 VAO，在之后其他 VAO 调用就不会意外地修改这个 VAO，但这种情况很少发生。
    // 修改其他 VAO 无论如何都需要调用 glBindVertexArray，因此我们通常不会在非必要的情况下解除
    // VAO（或 VBO）的绑定。
    // glBindVertexArray: 解除 VAO 的绑定
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    // 设置多边形的光栅化模式，这里传参意思是设置为线框多边形
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        // glUseProgram: 安装 program object 作为当前渲染状态的一部分
        // 调用该函数后，每个 shader 和 rendering 都会调用这个 program object
        glUseProgram(shaderProgram);
        glBindVertexArray(Vao);  // seeing as we only have a single VAO there's no need to bind it
                                 // every time, but we'll do so to keep things a bit more organized
        // glDrawArrays : 从数组数据渲染，
        // {mode, first 数组中的起始位置, count 要渲染的索引数量}
        // 它使用当前激活的着色器，之前定义的顶点属性配置，和VBO的顶点数据（通过VAO间接绑定）来绘制图元。
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // {mode, 渲染的元素数量，元素类型，偏移}
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    glDeleteBuffers(1, &Ebo);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

void shader()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // const char *fragmentShaderSourceUniform =
    //     "#version 330 core\n"
    //     "out vec4 FragColor;\n"
    //     "uniform vec4 ourColor;\n"  // uniform 修饰的变量为全局 global 变量，可在任意阶段被任意
    //                                 // shader 访问
    //     "void main()\n"
    //     "{\n"
    //     "   FragColor = ourColor;\n"
    //     "}\0";

    Shader shader("./shader/vertex_shader.vs", "./shader/fragment_shader.fs");
    float vertices[] = {
        // 位置              // 颜色
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // 右下
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // 左下
        0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f   // 顶部
    };
    uint32_t indices[] = {
        // 注意索引从0开始!
        0, 1, 2,  // 第一个三角形
    };

    uint32_t Vbo, Vao, Ebo;  // vertex buffer object, vertext array object
    glGenVertexArrays(1, &Vao);
    glGenBuffers(1, &Vbo);
    glGenBuffers(1, &Ebo);
    glBindVertexArray(Vao);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        // float timeValue = glfwGetTime();
        // float greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
        // 获取 uniform 的 location 不要求此前使用了 shader program
        // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // 但更新 uniform 的值时，要求先 调用 glUseProgram 使用 shader program
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        glBindVertexArray(Vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    glDeleteBuffers(1, &Ebo);
    // glDeleteProgram(shaderProgram);

    glfwTerminate();
}

void texture()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // 生成纹理 id
    uint32_t texture1;
    glGenTextures(1, &texture1);
    // 在绑定纹理之前先激活纹理单元，
    glActiveTexture(GL_TEXTURE0);  // 纹理单元 0 是默认激活的
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    // S 是竖直的 y 轴，T 是横向的 x 轴
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    int width, height, nrChannels;
    unsigned char *data = stbi_load("./texture/container.jpg", &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cout << "load texture1 fail" << std::endl;
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // 生成 mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    uint32_t texture2;
    glGenTextures(1, &texture2);
    // 在绑定纹理之前先激活纹理单元，
    glActiveTexture(GL_TEXTURE1);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    stbi_set_flip_vertically_on_load(true);  // 设置加载图片时翻转一下 y 轴
    data = stbi_load("./texture/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cout << "load texture2 fail" << std::endl;
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    Shader shader("./shader/texture_shader.vs", "./shader/texture_shader.fs");
    shader.use();  // 设置 uniform 变量之前需要先激活着色器程序！
    // 设置采样器从哪个纹理单元读取数据，此处为 0
    glUniform1i(glGetUniformLocation(shader.id_, "texture1"), 0);  // 手动设置
    // 设置采样器从哪个纹理单元读取数据，此处为 1
    shader.set_int("texture2", 1);  // 或者使用着色器类设置

    float vertices[] = {
        // ---- 位置 ----   ---- 颜色 ----    - 纹理坐标 -
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // 右上
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 右下
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 左下
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // 左上
    };
    uint32_t indices[] = {
        0, 1, 3,  // 第一个三角形
        1, 2, 3,  // 第二个三角形
    };

    uint32_t Vbo, Vao, Ebo;
    glGenVertexArrays(1, &Vao);
    glGenBuffers(1, &Vbo);
    glGenBuffers(1, &Ebo);
    glBindVertexArray(Vao);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(Vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    glDeleteBuffers(1, &Ebo);

    glfwTerminate();
}

void coordinate()
{
    /*
     * glfw 初始化
     */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /*
     * glfw 创建及初始化窗口
     */
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /*
     * glad 加载 OpenGL 函数指针
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    /*
     * 设置开启深度测试
     */
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    /*
     * vertex data: x,y,z,u,v;
     */
    float vertices[] = {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
                        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
                        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                        -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

                        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

                        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
                        0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
                        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
                        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                 glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                 glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                 glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                 glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
    /*
     * 绑定 VBO VAO
     */
    unsigned int VBO, Vao;
    glGenVertexArrays(1, &Vao);
    glGenBuffers(1, &VBO);

    glBindVertexArray(Vao);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /*
     * 指定 location 以及 vertex 解读方式
     */
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    /*
     * 加载 texture
     */
    unsigned int texture1, texture2;
    // texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(
        true);  // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("./texture/container.jpg", &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cout << "Failed to load texture1" << std::endl;
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    // texture 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("./texture/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cout << "Failed to load texture2" << std::endl;
        return;
    }
    // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to
    // tell OpenGL the data type is of GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    /*
     * 设置 texture 的纹理单元 location
     */
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    Shader shader("./shader/coordinate.vs", "./shader/coordinate.fs");
    shader.use();
    shader.set_int("texture1", 0);
    shader.set_int("texture2", 1);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清除颜色缓冲与深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // also clear the depth buffer now!

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        shader.use();

        // create transformations
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                      0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // pass transformation matrices to the shader
        shader.set_mat4("projection", projection);
        // note: currently we set the projection matrix each frame,
        // but since the projection matrix rarely changes it's often
        // best practice to set it outside the main loop only once.
        shader.set_mat4("view", view);

        // render boxes
        glBindVertexArray(Vao);
        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.set_mat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return;
}

void camera()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // 设置摄像机的回调函数
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glEnable(GL_DEPTH_TEST);

    float vertices[] = {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
                        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
                        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                        -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

                        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

                        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
                        0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
                        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
                        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                 glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                 glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                 glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                 glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("./texture/container.jpg", &width, &height, &nrChannels, 0);
    if (data == nullptr) std::cout << "Failed to load texture1" << std::endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("./texture/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data == nullptr) std::cout << "Failed to load texture2" << std::endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    Shader shader("./shader/coordinate.vs", "./shader/coordinate.fs");
    shader.use();
    shader.set_int("texture1", 0);
    shader.set_int("texture2", 1);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        shader.use();

        glm::mat4 projection =
            glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.set_mat4("projection", projection);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.set_mat4("view", view);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.set_mat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return;
}

int main()
{
    // triagnle();
    // shader();
    // texture();
    // coordinate();
    camera();
    return 0;
}
