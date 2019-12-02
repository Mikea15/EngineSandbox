#pragma once

#include <iostream>

#include <vector>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include "IMGUIHandler.h"
#include "WindowParams.h"

class SDLHandler
{
public:
	SDLHandler() = default;
	SDLHandler(WindowParams& params);

	~SDLHandler();

	bool Init();

	void HandleEvents(SDL_Event* event);
	void Update(float deltaTime);

	void BeginRender();
	void EndRender();
	void BeginUIRender();
	void EndUIRender();

	void SetWindowParameters(const WindowParams& params, bool initialSetup = false);

	const Uint32 GetTicks() const { return SDL_GetTicks(); }

	SDL_Window* GetSDLWindow() const { return m_window; }
	const SDL_GLContext& GetGLContext() const { return m_mainGLContext; }

	const char* GetGLSLVersion() const { return m_glslVersion; }

	const std::vector<SDL_DisplayMode>& GetDisplayModes() const { return m_displayModes; }
	
	std::string GetDisplayModeName(int index) const {
		auto it = m_displayModeIndexToName.find(index);
		if (it != m_displayModeIndexToName.end()) { return it->second; }
		return std::string();
	}
	const SDL_DisplayMode* GetCurrentDisplayMode();

	void RenderUI();

private:
	void FindDisplayModes();

private:
	WindowParams* m_params;
	WindowParams m_tempWindowParams;

	SDL_Window* m_window;
	SDL_GLContext m_mainGLContext;

	IMGUIHandler m_uiHandler;

	const char* m_glslVersion = "#version 130";

	std::vector<SDL_DisplayMode> m_displayModes;
	std::unordered_map<int, std::string> m_displayModeIndexToName;

	bool m_lockMouseCursor = false;
};


