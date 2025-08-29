// 如果打算从一个 shader 向另一个 shader 发送数据，
// 则必须在发送方中声明一个输出，在接收方中声明一个类似的输入。
// 当类型和变量名都一样的时候，
// OpenGL 就会把两个变量链接到一起，它们之间就能发送数据了（这是在链接程序对象时完成的）
#version 330 core

out vec4 FragColor;
in vec3 ourColor; // 与 vertex shader 中 out vec4 vertexColor 相同，数据会从传递过来

void main()
{
   FragColor = vec4(ourColor, 1.0);
}
