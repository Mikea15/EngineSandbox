#include "ShaderManager.h"

#include <string>
#include <fstream>

std::shared_ptr<Shader> ShaderManager::LoadShader(const std::string& name, const std::string& vertexFilePath, const std::string& fragmentFilePath, const std::string& geometryFilePath, bool reload)
{
	std::ifstream vertFile, fragFile, geomFile;
	vertFile.open(vertexFilePath);
	fragFile.open(fragmentFilePath);

	if (!vertFile.is_open() || !fragFile.is_open())
	{
		std::cerr << "[ShaderManager] Can't open shader files\n";
		return {};
	}

	std::string vertSource = ReadShader(vertFile, name, vertexFilePath);
	std::string fragSource = ReadShader(fragFile, name, fragmentFilePath);

	std::string geomSource = "";
	if (!geometryFilePath.empty())
	{
		geomFile.open(geometryFilePath);
		if (geomFile.is_open())
		{
			geomSource = ReadShader(geomFile, name, geometryFilePath);
		}
		geomFile.close();
	}

	vertFile.close();
	fragFile.close();

	std::cout << "[ShaderManager] Compiling shader: " << name << " ... ";
	auto shader = std::make_shared<Shader>(name, vertSource, fragSource, geomSource, vertexFilePath, fragmentFilePath, geometryFilePath);
	if (shader != nullptr) 
	{
		std::cout << "[ok]\n";
		if (!reload) 
		{
			m_loadedShaders.push_back(shader);
		}
		return shader;
	}
	std::cout << "[fail]\n";
	return {};
}

std::string ShaderManager::ReadShader(std::ifstream& file, const std::string& name, const std::string& path)
{
	const std::string directory = path.substr(0, path.find_last_of("/\\"));
	std::string line;
	std::string source;

	while (std::getline(file, line))
	{
		if (line.substr(0, 8) == "#include")
		{
			const size_t firstQuotePos = line.find_first_of('"');
			const size_t lastQuotePos = line.find_last_of('"');

			const std::string includeFilePath = line.substr(firstQuotePos + 1, lastQuotePos-firstQuotePos-1);
			const std::string includePath = directory + "/" + includeFilePath;
			std::ifstream includeFile(includePath);

			if (includeFile.is_open())
			{
				source += ReadShader(includeFile, name, includePath);
			}
			includeFile.clear();
		}
		else
		{
			source += line + "\n";
		}
	}
	return source;
}

void ShaderManager::ReloadShaderPath(const std::string& filepath)
{
	for (auto& shaderPtr : m_loadedShaders)
	{
		bool needsReload = shaderPtr->GetFragmentPath() == filepath
			|| shaderPtr->GetVertexPath() == filepath
			|| shaderPtr->GetGeometryPath() == filepath;

		if (!needsReload) 
		{
			continue;
		}
		shaderPtr = LoadShader(shaderPtr->GetName(), shaderPtr->GetVertexPath(), shaderPtr->GetFragmentPath(), shaderPtr->GetGeometryPath(), true);
		break;
	}
}
