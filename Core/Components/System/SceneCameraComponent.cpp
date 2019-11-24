#include "SceneCameraComponent.h"

#include "../../Game.h"
#include "../../Systems/Camera/Camera.h"

CLASS_DEFINITION(SystemComponent, SceneCameraComponent)

SceneCameraComponent::SceneCameraComponent()
	: m_mouseSensitivity(10.0f)
{
	m_camera = FlyCamera(glm::vec3(0.0f, 2.0f, -3.0f), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

	m_camera.SetPerspective(glm::radians(70.0f),
		m_windowParams.Width / static_cast<float>(m_windowParams.Height),
		0.1f, 1000.0f);

	m_mousePosition = glm::vec2(m_windowParams.Width * 0.5f, m_windowParams.Height * 0.5f);
}

SceneCameraComponent::~SceneCameraComponent()
{
}

void SceneCameraComponent::Initialize(Game* game)
{
	m_windowParams = game->GetWindowParameters();
	SDL_SetRelativeMouseMode(m_inputGrabMouse ? SDL_TRUE : SDL_FALSE);
}

void SceneCameraComponent::HandleInput(SDL_Event* event)
{
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_F1)
	{
		m_inputGrabMouse = !m_inputGrabMouse;
		SDL_SetRelativeMouseMode(m_inputGrabMouse ? SDL_TRUE : SDL_FALSE);
	}

	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_w) m_inputMoveForward = 1;
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_s) m_inputMoveForward = -1;
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_a) m_inputMoveRight = -1;
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_d) m_inputMoveRight = 1;
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_e) m_inputMoveUp = 1;
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_q) m_inputMoveUp = -1;
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_LSHIFT) m_inputEnableMovementBoost = true;

	if (event->type == SDL_KEYDOWN)
	{
		switch (event->key.keysym.sym)
		{
		case SDLK_r:
			m_camera.SetPosition(glm::vec3(0.0f));
			break;
		default: break;
		}
	}

	if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_w) m_inputMoveForward = 0;
	if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_s) m_inputMoveForward = 0;
	if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_a) m_inputMoveRight = 0;
	if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_d) m_inputMoveRight = 0;
	if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_e) m_inputMoveUp = 0;
	if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_q) m_inputMoveUp = 0;
	if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_LSHIFT) m_inputEnableMovementBoost = false;


	if (event->type == SDL_MOUSEWHEEL) {
		m_fovInputChange -= event->wheel.y;
	}
}

void SceneCameraComponent::PreUpdate(float frameTime)
{

}

void SceneCameraComponent::Update(float deltaTime)
{
	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	m_mousePosition = glm::vec2(x, -y);

	if (m_fovInputChange != 0)
	{
		m_fovChange = m_fovInputChange * m_fovVelocity * m_fovSensitivity * deltaTime;
		m_fovInputChange = 0;
	}
	m_fovChange *= m_fovDamping;
	m_camera.InputScroll(m_fovChange);

	// get camera movement input
	glm::vec3 inputDir(m_inputMoveRight, m_inputMoveUp, m_inputMoveForward);
	m_camera.InputKey(deltaTime, inputDir, m_inputEnableMovementBoost);
	
	if (m_inputGrabMouse)
	{
		m_camera.InputMouse(m_mousePosition);
	}

	m_camera.Update(deltaTime);
}

void SceneCameraComponent::Render(float alpha)
{

}

void SceneCameraComponent::RenderUI()
{

}

void SceneCameraComponent::Cleanup()
{

}
