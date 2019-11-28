#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "../Systems/Rendering/Shader.h"

class ShaderManager
{
public:
	std::shared_ptr<Shader> LoadShader(const std::string& name, const std::string& vertexFilePath, const std::string& fragmentFilePath, const std::string& geometryFilePath = "", bool reload = false);

	
	std::string ReadShader(std::ifstream& file, const std::string& name, const std::string& path);

	std::vector<std::shared_ptr<Shader>>& GetShaders() { return m_loadedShaders; }

	void ReloadShaderPath(const std::string& filepath);

private:
	std::vector<std::shared_ptr<Shader>> m_loadedShaders;
};