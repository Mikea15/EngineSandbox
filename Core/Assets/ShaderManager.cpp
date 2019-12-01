#include "ShaderManager.h"

#include "Systems/FileIO.h"

GLuint ShaderManager::CreateProgram(const std::string& vertexCode, const std::string& fragmentCode)
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

Shader ShaderManager::LoadShader(const std::string& rootDir, const std::string& vertexFilePath, const std::string& fragmentFilePath, bool reload)
{
	std::vector<std::string> vertexDependencies;
	std::string vertexCodeString = FileIO::ReadTextFile(rootDir, vertexFilePath, true,
		[&vertexDependencies, &rootDir] (std::string foundPath) {
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

	GLuint programId = CreateProgram(vertexCodeString, fragmentCodeString);
	Shader shader(programId);

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

	return programId;
}

Shader ShaderManager::NotifyShaderFileChanged(const Shader& oldShader)
{
	unsigned int index = 0;
	for (; index < m_shaders.size(); ++index)
	{
		if (m_shaders[index] == oldShader)
		{
			break;
		}
	}

	if (index < m_shaders.size())
	{
		auto it = m_shaderOriginalFiles.find(index);
		if (it != m_shaderOriginalFiles.end())
		{
			const bool reloadShader = true;
			Shader newShader = LoadShader(it->second.rootDir, it->second.vertexFile, it->second.fragmentFile, reloadShader);
			
			// Update internal data structures
			m_shaders[index] = newShader;

			std::cout << "[ShaderManager] Reloading Shader | v: " << it->second.vertexFile << " f: " << it->second.fragmentFile << "\n";

			return newShader;
		}
	}

	return Shader();
}

void ShaderManager::FindAndDisplayShaderError(GLuint shaderId, const std::string& name)
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

void ShaderManager::FindAndDisplayProgramError(GLuint shaderId, const std::string& name)
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

std::vector<Shader> ShaderManager::GetShaderFromPathDependency(const std::string& filePath)
{
	std::vector<Shader> shadersAffected;
	for (auto dependency : m_shaderOriginalFiles)
	{
		const std::string vertFile = dependency.second.rootDir + dependency.second.vertexFile;
		const std::string fragFile = dependency.second.rootDir + dependency.second.fragmentFile;

		if (filePath == vertFile || filePath == fragFile)
		{
			Shader shader = GetShader(dependency.first);
			if (shader.IsValid())
			{
				shadersAffected.push_back(shader);
			}
		}
	}

	for (const std::pair<std::string, std::unordered_set<unsigned int>>& dependency : m_dependentVertexShaders)
	{
		if (dependency.first == filePath)
		{
			for (unsigned int index : dependency.second) 
			{
				Shader shader = GetShader(index);
				if (shader.IsValid()) 
				{
					shadersAffected.push_back(shader);
				}
			}
		}
	}

	for (const std::pair<std::string, std::unordered_set<unsigned int>>& dependency : m_dependentFragmentShaders)
	{
		if (dependency.first == filePath)
		{
			for (unsigned int index : dependency.second)
			{
				Shader shader = GetShader(index);
				if (shader.IsValid())
				{
					shadersAffected.push_back(shader);
				}
			}
		}
	}
	return shadersAffected;
}

Shader ShaderManager::GetShader(unsigned int i) const
{
	if (i > m_shaders.size() || i < 0)
	{
		return Shader();
	}
	return m_shaders[i];
}
