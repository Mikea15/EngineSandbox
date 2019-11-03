#pragma once

union SDL_Event;
class Game;

class State
{
public:
	virtual ~State() = default;

	virtual void Init(Game* game) = 0;
	virtual void HandleInput(SDL_Event* event) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Render(float alpha = 1.0f) = 0;

	virtual void RenderUI() {};
	virtual void Cleanup() = 0;
};