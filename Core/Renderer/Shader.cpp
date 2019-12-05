#include "Shader.h"

const unsigned int Shader::s_InvalidId = UINT_MAX;

void Shader::Use() const
{
	glUseProgram(m_programId);
}

// utility uniform functions
void Shader::SetBool(const std::string &name, bool value) const
{
	glUniform1i(GetUniformLocation(name), static_cast<int>(value));
}

void Shader::SetInt(const std::string &name, int value) const
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string &name, float value) const
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVector(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVector(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVector(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetMatrix(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMatrix(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMatrix(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}


int Shader::GetUniformLocation(const std::string& name) const
{
	return glGetUniformLocation(m_programId, name.c_str());
}

