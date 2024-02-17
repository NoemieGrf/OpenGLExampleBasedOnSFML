#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::ifstream vertexFile;
		std::fstream fragmentFile;
		std::stringstream vertexSStream;
		std::stringstream fragmentSStream;
		const char* vertexSource;
		const char* fragmentSource;


		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);

		vertexFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		fragmentFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

		try
		{
			if (!vertexFile.is_open() || !fragmentFile.is_open())
				throw std::exception("ERROR");
			vertexSStream << vertexFile.rdbuf();
			fragmentSStream << fragmentFile.rdbuf();

			std::string vertexTmp = vertexSStream.str();
			std::string fragmentTmp = fragmentSStream.str();

			//std::cout << vertexTmp << std::endl;
			//std::cout << fragmentTmp << std::endl;

			vertexSource = vertexTmp.c_str();
			fragmentSource = fragmentTmp.c_str();

			/* 生成Shader，返回ID */
			unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
			unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);

			/* 向Shader中填充代码 */
			// 第一个参数是Shader的ID， 第二个参数指填充进去的字符串个数
			// 第三个参数就是字符串，第四个参数先不在意
			glShaderSource(vertex, 1, &vertexSource, nullptr);
			glShaderSource(fragment, 1, &fragmentSource, nullptr);

			/* 编译 */
			glCompileShader(vertex);
			checkCompileErrors(vertex, "VERTEX");

			glCompileShader(fragment);
			checkCompileErrors(fragment, "FRAGMENT");

			/* 组装Shader程序 */
			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			glLinkProgram(ID);
			checkCompileErrors(ID, "PROGRAM");

			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}
		catch (const std::exception& ex)
		{
			printf(ex.what());
		}
	}

	~Shader() = default;

	/* 使用Shader */
	void use(){ glUseProgram(ID);}

	void setBool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value)); }
	void setInt(const std::string& name, int value) const{ glUniform1i(glGetUniformLocation(ID, name.c_str()), value);}
	void setFloat(const std::string& name, float value) const{ glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
	void setMat4fv(const std::string& name, glm::mat4 mat) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(mat)); }

private:
	unsigned int ID;

	void checkCompileErrors(unsigned int id, std::string type)
	{
		int success;
		char infoLog[1024];

		if (type != "PROGRAM")
		{
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(id, 1024, nullptr, infoLog);
				std::cout << "Shader Compile Error : " << infoLog << std::endl;
			}
		}
		else
		{
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(id, 1024, nullptr, infoLog);
				std::cout << "Program Linking Error : " << infoLog << std::endl;
			}
		}
	}
};
