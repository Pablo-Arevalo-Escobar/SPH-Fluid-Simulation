#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include<string>;
#include<fstream>;
#include<sstream>;
#include<iostream>;

class Shader
{
private:
    unsigned int vertex, fragment, geom;
    int success;
    char  infoLog[512];
    void checkCompileErrors(unsigned int shader, std::string type);

public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setFloat3(const std::string& name, float v1, float v2, float v3) const;
    void setMat4(const std::string& name, glm::mat4 transMatrix) const;
    void setVec3(const std::string& name, glm::vec3 vec3) const;
    void setIVec3(const std::string& name, glm::ivec3 vec3) const;

};

