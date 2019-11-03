#include "ImGuiHandler.h"

IMGUIHandler::IMGUIHandler()
{
	// IMGUI_CHECKVERSION();
	// ImGui::CreateContext();

	// m_io = ImGui::GetIO();
}

void IMGUIHandler::Initialize(SDL_Window* window, SDL_GLContext* glContext, const char* glslVersion)
{
	// ImGui_ImplSDL2_InitForOpenGL(window, &glContext);
	// ImGui_ImplOpenGL3_Init(glslVersion);

	// ImGui::StyleColorsDark();
}

void IMGUIHandler::HandleInput(SDL_Event * event)
{
	// ImGui_ImplSDL2_ProcessEvent(event);
}

void IMGUIHandler::StartRender(SDL_Window* window)
{
	// ImGui_ImplOpenGL3_NewFrame();
	// ImGui_ImplSDL2_NewFrame(window);
	// ImGui::NewFrame();
}

void IMGUIHandler::EndRender()
{
	// ImGui::Render();
	// ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void IMGUIHandler::Cleanup()
{
	// ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplSDL2_Shutdown();
	// ImGui::DestroyContext();
}


