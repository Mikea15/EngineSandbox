#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Renderer/Shader.h"

class ShaderManager
{
public:
	Shader* NotifyShaderFileChanged(const Shader& oldShader);
	std::vector<Shader> GetShaderFromPathDependency(const std::string& filePath);

private:
	struct ShaderFiles 
	{
		std::string rootDir;
		std::string vertexFile;
		std::string fragmentFile;
	};

	std::unordered_map<unsigned int, ShaderFiles> m_shaderOriginalFiles;
	std::unordered_map<std::string, std::unordered_set<unsigned int>> m_dependentVertexShaders;
	std::unordered_map<std::string, std::unordered_set<unsigned int>> m_dependentFragmentShaders;
};