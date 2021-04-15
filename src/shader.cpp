#include "../headers/shader.h"

using namespace std;

std::string readShaderFile(const char* file) {
    std::string vertexCode;
    std::ifstream vShaderFile;
    std::stringstream vShaderStream;

    vShaderFile.open(file);
    vShaderStream << vShaderFile.rdbuf();
    vShaderFile.close();
    return vShaderStream.str();
}

void checkCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void createShader(uint32_t ID, const char* &code, const char* type) {
    glShaderSource(ID, 1, &code, nullptr);
    glCompileShader(ID);
    checkCompileErrors(ID, type);
}

void initProgram(uint32_t ID, const char* vShader, const char* fShader) {
    std::string vertexCode = readShaderFile(vShader);
    const char* vShaderCode = vertexCode.c_str();

    std::string fragmentCode = readShaderFile(fShader);
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER), fragment = glCreateShader(GL_FRAGMENT_SHADER);
    createShader(vertex, vShaderCode, "VERTEX");
    createShader(fragment, fShaderCode, "FRAGMENT");

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
