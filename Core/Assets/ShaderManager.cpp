#include "ShaderManager.h"

#include <string>
#include <fstream>

std::shared_ptr<Shader> ShaderManager::LoadShader(const std::string& name, const std::string& vertexFilePath,
	const std::string& fragmentFilePath, const std::string& geometryFilePath)
{
	ShaderInfo si;
	si.m_name = name;
	si.m_vertFilePath = vertexFilePath;
	si.m_fragFilePath = fragmentFilePath;
	si.m_hasGeometryFilePath = !geometryFilePath.empty();
	if (si.m_hasGeometryFilePath) 
	{
		si.m_geomFilePath = geometryFilePath;
	}
	
	si.m_shader = LoadShader(si);

	m_loadedShaders.push_back(si);

	return si.m_shader;
}

std::shared_ptr<Shader> ShaderManager::LoadShader(ShaderInfo& si, bool recompileShader)
{
	std::ifstream vertFile, fragFile, geomFile;
	vertFile.open(si.m_vertFilePath);
	fragFile.open(si.m_fragFilePath);

	if (!vertFile.is_open() || !fragFile.is_open())
	{
		std::cerr << "[ShaderManager] Can't open shader files\n";
		return {};
	}

	std::string vertSource = ReadShader(vertFile, si.m_name, si.m_vertFilePath);
	std::string fragSource = ReadShader(fragFile, si.m_name, si.m_fragFilePath);

	std::string geomSource = "";
	if (!si.m_hasGeometryFilePath)
	{
		geomFile.open(si.m_geomFilePath);
		if (geomFile.is_open())
		{
			geomSource = ReadShader(geomFile, si.m_name, si.m_geomFilePath);
		}
		geomFile.close();
	}

	vertFile.close();
	fragFile.close();

	auto newShader = std::make_shared<Shader>(si.m_name, vertSource, fragSource, geomSource);
	
	if (recompileShader)
	{
		si.m_shader->CompileShader(newShader->GetVertexCode(), newShader->GetFragmentCode(), newShader->GetGeometryCode(), recompileShader);
	}
	else
	{
		si.m_shader = newShader;
	}
	return si.m_shader;
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

void ShaderManager::ReloadShader(ShaderInfo& si)
{
	const bool recompileShader = true;
	si.m_shader = LoadShader(si, recompileShader);
}
