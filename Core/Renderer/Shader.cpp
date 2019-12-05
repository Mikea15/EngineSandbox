#include "Shader.h"

const unsigned int Shader::s_InvalidId = UINT_MAX;

Shader::Shader(const std::string& rootDir, const std::string& vertexFilePath, const std::string& fragmentFilePath, bool reload)
{
	LoadShader(rootDir, vertexFilePath, fragmentFilePath, reload);
}

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

GLuint Shader::CreateProgram(const std::string& vertexCode, const std::string& fragmentCode)
{
	const char* vertexCodeC = vertexCode.c_str();
	const char* fragmentCodeC = fragmentCode.c_str();

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexCodeC, 0);
	glCompileShader(vertexShaderId);
	FindAndDisplayShaderError(vertexShaderId, "VertexShader");

	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentCodeC, 0);
	glCompileShader(fragmentShaderId);
	FindAndDisplayShaderError(fragmentShaderId, "FragmentShader");

	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glValidateProgram(programId);
	FindAndDisplayProgramError(programId, "Program");

	return programId;
}

void Shader::LoadShader(const std::string& rootDir, const std::string& vertexFilePath, const std::string& fragmentFilePath, bool reload)
{
	std::vector<std::string> vertexDependencies;
	std::string vertexCodeString = FileIO::ReadTextFile(rootDir, vertexFilePath, true,
		[&vertexDependencies, &rootDir](std::string foundPath) {
			vertexDependencies.push_back(rootDir + foundPath);
		}
	);

	std::vector<std::string> fragmentDependencies;
	std::string fragmentCodeString = FileIO::ReadTextFile(rootDir, fragmentFilePath, true,
		[&fragmentDependencies, &rootDir](std::string foundPath) {
			fragmentDependencies.push_back(rootDir + foundPath);
		}
	);

	std::cout << "[ShaderManager] Creating new shader | v: " << vertexFilePath << " | f: " << fragmentFilePath << "\n";

	m_programId = CreateProgram(vertexCodeString, fragmentCodeString);

#if 0 
	if (!reload)
	{
		unsigned int index = m_shaders.size();
		m_shaders.push_back(shader);

		ShaderFiles files = { rootDir, vertexFilePath, fragmentFilePath };
		m_shaderOriginalFiles.emplace(index, files);

		for (const std::string& vertexDependency : vertexDependencies)
		{
			m_dependentVertexShaders[vertexDependency].emplace(index);
		}

		for (const std::string& fragmentDependency : fragmentDependencies)
		{
			m_dependentFragmentShaders[fragmentDependency].emplace(index);
		}
	}
#endif
}

void Shader::FindAndDisplayShaderError(GLuint shaderId, const std::string& name)
{
	GLint success;
	GLchar errorText[4096];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderId, 4096, NULL, errorText);
		std::cerr << "[Shader][Error][" << name << "]\n" << errorText << "\n";
	}
}

void Shader::FindAndDisplayProgramError(GLuint shaderId, const std::string& name)
{
	GLint success;
	GLchar errorText[4096];
	glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderId, 4096, NULL, errorText);
		std::cerr << "[Shader][Linker][" << name << "]\n" << errorText << "\n";
	}
}

int Shader::GetUniformLocation(const std::string& name) const
{
	return glGetUniformLocation(m_programId, name.c_str());
}
