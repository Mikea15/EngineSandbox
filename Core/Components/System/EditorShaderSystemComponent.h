#pragma once

#include "SystemComponentManager.h"
#include "Assets/ShaderManager.h"

#include <imgui.h>
#include "Imgui/imgui_stdlib.h"

#include "Game.h"

struct ShaderDoc
{
	bool        Open;           // Set when the document is open (in this demo, we keep an array of all available documents to simplify the demo)
	bool        OpenPrev;       // Copy of Open from last update.
	bool        Dirty;          // Set when the document has been modified
	bool        WantClose;      // Set when the document

	std::shared_ptr<Shader> m_shader;

	std::string m_bufferVertex;
	std::string m_bufferFragment;
	std::string m_bufferGeometry;

	ShaderDoc(std::shared_ptr<Shader> shader, bool open = true)
		: m_shader(shader)
	{
		Open = OpenPrev = open;
		Dirty = false;
		WantClose = false;

		m_bufferVertex = m_shader->GetVertexCode();
		m_bufferFragment = m_shader->GetFragmentCode();
		m_bufferGeometry = m_shader->GetGeometryCode();
	}

	const char* GetName() const { return m_shader->GetName().c_str(); }

	void DoOpen() { Open = true; }
	void DoQueueClose() { WantClose = true; }
	void DoForceClose() { Open = false; Dirty = false; }
	void DoSave() 
	{
		m_shader->SetVertexCode(m_bufferVertex);
		m_shader->SetFragmentCode(m_bufferFragment);
		m_shader->SetGeometryCode(m_bufferGeometry);

		Dirty = false;
	}

	void DoSaveAndCompile() 
	{
		DoSave();
		m_shader->CompileShader();
	}

	// static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
	static const ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
	// Display dummy contents for the Document
	static void DisplayContents(ShaderDoc* doc)
	{
		ImGui::PushID(doc);
		ImGui::Text("Document \"%s\"", doc->m_shader->GetName().c_str());
		doc->Dirty = ImGui::InputTextMultiline("##source", &doc->m_bufferFragment,
			ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

		if (ImGui::Button("Save", ImVec2(100, 0))) {
			doc->DoSave();
		}
		ImGui::SameLine();
		
		if(ImGui::Button("Save and Compile", ImVec2(150, 0)))
		{
			doc->DoSaveAndCompile();
		}

		ImGui::PopID();
	}

	// Display context menu for the Document
	static void DisplayContextMenu(ShaderDoc* doc)
	{
		if (!ImGui::BeginPopupContextItem()) 
		{
			return;
		}

		char buf[256];
		sprintf(buf, "Save %s", doc->m_shader->GetName().c_str());
		if (ImGui::MenuItem(buf, "CTRL+S", false, doc->Open)) 
		{
			doc->DoSave();
		}
		if (ImGui::MenuItem("Close", "CTRL+W", false, doc->Open)) 
		{
			doc->DoQueueClose();
		}
		ImGui::EndPopup();
	}
};

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

	bool m_enable = {};
	bool m_open = {};
};