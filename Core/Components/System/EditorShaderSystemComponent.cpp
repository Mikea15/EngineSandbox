
#include "EditorShaderSystemComponent.h"

CLASS_DEFINITION(SystemComponent, EditorShaderSystemComponent)

EditorShaderSystemComponent::EditorShaderSystemComponent()
{

}

EditorShaderSystemComponent::~EditorShaderSystemComponent()
{
	m_docs.clear();
}

void EditorShaderSystemComponent::Initialize(Game* game)
{
	m_gamePtr = game;
}

void EditorShaderSystemComponent::HandleInput(SDL_Event* event)
{

}

void EditorShaderSystemComponent::PreUpdate(float frameTime)
{

}

void EditorShaderSystemComponent::Update(float deltaTime)
{

}

void EditorShaderSystemComponent::Render(float alpha)
{

}

void EditorShaderSystemComponent::RenderUI()
{
	ImGui::Begin("Shader Editor", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Open", !m_docs.empty()))
			{
				const int size = m_docs.size();
				for (int i = 0; i < size; i++)
				{
					ShaderDoc* doc = m_docs[i];
					if (!doc->Open)
					{
						if (ImGui::MenuItem(doc->GetName()))
						{
							if (m_openDoc) 
							{
								m_openDoc->DoForceClose();
								m_openDoc = nullptr;
							}

							m_openDoc = doc;
							m_openDoc->DoOpen();
						}
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Load Shaders")) 
			{
				LoadShaders();
			}

			if (ImGui::MenuItem("Save"))
			{
				m_openDoc->DoSave();
			}

			if (ImGui::MenuItem("Close Active File"))
			{
				m_openDoc->DoForceClose();
				m_openDoc = nullptr;
			}

			if (ImGui::MenuItem("Quit", "Alt-F4")) 
			{
				// break;
			}
			ImGui::EndMenu();
		}

		if ( m_openDoc && ImGui::BeginMenu("Edit"))
		{
			auto& editor = m_openDoc->GetEditor();
			bool ro = editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				editor.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
				editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
				editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
				editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
				editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (m_openDoc && ImGui::BeginMenu("View"))
		{
			auto& editor = m_openDoc->GetEditor();
			if (ImGui::MenuItem("Dark palette"))
				editor.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				editor.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				editor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	if (m_openDoc != nullptr && m_openDoc->Open) 
	{
		m_openDoc->DisplayEditor();
	}

	ImGui::End();

	/*
	// Options
	static bool opt_reorderable = true;
	static ImGuiTabBarFlags opt_fitting_flags = ImGuiTabBarFlags_FittingPolicyDefault_;

	if (!ImGui::Begin("Example: Documents", &m_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	// Submit Tab Bar and Tabs
	{
		ImGuiTabBarFlags tab_bar_flags = (opt_fitting_flags) | (opt_reorderable ? ImGuiTabBarFlags_Reorderable : 0);
		if (ImGui::BeginTabBar("##tabs", tab_bar_flags))
		{
			// if (opt_reorderable)
			// 	NotifyOfDocumentsClosedElsewhere(app);

			// [DEBUG] Stress tests
			//if ((ImGui::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g. dragging with this on.
			//if (ImGui::GetIO().KeyCtrl) ImGui::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not very useful as-is anyway..

			// Submit Tabs
			for (int doc_n = 0; doc_n < m_docs.size(); doc_n++)
			{
				ShaderDoc* doc = m_docs[doc_n];
				if (!doc->Open)
					continue;

				ImGuiTabItemFlags tab_flags = (doc->Dirty ? ImGuiTabItemFlags_UnsavedDocument : 0);
				bool visible = ImGui::BeginTabItem(doc->GetName(), &doc->Open, tab_flags);

				// Cancel attempt to close when unsaved add to save queue so we can display a popup.
				if (!doc->Open && doc->Dirty)
				{
					doc->Open = true;
					doc->DoQueueClose();
				}

				ShaderDoc::DisplayContextMenu(doc);
				if (visible)
				{
					ShaderDoc::DisplayContents(doc, m_editor);
					ImGui::EndTabItem();
				}
			}

			ImGui::EndTabBar();
		}
	}

	
	// Update closing queue
	static ImVector<ShaderDoc*> close_queue;
	if (close_queue.empty())
	{
		// Close queue is locked once we started a popup
		for (int doc_n = 0; doc_n < m_docs.size(); doc_n++)
		{
			ShaderDoc* doc = m_docs[doc_n];
			if (doc->WantClose)
			{
				doc->WantClose = false;
				close_queue.push_back(doc);
			}
		}
	}

	// Display closing confirmation UI
	if (!close_queue.empty())
	{
		int close_queue_unsaved_documents = 0;
		for (int n = 0; n < close_queue.Size; n++)
			if (close_queue[n]->Dirty)
				close_queue_unsaved_documents++;

		if (close_queue_unsaved_documents == 0)
		{
			// Close documents when all are unsaved
			for (int n = 0; n < close_queue.Size; n++)
				close_queue[n]->DoForceClose();
			close_queue.clear();
		}
		else
		{
			if (!ImGui::IsPopupOpen("Save?"))
				ImGui::OpenPopup("Save?");
			if (ImGui::BeginPopupModal("Save?"))
			{
				ImGui::Text("Save change to the following items?");
				ImGui::SetNextItemWidth(-1.0f);
				if (ImGui::ListBoxHeader("##", close_queue_unsaved_documents, 6))
				{
					for (int n = 0; n < close_queue.Size; n++)
						if (close_queue[n]->Dirty)
							ImGui::Text("%s", close_queue[n]->GetName());
					ImGui::ListBoxFooter();
				}

				if (ImGui::Button("Yes", ImVec2(80, 0)))
				{
					for (int n = 0; n < close_queue.Size; n++)
					{
						if (close_queue[n]->Dirty)
							close_queue[n]->DoSave();
						close_queue[n]->DoForceClose();
					}
					close_queue.clear();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("No", ImVec2(80, 0)))
				{
					for (int n = 0; n < close_queue.Size; n++)
						close_queue[n]->DoForceClose();
					close_queue.clear();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(80, 0)))
				{
					close_queue.clear();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}

	ImGui::End();
	*/
}

void EditorShaderSystemComponent::Cleanup()
{

}

void EditorShaderSystemComponent::LoadShaders()
{
	m_docs.clear();

	auto shaderManager = m_gamePtr->GetAssetManager()->GetShaderManager();
	std::vector<ShaderInfo>& shaderRefs = shaderManager->GetShaderInfo();

	static std::function<void(ShaderInfo*)> OnSave = [&](ShaderInfo* si) -> void {
		shaderManager->ReloadShader(*si);
	};

	for( int i = 0; i < shaderRefs.size(); ++i )
	{
		m_docs.push_back(new ShaderDoc(&shaderRefs[i], false, OnSave));
	}
}

void EditorShaderSystemComponent::NotifyCloseDoc()
{
	//for (int doc_n = 0; doc_n < m_docs.size(); doc_n++)
	//{
	//	MyDocument* doc = &app.Documents[doc_n];
	//	if (!doc->Open && doc->OpenPrev)
	//		ImGui::SetTabItemClosed(doc->Name);
	//	doc->OpenPrev = doc->Open;
	//}
}
