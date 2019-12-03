
#pragma once

#include "../Material.h"
#include "Mesh.h"


struct RenderCommand
{
	Material material;
	Mesh mesh;
};

class GPUCommandList
{
public:
	void AddObject(Material material, Mesh mesh) {
		m_renderCommands.emplace(material, mesh);
	}

	void AddDebugObject(Mesh mesh) {
		m_renderDebugCommands.emplace(m_wireframeMat, mesh);
	}

	std::list<RenderCommand> GetSortedList() {
		std::sort(m_renderCommands.begin(), m_renderCommands.end(), [](const RenderCommand& lhs, const RenderCommand& rhs) { lhs.material < rhs.material; });
		return m_renderCommands;
	}

	voic Clear() { 
		m_renderCommands.clear(); 
		m_renderDebugCommands.clear();
	}

private:
	std::list<RenderCommand> m_renderCommands;
	std::list<RenderCommand> m_renderDebugCommands;

	Material m_wireframeMat;
};