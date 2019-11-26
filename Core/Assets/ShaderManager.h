#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "../Systems/Rendering/Shader.h"

class ShaderManager
{
public:
	std::shared_ptr<Shader> LoadShader(const std::string& name, const std::string& vertex,
		const std::string& fragment, const std::string& geometry = "");

	std::string ReadShader(std::ifstream& file, const std::string& name, const std::string& path);

private:
	static const std::string s_shaderDirectory;
};