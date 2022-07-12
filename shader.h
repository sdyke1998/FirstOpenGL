#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Light.h>

#define WHITE glm::vec3(1.0f, 1.0f, 1.0f)
#define RED glm::vec3(1.0f, 0.0f, 0.0f)
#define BLUE glm::vec3(0.0f, 0.0f, 1.0f)
#define GREEN glm::vec3(0.0f, 1.0f, 0.0f)
#define BLACK glm::vec3(0.0f, 0.0f, 0.0f)

class Shader {

public:

	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);
	void use();
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMat4(const std::string& name, glm::mat4 matrix) const;
	void setVec3(const std::string& name, glm::vec3 vector) const;
	void setVec3(const std::string& name, float v1, float v2, float v3) const;

	void setDirLight(DirLight sun);
	void setPointLight(PointLight light);
	void setSpotLight(SpotLight light);
	void useLight(Light light);
};

Shader::Shader(const char* vertexPath, const char* fragmentPath) {

	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	try {

		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	};

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	};

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
			infoLog << std::endl;
	}
	// delete shaders; they’re linked into our program and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {

	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setMat4(const std::string& name, glm::mat4 matrix) const
{
	glUniformMatrix4fv(
		glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::setVec3(const std::string& name, glm::vec3 vector) const {
	glUniform3fv(
		glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vector)
	);
}
void Shader::setVec3(const std::string& name, float v1, float v2, float v3) const {

	glUniform3f(
		glGetUniformLocation(ID, name.c_str()), v1, v2, v3
	);
}
void Shader::setDirLight(DirLight sun) {

	setVec3("sun.direction", sun.getDirection());
	setVec3("sun.ambient", sun.getAmbient());
	setVec3("sun.diffuse", sun.getDiffuse());
	setVec3("sun.specular", sun.getSpecular());
}

void Shader::setPointLight(PointLight light) {

	glm::vec3 constants = light.getConstants();

	setFloat("pointLight.constant", constants[0]);
	setFloat("pointLight.linear", constants[1]);
	setFloat("pointLight.quadratic", constants[2]);
	setVec3("pointLight.position", light.getPosition());
	setVec3("pointLight.ambient", light.getAmbient());
	setVec3("pointLight.diffuse", light.getDiffuse());
	setVec3("pointLight.specular", light.getSpecular());
}
void Shader::setSpotLight(SpotLight light) {

	glm::vec3 constants = light.getConstants();

	setFloat("spotLight.constant", constants[0]);
	setFloat("spotLight.linear", constants[1]);
	setFloat("spotLight.quadratic", constants[2]);
	setVec3("spotLight.position", light.getPosition());
	setFloat("spotLight.cosPhi", light.getInnerAngle());
	setFloat("spotLight.cosGamma", light.getOuterAngle());
	setVec3("spotLight.direction", light.getDirection());
	setVec3("spotLight.ambient", light.getAmbient());
	setVec3("spotLight.diffuse", light.getDiffuse());
	setVec3("spotLight.specular", light.getSpecular());
}

void Shader::useLight(Light light) {

	Shader::setVec3("lightColor", light.getColour());
}
#endif