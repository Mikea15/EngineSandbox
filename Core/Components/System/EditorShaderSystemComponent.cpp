
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
	// Options
	static bool opt_reorderable = true;
	static ImGuiTabBarFlags opt_fitting_flags = ImGuiTabBarFlags_FittingPolicyDefault_;

	if (!ImGui::Begin("Example: Documents", &m_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	// Menu
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			int open_count = 0;
			for (int doc_n = 0; doc_n < m_docs.size(); doc_n++) 
			{
				open_count += m_docs[doc_n]->Open ? 1 : 0;
			}

			if (ImGui::MenuItem("Load Shader List"))
			{
				LoadShaders();
			}

			if (ImGui::BeginMenu("Open", open_count < m_docs.size()))
			{
				for (int doc_n = 0; doc_n < m_docs.size(); doc_n++)
				{
					ShaderDoc* doc = m_docs[doc_n];
					if (!doc->Open) 
					{
						if (ImGui::MenuItem(doc->GetName()))
						{
							doc->DoOpen();
						}
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Close All Documents", NULL, false, open_count > 0)) 
			{
				for (int doc_n = 0; doc_n < m_docs.size(); doc_n++)
				{
					m_docs[doc_n]->DoQueueClose();
				}
			}
			if (ImGui::MenuItem("Exit", "Alt+F4")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// [Debug] List documents with one checkbox for each
	for (int doc_n = 0; doc_n < m_docs.size(); doc_n++)
	{
		ShaderDoc* doc = m_docs[doc_n];
		if (doc_n > 0)
			ImGui::SameLine();
		ImGui::PushID(doc);
		if (ImGui::Checkbox(doc->GetName(), &doc->Open))
			if (!doc->Open)
				doc->DoForceClose();
		ImGui::PopID();
	}

	ImGui::Separator();

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
					ShaderDoc::DisplayContents(doc);
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
}

void EditorShaderSystemComponent::Cleanup()
{

}

void EditorShaderSystemComponent::LoadShaders()
{
	for (std::pair<size_t, std::shared_ptr<Shader>> shaderPair : m_gamePtr->GetAssetManager()->GetShaders())
	{
		m_docs.push_back(new ShaderDoc(shaderPair.second, true));
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
