#ifndef FRACTALS_PLATONIC4D_SHADER_H
#define FRACTALS_PLATONIC4D_SHADER_H

using namespace std;

string readShaderFile(const char* file);

void checkCompileErrors(unsigned int shader, const string& type);

void createShader(unsigned int ID, const char* &code, const char* type);

void initProgram(unsigned int ID, const char* vShader, const char* fShader);

#endif //FRACTALS_PLATONIC4D_SHADER_H
