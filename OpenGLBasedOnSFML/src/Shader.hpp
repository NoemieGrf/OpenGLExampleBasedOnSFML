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
	Shader(const char* vertexPath, const char* fragmentPath);

	~Shader() = default;

	/*  π”√Shader */
	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4fv(const std::string& name, glm::mat4 mat) const;

private:
	unsigned int ID;

	void checkCompileErrors(unsigned int id, const std::string& type);
};
