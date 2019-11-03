#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <nlohmann/json.hpp>

using namespace nlohmann;

class WindowParams
{
public:
	// Window
	int Height = 600;
	int Width = 800;
	int Depth = 24;
	int ResolutionIndex = 0;
	bool Fullscreen = false;
	int DisplayIndex = 0;

	std::string WindowName = "Sandbox";

	// 0 immediate, 1 wait for sync
	bool VSync = 0;

	int FPSLimit = 90;

	// OpenGL
	int GL_Flags = 0;
	// SDL_GLprofile GL_ProfileMask = SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE;
	int GL_MajorVersion = 4;
	int GL_MinorVersion = 5;
	int GL_DoubleBuffer = 1;
	int GL_Accelerated = 1;
	int GL_DepthSize = 24;
	int GL_StencilSize = 8;
	int GL_MultiSampleBuffers = 1;
	int GL_MultiSamplesSamples = 4;
};

void to_json(json& j, const WindowParams& p);
void from_json(const json& j, WindowParams& p);

