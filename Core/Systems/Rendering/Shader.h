#pragma once

#include <string>

#include <gl/glew.h>
#include <glm/glm.hpp>

#define INVALID_SHADER_ID 0xFFFFFFFF

class Shader
{
public:
	Shader()
		: m_id(s_InvalidId)
	{}

	Shader(const std::string& vertPath, const std::string& fragPath, const std::string& geomPath);
	
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
	std::string LoadShaderContent(const std::string& filePath) const;
	void CheckCompileErrors(GLuint shader, const std::string& type) const;

private:
	unsigned int m_id;

	static const unsigned int s_InvalidId;
};
