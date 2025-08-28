#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>  // 包含glad来获取所有的必须OpenGL头文件

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
    // 程序ID
    uint32_t id_;

    // 构造器，读取并构建着色器
    Shader(const char *vertexPath, const char *fragmentPath);
    // 使用/激活程序
    void use();
    // uniform工具函数
    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
};

#endif
