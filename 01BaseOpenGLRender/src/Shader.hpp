#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.hpp"

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);

	~Shader() = default;

	void Bind();

	void SetUniformBool(const std::string& name, bool value) const;
	void SetUniformInt(const std::string& name, int value) const;
	void SetUniformFloat(const std::string& name, float value) const;
	void SetUniformMat4(const std::string& name, glm::mat4 mat) const;

private:
	unsigned int _programHandle;

	void CheckCompileErrors(unsigned int id, const std::string& type);
};
