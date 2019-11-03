#pragma once

#include <imgui.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>


namespace ImGui
{
	void ColorEdit3(const char* label, glm::vec3& outColor, ImGuiColorEditFlags flags = ImGuiColorEditFlags_RGB)
	{
		float color[3] = { outColor.x, outColor.y, outColor.z };
		ColorEdit3(label, color, flags);
		outColor = glm::vec3(color[0], color[1], color[2]);
	}

}