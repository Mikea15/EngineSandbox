
#include "Game.h"

#include "Systems/FileIO.h"

#include <cmath>
#include <algorithm>

#include "Components/System/FpsCounterSystemComponent.h"

std::string Game::s_configFileName = "../../../../data/config.json";

Game::Game()
	: m_isRunning(true)
	, m_deltaTime(1.0f / 60.0f)
{
	LoadConfig();

	m_sdlHandler = SDLHandler(m_winParams);

	m_systemComponentManager = std::make_unique<SystemComponentManager>();
	m_systemComponentManager->AddComponent<SceneCameraComponent>();
	m_systemComponentManager->AddComponent<FpsCounterSystemComponent>();

	m_assetManager = std::make_unique<AssetManager>();
}

Game::~Game()
{
	SaveConfig();
}

void Game::InitSystems()
{
	srand(static_cast<unsigned int>(time(NULL)));

	m_sdlHandler.Init();

	m_assetManager->Initialize();

	// system components
	m_systemComponentManager->Initialize(this);

	m_gameState->Init(this);
}

int Game::Execute()
{
	InitSystems();

	m_currentTime = std::chrono::high_resolution_clock::now();
	float accumulator = 0.0f;

	while (m_isRunning)
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_currentTime);
		float frameTime = static_cast<float>(timeSpan.count());

		m_time += frameTime;

		// cap on how many updates we can do.
		// eg. dt: 1/60 ( 60 ups ).
		// frametime: 0.25
		// max ups = frametime / m_deltaTime
		// max ups = 0.25 / 0.016 = 15.6ups
		// for 30 ups -> max frametime 0.25 ( 7.5 )
		// for 60 ups -> max frametime 0.083 ( 5 updates )
		if (frameTime > 0.25)
			frameTime = 0.25;

		m_currentTime = now;

		accumulator += frameTime;

		m_assetManager->Update(frameTime);

		m_systemComponentManager->PreUpdate(frameTime);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// Handle Input
			m_sdlHandler.HandleEvents(&event);

			if (event.type == SDL_QUIT)
			{
				m_isRunning = false;
			}
			if (event.type == SDL_WINDOWEVENT
				&& event.window.event == SDL_WINDOWEVENT_CLOSE
				&& event.window.windowID == SDL_GetWindowID(m_sdlHandler.GetSDLWindow())
				)
			{
				m_isRunning = false;
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			{
				m_isRunning = false;
			}

			// System Components
			m_systemComponentManager->HandleInput(&event);

			m_gameState->HandleInput(&event);
		}

		while (accumulator >= m_deltaTime)
		{
			// system components
			m_systemComponentManager->Update(m_deltaTime);

			// window Update
			m_sdlHandler.Update(m_deltaTime);

			// Update
			m_gameState->Update(m_deltaTime);

			accumulator -= m_deltaTime;
		}

		const float alpha = accumulator / m_deltaTime;

		// Render
		m_sdlHandler.BeginRender();
		m_systemComponentManager->Render(alpha);
		m_gameState->Render(alpha);

		// ui
		m_sdlHandler.BeginUIRender();
		m_systemComponentManager->RenderUI();
		m_gameState->RenderUI();
		m_sdlHandler.EndUIRender();

		m_sdlHandler.EndRender();
	}

	CleanupSystems();

	return 0;
}

void Game::CleanupSystems()
{
	// state cleanup
	m_gameState->Cleanup();
	m_gameState = nullptr;

	// system components
	m_systemComponentManager->Cleanup();
}

void Game::LoadConfig()
{
	std::string config = FileIO::ReadTextFile(s_configFileName);
	if (!config.empty())
	{
		json jsonParams = json::parse(config);
		if (!jsonParams.is_null()) {
			m_winParams = jsonParams;
		}
	}
}

void Game::SaveConfig()
{
	std::string config = static_cast<json>(m_winParams).dump(4);
	FileIO::SaveTextFile(s_configFileName, config);
}
