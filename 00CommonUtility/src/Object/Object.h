#pragma once

#include <glm/glm.hpp>
#include "../Vertex/VertexData.hpp"
#include "../Camera/Camera.h"
#include "../Shader/Shader.h"
#include "../Texture/Texture.h"

class Object
{
public:
	Object(const VertexData* pVertexData, const Shader* pShader, const Texture* pMainTexture);

public:
	auto SetPosition(const glm::vec3& position) -> void;
	auto SetRotation(const glm::quat& rotation) -> void;
	auto SetScale(const glm::vec3& scale) -> void;

	auto GetPosition() const -> const glm::vec3&;
	auto GetModelMatrix() const -> glm::mat4;
	auto GetVertexData() const-> const VertexData*;
	auto GetMainTexture() const -> const Texture*;
	auto GetShader() const -> const Shader*;

private:
	// World state
	glm::vec3 _position;
	glm::quat _rotation;
	glm::vec3 _scale;

	// Render context
	const VertexData* _pVertexData;
	const Texture* _pMainTexture;
	const Shader* _pShader;
};