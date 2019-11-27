#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "../Systems/Rendering/Shader.h"

struct ShaderInfo
{
	std::shared_ptr<Shader> m_shader {};
	std::string m_name {};
	std::string m_vertFilePath {};
	std::string m_fragFilePath {};
	std::string m_geomFilePath {};

	bool m_hasGeometryFilePath = false;
};

class ShaderManager
{
public:
	std::shared_ptr<Shader> LoadShader(const std::string& name, const std::string& vertexFilePath,
		const std::string& fragmentFilePath, const std::string& geometryFilePath = "");
	void ReloadShader(ShaderInfo& si);

	std::string ReadShader(std::ifstream& file, const std::string& name, const std::string& path);

	std::vector<ShaderInfo>& GetShaderInfo() { return m_loadedShaders; }

private:
	std::shared_ptr<Shader> LoadShader(ShaderInfo& si, bool recompileShader = false);

private:
	std::vector<ShaderInfo> m_loadedShaders;
};