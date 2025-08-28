// clangd-format off
#include <glad/glad.h>  // 这个头文件得放在最前面
// clangd-format on

#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
constexpr uint32_t SCR_WIDTH = 800;
constexpr uint32_t SCR_HEIGHT = 600;

const char *vertexShaderSource =
    "#version 330 core\n"                    // 版本声明
    "layout (location = 0) in vec3 aPos;\n"  // layout(locatioon = 0): shader
                                             // 前一阶段的输入变量和后一阶段的输出变量是通过location来确定关系的
                                             // in 关键字, 表示该变量为输入变量，来自前一阶段的输入
                                             // vec3 表示长度为 3 的向量类型
                                             // aPos 变量名
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
    "}\n\0";

int main()
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
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

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
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react
// accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
