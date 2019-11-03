#include "ShaderManager.h"

const std::string ShaderManager::s_shaderDirectory = "Data/Shaders/";

Shader ShaderManager::LoadShader(const std::string& name, const std::string& vertex,
	const std::string& fragment, const std::string& geometry)
{
	Shader shader;

	if (name.empty() || vertex.empty() || fragment.empty())
	{
		std::cerr << "[ShaderManager] Wanna load a shader maybe?\n";
		return shader;
	}

	if (FindShader(name, shader))
	{
		return shader;
	}

	const std::string vertexPath = s_shaderDirectory + vertex;
	const std::string fragmentPath = s_shaderDirectory + fragment;
	const std::string geometryPath = s_shaderDirectory + geometry;

	shader = Shader(vertexPath.c_str(), fragmentPath.c_str(), !geometry.empty() ? geometryPath.c_str() : nullptr );

	auto result = m_shaderMap.emplace(name, shader);
	if (result.second)
	{
		return result.first->second;
	}

	return Shader();
}

bool ShaderManager::FindShader(const std::string& name, Shader& outShader) const
{
	const auto& findIt = m_shaderMap.find(name);
	if (findIt != m_shaderMap.end())
	{
		outShader = findIt->second;
		return true;
	}
	return false;
}
