#include "ShaderManager.h"

#include "Systems/FileIO.h"





Shader* ShaderManager::NotifyShaderFileChanged(const Shader& oldShader)
{
#if 0
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

			return &m_shaders[index];
		}
	}

#endif
	return nullptr;
}



std::vector<Shader> ShaderManager::GetShaderFromPathDependency(const std::string& filePath)
{
	std::vector<Shader> shadersAffected;
#if 0
	for (auto dependency : m_shaderOriginalFiles)
	{
		const std::string vertFile = dependency.second.rootDir + dependency.second.vertexFile;
		const std::string fragFile = dependency.second.rootDir + dependency.second.fragmentFile;

		if (filePath == vertFile || filePath == fragFile)
		{
			Shader shader = GetShader(dependency.first);
			if (shader.HasValidProgram())
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
				if (shader.HasValidProgram()) 
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
				if (shader.HasValidProgram())
				{
					shadersAffected.push_back(shader);
				}
			}
		}
	}
#endif
	return shadersAffected;
}

