#include "FpsCounterSystemComponent.h"

#include "../../Game.h"

CLASS_DEFINITION(SystemComponent, FpsCounterSystemComponent)

FpsCounterSystemComponent::FpsCounterSystemComponent()
{
}

FpsCounterSystemComponent::~FpsCounterSystemComponent()
{
}

void FpsCounterSystemComponent::Initialize(Game* game)
{
	
}

void FpsCounterSystemComponent::HandleInput(SDL_Event* event)
{
	
}

void FpsCounterSystemComponent::PreUpdate(float frameTime)
{
	float frameTimeMS = frameTime * 1000.0f;
	m_minFrameTime = (frameTimeMS < m_minFrameTime) ? frameTimeMS : m_minFrameTime;
	m_maxFrameTime = (frameTimeMS > m_maxFrameTime) ? frameTimeMS : m_maxFrameTime;

	m_totalTime += frameTime;
	m_oneSecond += frameTime;
	if (m_oneSecond >= 1.0f)
	{
		m_oneSecond -= 1.0f;
		m_updatesPerSecond = m_updateCicles;
		m_framesPerSecond = m_framesCicles;

		m_updateCicles = 0;
		m_framesCicles = 0;
	}

	m_minFps = (m_framesPerSecond < m_minFps) ? m_framesPerSecond : m_minFps;
	m_maxFps = (m_framesPerSecond > m_maxFps) ? m_framesPerSecond : m_maxFps;

	m_lastFrameTimeMS = frameTimeMS;
}

void FpsCounterSystemComponent::Update(float deltaTime)
{
	m_lastDeltaTime = deltaTime;
	++m_updateCicles;

	m_msBuffer[m_currentMsBufferIndex] = m_lastFrameTimeMS;
	if (m_currentMsBufferIndex < nMsBuffer - 1)
	{
		m_currentMsBufferIndex++;
	}
	else
	{
		for (int i = 0; i < nMsBuffer - 1; ++i)
		{
			m_msBuffer[i] = m_msBuffer[i + 1];
		}
	}

	m_fpsBuffer[m_currentFpsBufferIndex] = m_framesPerSecond;
	if (m_currentFpsBufferIndex < nFpsBuffer - 1)
	{
		m_currentFpsBufferIndex++;
	}
	else
	{
		for (int i = 0; i < nFpsBuffer - 1; ++i)
		{
			m_fpsBuffer[i] = m_fpsBuffer[i + 1];
		}
	}
}

void FpsCounterSystemComponent::Render(float alpha)
{
	m_alpha = alpha;
	++m_framesCicles;
}

void FpsCounterSystemComponent::RenderUI()
{
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	ImGui::Begin("Stats");

	ImGui::Columns(2, NULL, false);

	const int padding = 1;

	std::stringstream fmt;
	fmt << m_lastFrameTimeMS << " (ms)";

	// frame time
	ImGui::PushItemWidth(-padding);
	ImGui::PlotLines("", m_msBuffer, IM_ARRAYSIZE(m_msBuffer), 0,
		fmt.str().c_str(), 80.0f, 10.0f, ImVec2(0, 50));
	ImGui::NextColumn();

	ImGui::PushItemWidth(15);
	ImGui::Text("Time: %0.2f", m_totalTime);
	ImGui::Text("Frametime: %0.2f (ms)", m_lastFrameTimeMS);
	ImGui::Text("Min %.2f", m_minFrameTime);
	ImGui::Text("Max %.2f", m_maxFrameTime);
	ImGui::NextColumn();

	std::stringstream fmtFps;
	fmtFps << "FPS: " << m_framesPerSecond;

	// fps
	ImGui::PushItemWidth(-padding);
	ImGui::PlotLines("", m_fpsBuffer, IM_ARRAYSIZE(m_fpsBuffer), 0, 
		fmtFps.str().c_str(), m_minFps, m_maxFps, ImVec2(0, 50));
	ImGui::NextColumn();

	ImGui::PushItemWidth(15);
	ImVec4 color = (m_framesPerSecond < 30) ? ImVec4(0.9f, 0.7f, 0.f, 1.f) : ImVec4(0.1f, 0.75f, 0.1f, 1.0f);
	ImGui::TextColored(color, fmtFps.str().c_str());
	ImGui::Text("Min %.2f", m_minFps);
	ImGui::Text("Max %.2f", m_maxFps);
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Text("DeltaTime: %.2f (ms)", m_lastDeltaTime);
	ImGui::Text("Updates p/ Second: %.2f", m_updatesPerSecond);
	ImGui::Text("1 (s): %.2f", m_oneSecond);

	ImGui::NextColumn();

	ImGui::Text("UpdateCount: %.2f", m_updateCicles);
	ImGui::Text("FrameCount: %.2f", m_framesCicles);
	ImGui::Text("Updates p/ Frame: %.2f", m_updateCicles / m_framesCicles);
	ImGui::NextColumn();

	ImGui::Text("Interp. Alpha: %.2f", m_alpha);

	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::Checkbox("Demo window", &show_demo_window);
	ImGui::End();

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

void FpsCounterSystemComponent::Cleanup()
{

}
