#pragma once

#include <string>
#include <fstream>

#include "External/TextEditor.h"
#include "Systems/Rendering/Shader.h"

struct ShaderDoc
{
	ShaderInfo* m_shaderInfo;

	bool Open;           // Set when the document is open (in this demo, we keep an array of all available documents to simplify the demo)
	bool OpenPrev;       // Copy of Open from last update.
	bool Dirty;          // Set when the document has been modified
	bool WantClose;      // Set when the document

	std::string m_vertFileContent;
	std::string m_fragFileContent;
	std::string m_geomFileContent;

	std::function<void(ShaderInfo*)> m_onSave;

	TextEditor m_editorV; bool vActive{};
	TextEditor m_editorF; bool fActive{};
	TextEditor m_editorG; bool gActive{};

	ShaderDoc(ShaderInfo* shader, bool open, std::function<void(ShaderInfo*)> onSaveCallback)
		: m_shaderInfo(shader)
	{
		Open = OpenPrev = open;
		Dirty = false;
		WantClose = false;
		m_onSave = onSaveCallback;
	}

	const char* GetName() const { return m_shaderInfo->m_name.c_str(); }

	void DoOpen()
	{
		m_vertFileContent = LoadFileContent(m_shaderInfo->m_vertFilePath);
		m_fragFileContent = LoadFileContent(m_shaderInfo->m_fragFilePath);
		if (m_shaderInfo->m_hasGeometryFilePath)
		{
			m_geomFileContent = LoadFileContent(m_shaderInfo->m_geomFilePath);
		}

		m_editorV.SetText(m_vertFileContent);
		m_editorF.SetText(m_fragFileContent);
		m_editorG.SetText(m_geomFileContent);

		Open = !m_vertFileContent.empty() && !m_fragFileContent.empty();
	}
	void DoQueueClose() { WantClose = true; }
	void DoForceClose() { Open = false; Dirty = false; }

	void DoSave()
	{
		m_vertFileContent = m_editorV.GetText();
		m_fragFileContent = m_editorF.GetText();
		m_geomFileContent = m_editorG.GetText();

		SaveFileContent(m_shaderInfo->m_vertFilePath, m_vertFileContent);
		SaveFileContent(m_shaderInfo->m_fragFilePath, m_fragFileContent);

		if (m_shaderInfo->m_hasGeometryFilePath)
		{
			SaveFileContent(m_shaderInfo->m_geomFilePath, m_geomFileContent);
		}

		Dirty = false;
		m_onSave(m_shaderInfo);
	}

	void DisplayEditor()
	{
		ImGui::PushItemWidth(-10);
		ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_FittingPolicyDefault_ | ImGuiTabBarFlags_Reorderable;
		if (ImGui::BeginTabBar("##tabs", tabBarFlags))
		{
			auto& editor = GetEditor();

			ImGuiTabItemFlags tabFlags = (editor.CanUndo() ? ImGuiTabItemFlags_UnsavedDocument : 0);
			
			vActive = ImGui::BeginTabItem("vert");
			if (vActive)
			{
				auto cpos = m_editorV.GetCursorPosition();

				editor.Render("Editor");

				ImGui::Text("Filename: %s ", m_shaderInfo->m_vertFilePath.c_str());
				ImGui::Text(" %2d/%-2d %2d lines | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
					editor.IsOverwrite() ? "Ovr" : "Ins",
					editor.CanUndo() ? "*" : " ", editor.GetLanguageDefinition());

				ImGui::EndTabItem();
			}

			fActive = ImGui::BeginTabItem("frag");
			if (fActive)
			{
				auto cpos = m_editorV.GetCursorPosition();

				editor.Render("Editor");

				ImGui::Text("Filename: %s ", m_shaderInfo->m_fragFilePath.c_str());
				ImGui::Text(" %2d/%-2d %2d lines | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
					editor.IsOverwrite() ? "Ovr" : "Ins",
					editor.CanUndo() ? "*" : " ", editor.GetLanguageDefinition());

				ImGui::EndTabItem();
			}

			gActive = ImGui::BeginTabItem("geom");
			if (gActive)
			{
				auto cpos = m_editorV.GetCursorPosition();

				editor.Render("Editor");

				ImGui::Text("Filename: %s ", m_shaderInfo->m_geomFilePath.c_str());
				ImGui::Text(" %2d/%-2d %2d lines | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
					editor.IsOverwrite() ? "Ovr" : "Ins",
					editor.CanUndo() ? "*" : " ", editor.GetLanguageDefinition());

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}



		ImGui::PopItemWidth();
	}

	TextEditor& GetEditor() 
	{
		if (vActive) return m_editorV;
		if (gActive) return m_editorG;
		return m_editorF;
	}

	// static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
	static const ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
	// Display dummy contents for the Document
	static void DisplayContents(ShaderDoc* doc, TextEditor& editor)
	{
		if (ImGui::Button("Save", ImVec2(100, 0))) {
			doc->DoSave();
		}
		ImGui::SameLine();
	}

	// Display context menu for the Document
	static void DisplayContextMenu(ShaderDoc* doc)
	{
		if (!ImGui::BeginPopupContextItem())
		{
			return;
		}

		char buf[256];
		sprintf(buf, "Save %s", doc->m_shaderInfo->m_name.c_str());
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

private:
	std::string LoadFileContent(std::string path)
	{
		std::ifstream fileStream;
		fileStream.open(path.c_str());

		std::stringstream fileContent;
		if (fileStream.is_open())
		{
			fileContent << fileStream.rdbuf();
			fileStream.close();
		}

		return fileContent.str();
	}

	void SaveFileContent(std::string path, std::string content)
	{
		std::ofstream fileStream;
		fileStream.open(path.c_str());

		if (fileStream.is_open()) {
			fileStream << content;
			fileStream.close();
		}
	}
};
