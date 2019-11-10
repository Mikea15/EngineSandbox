#pragma once

#include <string>
#include <algorithm>
#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>

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

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	static bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), static_cast<int>(values.size()));
	}

	static bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), static_cast<int>(values.size()));
	}

	static void Color3(const char* label, glm::vec3& outColor, ImGuiColorEditFlags flags = ImGuiColorEditFlags_RGB)
	{
		float color[3] = { outColor.x, outColor.y, outColor.z };
		ColorEdit3(label, color, flags);
		outColor = glm::vec3(color[0], color[1], color[2]);
	}
}