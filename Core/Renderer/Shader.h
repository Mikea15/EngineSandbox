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
	void Use() const;

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;

	void SetVector(const std::string& name, const glm::vec2& value) const;
	void SetVector(const std::string& name, const glm::vec3& value) const;
	void SetVector(const std::string& name, const glm::vec4& value) const;

	void SetMatrix(const std::string& name, const glm::mat2& mat) const;
	void SetMatrix(const std::string& name, const glm::mat3& mat) const;
	void SetMatrix(const std::string& name, const glm::mat4& mat) const;

private:
	int GetUniformLocation(const std::string& name) const;

private:
	GLuint m_programId = s_InvalidId;
	std::string m_name;

	static const unsigned int s_InvalidId;
};
