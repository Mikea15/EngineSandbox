#pragma once

#include <memory>
#include <chrono>

#include "SDLHandler.h"
#include "State.h"

#include "Assets/AssetManager.h"
#include "SystemComponentManager.h"
#include "Components/System/SceneCameraComponent.h"
#include "Components/System/EditorShaderSystemComponent.h"


class Game
{
public:
	Game();
	~Game();

	void SetState(State* state) { m_gameState = state; }
	int Execute();

	const WindowParams& GetWindowParameters() const { return m_winParams; }
	SDLHandler* GetSDLHandler() { return &m_sdlHandler; }
	SystemComponentManager* GetSystemComponentManager() const { return m_systemComponentManager.get(); }
	AssetManager* GetAssetManager() { return m_assetManager.get(); }

	double GetTimeMS() const { return m_time; }

private:
	void InitSystems();
	void CleanupSystems();

	void LoadConfig();
	void SaveConfig();

private:
	bool m_isRunning;

	std::chrono::high_resolution_clock::time_point m_currentTime;
	float m_deltaTime = 0.0f;
	double m_time = 0.0f;

	State* m_gameState;

	SDLHandler m_sdlHandler;
	WindowParams m_winParams;

	std::unique_ptr<AssetManager> m_assetManager;

	std::unique_ptr<SystemComponentManager> m_systemComponentManager;

	static const char* s_configFileName;
};

