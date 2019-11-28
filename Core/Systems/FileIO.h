
#pragma once

#include <sstream>
#include <string>
#include <fstream>
#include <functional>

class FileIO
{
public:
	static std::string ReadTextFile(const std::string& path)
	{
		std::stringstream stream;
		std::fstream file(path, std::fstream::in);
		if (file.is_open())
		{
			while (!file.eof())
			{
				std::string buffer;
				std::getline(file, buffer);
				stream << buffer + "\n";
			}
		}
		file.close();
		return stream.str();
	}

	static std::string ReadTextFile(const std::string& rootDir, const std::string& path,
		bool autoInclude = true, 
		std::function<void(std::string)> OnStringFound = nullptr)
	{
		std::stringstream stream;
		std::fstream file(rootDir + path, std::fstream::in);
		if (file.is_open())
		{
			while (!file.eof())
			{
				std::string buffer;
				std::getline(file, buffer);

				// Find #include and skip line.
				if (autoInclude && buffer.substr(0, 8) == "#include")
				{
					const size_t firstQuotePos = buffer.find_first_of('"');
					const size_t lastQuotePos = buffer.find_last_of('"');
					const std::string includeFilePath = buffer.substr(firstQuotePos + 1, lastQuotePos - firstQuotePos - 1);
					
					stream << ReadTextFile(rootDir, includeFilePath, false);
				}
				else 
				{
					stream << buffer + "\n";
				}
			}
		}

		file.close();

		return stream.str();
	}

	static void SaveTextFile(const std::string& path, const std::string& content)
	{
		std::ofstream file;
		file.open(path, std::fstream::out);
		if (file.is_open()) 
		{
			file << content;
		}
		file.close();
	}

private:

};