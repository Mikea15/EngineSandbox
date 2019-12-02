#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>

#include <string>

#include "Assets/BaseResource.h"

class Shader
	: public BaseResource
{
public:
	Shader() = default;
	Shader(GLuint programId)
		: m_programId(programId)
	{}

	bool operator==(const Shader& rhs) const {
		return m_programId == rhs.m_programId;
	}

	bool operator!=(const Shader& rhs) const {
		return m_programId != rhs.m_programId;
	}

	bool HasValidProgram() const { return m_programId != s_InvalidId; }
	void Use();

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;

	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec2(const std::string& name, float x, float y) const;

	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetVec3(const std::string& name, float x, float y, float z) const;

	void SetVec4(const std::string& name, const glm::vec4& value) const;
	void SetVec4(const std::string& name, float x, float y, float z, float w);

	void SetMat2(const std::string& name, const glm::mat2& mat) const;
	void SetMat3(const std::string& name, const glm::mat3& mat) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;

private:
	GLuint m_programId = s_InvalidId;

	static const unsigned int s_InvalidId;
};
