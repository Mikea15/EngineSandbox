#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Renderer/Shader.h"

class ShaderManager
{
public:
	GLuint CreateProgram(const std::string& vertexCode, const std::string& fragmentCode);
	Shader LoadShader(const std::string& rootDir, const std::string& vertexFilePath, const std::string& fragmentFilePath, bool reload = false);

	Shader* NotifyShaderFileChanged(const Shader& oldShader);

	std::vector<Shader>& GetShaders() { return m_shaders; }

	void FindAndDisplayShaderError(GLuint shaderId, const std::string& name);
	void FindAndDisplayProgramError(GLuint shaderId, const std::string& name);

	std::vector<Shader> GetShaderFromPathDependency(const std::string& filePath);

private:
	Shader GetShader(unsigned int i) const;

private:
	struct ShaderFiles 
	{
		std::string rootDir;
		std::string vertexFile;
		std::string fragmentFile;
	};

	std::vector<Shader> m_shaders;
	std::unordered_map<unsigned int, ShaderFiles> m_shaderOriginalFiles;
	std::unordered_map<std::string, std::unordered_set<unsigned int>> m_dependentVertexShaders;
	std::unordered_map<std::string, std::unordered_set<unsigned int>> m_dependentFragmentShaders;
};