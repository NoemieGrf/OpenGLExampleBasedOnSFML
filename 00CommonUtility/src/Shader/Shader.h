#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Glad/Glad.h"

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);

	~Shader();

	unsigned int GetHandle() const;

	void SetUniformBool(const std::string& name, bool value) const;
	void SetUniformInt(const std::string& name, int value) const;
	void SetUniformFloat(const std::string& name, float value) const;
	void SetUniformMat4(const std::string& name, glm::mat4 mat) const;
	void SetUniformVec3(const std::string& name, glm::vec3 vec) const;
	void SetUniformVec3(const std::string& name, float x, float y, float z) const;

private:
	unsigned int _programHandle;

	void CheckCompileErrors(unsigned int id, const std::string& type);
};
