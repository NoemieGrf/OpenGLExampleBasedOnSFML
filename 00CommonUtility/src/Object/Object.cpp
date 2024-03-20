#include "Object.h"

Object::Object(const VertexData* pVertexData, const Shader* pShader, const Texture* pMainTexture)
	: _pVertexData(pVertexData)
	, _pShader(pShader)
	, _pMainTexture(pMainTexture)
	, _position(0.0f, 0.0f, 0.0f)
	, _rotation(glm::identity<glm::quat>())
	, _scale(1.0f, 1.0f, 1.0f)
{
}

auto Object::SetPosition(const glm::vec3& position) -> void
{
	_position = position;
}

auto Object::SetRotation(const glm::quat& rotation) -> void
{
	_rotation = rotation;
}

auto Object::SetScale(const glm::vec3& scale) -> void
{
	_scale = scale;
}

auto Object::GetModelMatrix() const -> glm::mat4
{
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), _position);
	glm::mat4 rotation = glm::mat4_cast(_rotation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), _scale);
	return scale * rotation * translate;
}

auto Object::GetPosition() const -> const glm::vec3&
{
	return _position;
}

auto Object::GetVertexData() const-> const VertexData*
{
	return _pVertexData;
}

auto Object::GetMainTexture() const -> const Texture*
{
	return _pMainTexture;
}

auto Object::GetShader() const -> const Shader*
{
	return _pShader;
}
