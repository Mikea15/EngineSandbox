#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <iostream>

#include "Assets/BaseResource.h"
#include "Systems/FileIO.h"

class Shader
	: public BaseResource
{
public:
	Shader() = default;
	Shader(const std::string& rootDir, const std::string& vertexFilePath, const std::string& fragmentFilePath, bool reload = false);

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
	GLuint CreateProgram(const std::string& vertexCode, const std::string& fragmentCode);
	void LoadShader(const std::string& rootDir, const std::string& vertexFilePath, const std::string& fragmentFilePath, bool reload = false);
	
	void FindAndDisplayShaderError(GLuint shaderId, const std::string& name);
	void FindAndDisplayProgramError(GLuint shaderId, const std::string& name);
	
	int GetUniformLocation(const std::string& name) const;

private:
	GLuint m_programId = s_InvalidId;
	std::string m_name;

	static const unsigned int s_InvalidId;
};
