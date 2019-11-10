#pragma once

#include <string>
#include <algorithm>

namespace Utils
{
	static float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	static std::string Lowercase(const std::string& text)
	{
		std::string lowercase = text;
		std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
		return lowercase;
	}

	static size_t Hash(const std::string& text)
	{
		return std::hash<std::string>{}(text);
	}
}
