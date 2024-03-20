#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// vertex
	std::ifstream vertexFile(vertexPath);
	if (!vertexFile.is_open())
	{
		std::cout << "File open fail: " << vertexPath << std::endl;
		return;
	}

	std::stringstream vertexSStream;
	vertexSStream << vertexFile.rdbuf();
	std::string vertexTmp = vertexSStream.str();
	const char* vertexSource = vertexTmp.c_str();

	unsigned int vertexHandle = glCreateShader(GL_VERTEX_SHADER);
	::glShaderSource(vertexHandle, 1, &vertexSource, nullptr);
	::glCompileShader(vertexHandle);
	CheckCompileErrors(vertexHandle, "VERTEX");

	// pixel
	std::fstream fragmentFile(fragmentPath);
	if (!fragmentFile.is_open())
	{
		std::cout << "File open fail: " << fragmentPath << std::endl;
		return;
	}

	std::stringstream fragmentSStream;
	fragmentSStream << fragmentFile.rdbuf();
	std::string fragmentTmp = fragmentSStream.str();
	const char* fragmentSource = fragmentTmp.c_str();

	unsigned int pixelHandle = glCreateShader(GL_FRAGMENT_SHADER);
	::glShaderSource(pixelHandle, 1, &fragmentSource, nullptr);
	::glCompileShader(pixelHandle);
	CheckCompileErrors(pixelHandle, "FRAGMENT");

	// link
	_programHandle = glCreateProgram();
	::glAttachShader(_programHandle, vertexHandle);
	::glAttachShader(_programHandle, pixelHandle);
	::glLinkProgram(_programHandle);
	CheckCompileErrors(_programHandle, "PROGRAM");

	// clear up
	::glDeleteShader(vertexHandle);
	::glDeleteShader(pixelHandle);
}

Shader::~Shader()
{
	::glDeleteProgram(_programHandle);
}

unsigned int Shader::GetHandle() const
{
	return _programHandle;
}

void Shader::SetUniformBool(const std::string& name, bool value) const 
{ 
	::glUniform1i(glGetUniformLocation(_programHandle, name.c_str()), static_cast<int>(value));
}

void Shader::SetUniformInt(const std::string& name, int value) const 
{ 
	::glUniform1i(glGetUniformLocation(_programHandle, name.c_str()), value);
}

void Shader::SetUniformFloat(const std::string& name, float value) const 
{ 
	::glUniform1f(glGetUniformLocation(_programHandle, name.c_str()), value);
}

void Shader::SetUniformMat4(const std::string& name, glm::mat4 mat) const 
{ 
	::glUniformMatrix4fv(glGetUniformLocation(_programHandle, name.c_str()), 1, GL_FALSE, value_ptr(mat));
}

void Shader::SetUniformVec3(const std::string& name, glm::vec3 vec) const
{
	::glUniform3fv(glGetUniformLocation(_programHandle, name.c_str()), 1, &vec[0]);
}

void Shader::SetUniformVec3(const std::string& name, float x, float y, float z) const
{
	::glUniform3f(glGetUniformLocation(_programHandle, name.c_str()), x, y, z);
}

void Shader::CheckCompileErrors(unsigned int id, const std::string& type)
{
	int success;
	char infoLog[1024];

	if (type != "PROGRAM")
	{
		::glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			::glGetShaderInfoLog(id, 1024, nullptr, infoLog);
			std::cout << "Shader Compile Error : " << infoLog << std::endl;
		}
	}
	else
	{
		::glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success)
		{
			::glGetProgramInfoLog(id, 1024, nullptr, infoLog);
			std::cout << "Program Linking Error : " << infoLog << std::endl;
		}
	}
}
