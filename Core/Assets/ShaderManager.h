#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "../Systems/Rendering/Shader.h"

class ShaderManager
{
public:
	Shader LoadShader(const std::string& name, const std::string& vertex,
		const std::string& fragment, const std::string& geometry = "");
	bool FindShader(const std::string& name, Shader& outShader) const;

private:
	std::unordered_map<std::string, Shader> m_shaderMap;

	static const std::string s_shaderDirectory;
};