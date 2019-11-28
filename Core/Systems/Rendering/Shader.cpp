#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

const unsigned int Shader::s_InvalidId = UINT_MAX;

Shader::Shader(const std::string& name, const std::string& vertexSource, 
	const std::string& fragmentSource, const std::string& geometrySource,
	const std::string& vertexPath, const std::string& fragPath, const std::string& geomPath)
	: m_id(s_InvalidId)
	, m_name(name)
	, m_vertFilePath(vertexPath)
	, m_fragFilePath(fragPath)
	, m_geomFilePath(geomPath)
{
	CompileShader(vertexSource, fragmentSource, geometrySource);
}


void Shader::CompileShader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource, bool recompile)
{
	unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertexCodeC = vertexSource.c_str();
	const char* fragmentCode = fragmentSource.c_str();

	glShaderSource(vertexShaderId, 1, &vertexCodeC, NULL);
	glShaderSource(fragmentShaderId, 1, &fragmentCode, NULL);

	glCompileShader(vertexShaderId);
	CheckCompileErrors(vertexShaderId, "VERTEX");

	glCompileShader(fragmentShaderId);
	CheckCompileErrors(fragmentShaderId, "FRAGMENT");

	unsigned int geometryShaderId = UINT_MAX;
	if (!geometrySource.empty())
	{
		const char* fragmentCode = geometrySource.c_str();

		geometryShaderId = glCreateShader(GL_GEOMETRY_SHADER);

		glShaderSource(geometryShaderId, 1, &fragmentCode, NULL);

		glCompileShader(geometryShaderId);
		CheckCompileErrors(geometryShaderId, "GEOMETRY");
	}

	if (!recompile) 
	{
		m_id = glCreateProgram();
	}
	glAttachShader(m_id, vertexShaderId);
	glAttachShader(m_id, fragmentShaderId);

	if (geometryShaderId != UINT_MAX)
	{
		glAttachShader(m_id, geometryShaderId);
	}

	glLinkProgram(m_id);
	CheckCompileErrors(m_id, "PROGRAM");

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
	if (geometryShaderId != UINT_MAX)
	{
		glDeleteShader(geometryShaderId);
	}
}

void Shader::Use()
{
	glUseProgram(m_id);
}

// utility uniform functions
void Shader::SetBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::SetVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::SetVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::SetVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
}

void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::CheckCompileErrors(GLuint shader, const std::string& type) const
{
	GLint success;
	GLchar errorText[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, errorText);
			std::cerr << "[Shader][Program]["<< m_name <<"][" << type << "] " << errorText;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, errorText);
			std::cerr << "[Shader][Linker][" << m_name << "][" << type << "] " << errorText;
		}
	}
}


