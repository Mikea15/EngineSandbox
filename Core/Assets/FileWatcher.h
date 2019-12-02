#pragma once

#include <iostream>
#include <filesystem>
#include <set>

// Until Cpp2a is not here.
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif
//~ 

#include "AssetManager.h"
#include "Utils.h"

enum class FileType : int
{
	None = 0,
	// Shaders
	Shader_Vert,
	Shader_Frag,
	Shader_Geom,

	Count
};

struct FileTypeInfo
{
	std::string m_extension;
	std::function<void(const std::string&)> OnFileChanged;
};

class FileWatcher
{
public:
	FileWatcher() = default;

	FileWatcher(const std::string& rootFolder, float updateDelay) 
		: m_rootFolder(rootFolder)
		, m_updateDelay(updateDelay)
	{ }

	void RegisterFileType(FileType fileType, const std::string& extension, std::function<void(const std::string&)> OnFileChanged)
	{
		m_watchedFilesTypes.emplace(fileType, FileTypeInfo({ extension, OnFileChanged }));
	}

	void Update(float deltaTime)
	{
		m_currentTime += deltaTime;
		if (m_currentTime > m_nextAccessTime)
		{
			m_lastAccessTime = m_nextAccessTime;
			m_nextAccessTime = m_lastAccessTime + m_updateDelay;

			AnalizeFoldersAndFiles();
		}
	}

	void AnalizeFoldersAndFiles()
	{
		for (auto& pathIt : std::filesystem::recursive_directory_iterator(m_rootFolder))
		{
			const std::string pathString = pathIt.path().string();
			const std::string extension = pathIt.path().extension().string();
			for (const std::pair<FileType, FileTypeInfo>& info : m_watchedFilesTypes)
			{
				if (info.second.m_extension != extension)
				{
					continue;
				}

				m_watchedFilesPaths[info.first].insert(pathIt.path().string());

				long long time = GetLastModifiedTime(pathString);
				
				auto findIt = m_lastEditTime.find(pathString);
				if (findIt != m_lastEditTime.end())
				{
					if (time > findIt->second)
					{
						findIt->second = time;
						info.second.OnFileChanged( Utils::NormalizePath( pathString ) );
					}
				}
				else
				{
					m_lastEditTime.emplace(pathString, time);
				}
			}
		}
	}

	long long GetLastModifiedTime(const std::string& path)
	{
		struct stat result;
		if (stat(path.c_str(), &result))
		{
			// Error
			return 0;
		}
		return result.st_mtime;
	}

private:
	std::unordered_map<FileType, FileTypeInfo> m_watchedFilesTypes;
	std::unordered_map<FileType, std::set<std::string>> m_watchedFilesPaths;
	std::unordered_map<std::string, long long> m_lastEditTime;

	float m_currentTime = 0.0f;
	float m_lastAccessTime = 0.0f;
	float m_nextAccessTime = 0.0f;
	float m_updateDelay = 0.0f;

	std::filesystem::path m_rootFolder;
};