#include "ShaderManager.h"

#include <string>
#include <fstream>

const std::string ShaderManager::s_shaderDirectory = "../../../../data/Shaders/";

std::shared_ptr<Shader> ShaderManager::LoadShader(const std::string& name, const std::string& vertex,
	const std::string& fragment, const std::string& geometry)
{
	const std::string vertexPath = s_shaderDirectory + vertex;
	const std::string fragmentPath = s_shaderDirectory + fragment;
	
	std::ifstream vertFile, fragFile, geomFile;

	vertFile.open(vertexPath);
	fragFile.open(fragmentPath);

	if (!vertFile.is_open() || !fragFile.is_open())
	{
		std::cerr << "[ShaderManager] Can't open shader files\n";
		return {};
	}

	std::string vertDirectory = vertexPath.substr(0, vertexPath.find_last_not_of("/\\"));
	std::string fragDirectory = fragmentPath.substr(0, fragmentPath.find_last_not_of("/\\"));

	std::string vertSource = ReadShader(vertFile, name, vertexPath);
	std::string fragSource = ReadShader(fragFile, name, fragmentPath);

	std::string geomSource = "";
	if (!geometry.empty()) 
	{
		const std::string geometryPath = s_shaderDirectory + geometry;
		geomFile.open(geometryPath);
		if (geomFile.is_open()) 
		{
			geomSource = ReadShader(geomFile, name, geometryPath);
		}
		geomFile.close();
	}

	vertFile.close();
	fragFile.close();

	return std::make_shared<Shader>(name, vertSource, fragSource, geomSource);
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
