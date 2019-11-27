#pragma once

#include "SystemComponentManager.h"
#include "Assets/ShaderManager.h"

#include "Game.h"
#include "External/Imgui/imgui_stdlib.h"

#include "Editor/TextDocument.h"


class EditorShaderSystemComponent
	: public SystemComponent
{
	CLASS_DECLARATION(EditorShaderSystemComponent)

public:
	EditorShaderSystemComponent();
	~EditorShaderSystemComponent() override;

	void Initialize(Game* game) override;
	void HandleInput(SDL_Event* event) override;
	void PreUpdate(float frameTime) override;
	void Update(float deltaTime) override;
	void Render(float alpha) override;
	void RenderUI() override;
	void Cleanup() override;

	void LoadShaders();
	void OpenEditor() { m_open = true; }
	void CloseEditor() { m_open = false; }

	static void NotifyCloseDoc();

private:
	Game* m_gamePtr;

	std::vector<ShaderDoc*> m_docs;

	ShaderDoc* m_openDoc {};

	bool m_enable = {};
	bool m_open = {};
};