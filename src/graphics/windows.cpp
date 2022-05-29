#include "windows.h"
#include "imgui_tools.h"
#include "imgui_markdown.h"

void graphics::ViewportRenderer::renderMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Open");
			ImGui::MenuItem("Save");
			ImGui::MenuItem("Close");

			ImGui::Separator();

			if (ImGui::BeginMenu("Import"))
			{
				ImGui::MenuItem("Text");
				ImGui::MenuItem("Image");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Export"))
			{
				ImGui::MenuItem("PDF");
				ImGui::MenuItem("HTML");
				ImGui::MenuItem("LaTeX");
				ImGui::EndMenu();
			}

			ImGui::Separator();

			ImGui::MenuItem("Settings");
			ImGui::MenuItem("Quit");

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			for (StaticWindow* window : staticWindows)
				ImGui::MenuItem(window->getName().data(), NULL, &window->_active);

			ImGui::Separator();

#ifdef DEBUG
			ImGui::MenuItem("Demo Window", NULL, &active_demo_window);
			ImGui::MenuItem("Stack Tool", NULL, &active_stack_tool_window);
#endif
			ImGui::MenuItem("Style Editor", NULL, &active_style_editor_window);
			ImGui::MenuItem("UI Performance Metrics", NULL, &active_metrics_window);
			ImGui::MenuItem("About Dear ImGui", NULL, &active_about_window);
			ImGui::EndMenu();
		}

		ImGui::Separator();

#ifdef SHOW_FPS_COUNTER
		ImGui::TextDisabled("%2.1f FPS [%2.0f ms/f]", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Separator();
#endif

#ifdef DEBUG
		ImGui::TextDisabled(VERSION_TRIM);
#else
		ImGui::TextDisabled(VERSION_SHORT);
#endif
		ImGui::EndMainMenuBar();
	}
}

void graphics::ViewportRenderer::renderWindows()
{
	// Render all static windows
	for (StaticWindow* window : staticWindows)
		if (window->_active)
			window->render(&window->_active);

#ifdef DEBUG
	if (active_demo_window)
		ImGui::ShowDemoWindow(&active_demo_window);
	if (active_stack_tool_window)
		ImGui::ShowStackToolWindow(&active_stack_tool_window);
#endif
	if (active_metrics_window)
		ImGui::ShowMetricsWindow(&active_metrics_window);
	if (active_about_window)
		ImGui::ShowAboutWindow(&active_about_window);
}

void graphics::LibraryWindow::render(bool* open)
{
	ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Library Viewer", open, ImGuiWindowFlags_MenuBar))
	{
		renderMenuBar();
		renderLibrary(library);
	}
	ImGui::End();
}

void graphics::LibraryWindow::renderMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::BeginMenu("Add"))
			{
				if (ImGui::MenuItem("Shelf"))
				{
					if (currentShelf != nullptr)
						currentShelf->subshelfs.emplace_back();
					else
						library->shelfs.emplace_back();
				}

				if (ImGui::MenuItem("Book"))
					if (currentShelf != nullptr)
						currentShelf->books.emplace_back();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Delete"))
			{
				if (ImGui::MenuItem("Shelf"))
					if (library->deleteShelf(currentShelf))
						currentShelf = nullptr;

				if (ImGui::MenuItem("Book"))
					if (library->deleteBook(currentBook))
						currentBook = nullptr;

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Save"))
			library->save();

		ImGui::EndMenuBar();
	}
}

void graphics::LibraryWindow::renderLibrary(storage::Library* library)
{
	for (auto& shelf : *library)
		renderShelf(shelf);
}

void graphics::LibraryWindow::renderShelf(storage::LibraryShelf& shelf)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	bool node_open = ImGui::TreeNodeEx((void*) nullptr,
			ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_SpanFullWidth
			| (currentShelf == &shelf ? ImGuiTreeNodeFlags_Selected : 0),
			shelf.getName().c_str());
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	{
		if (currentShelf != &shelf)
			currentShelf = &shelf;
		else
			currentShelf = nullptr;
	}
	if (node_open)
	{
		for (auto& sub : shelf.subshelfs) renderShelf(sub);
		for (auto& book : shelf) renderBook(book);
		ImGui::TreePop();
	}
}

void graphics::LibraryWindow::renderBook(storage::LibraryBook& book)
{
	ImGui::TreeNodeEx((void*) nullptr,
			ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_SpanFullWidth
			| ImGuiTreeNodeFlags_Leaf
			| ImGuiTreeNodeFlags_NoTreePushOnOpen
			| (currentBook == &book ? ImGuiTreeNodeFlags_Bullet : 0),
			book.getName().c_str());
	if (ImGui::IsItemClicked())
		currentBook = &book;
}

void graphics::EditorWindowTest::render(bool* open)
{
	ImGui::SetNextWindowSize(ImVecN<2>(0.0f, 500.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Editor Test", open, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("Undo", "Ctrl-Z", nullptr, editor.CanUndo()))
				editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
				editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, editor.HasSelection()))
				editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, editor.HasSelection()))
				editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			ImGui::Text("%4d lines", editor.GetTotalLines());

			ImGui::EndMenuBar();
		}

		editor.Render("TextEditor", ImVec2(0, ImGui::GetWindowHeight() - (ImGui::GetTextLineHeightWithSpacing() * 4)));

		auto cpos = editor.GetCursorPosition();
		ImGui::Text("%c%c | %6d:%-6d | %s | %s | UTF-8",
				editor.CanUndo() ? '<' : ' ',
				editor.CanRedo() ? '>' : ' ',
				cpos.mLine + 1, cpos.mColumn + 1,
				editor.IsOverwrite() ? "Ovr" : "Ins",
				editor.GetLanguageDefinition().mName.c_str());
	}
	ImGui::End();
}

void graphics::MarkdownWindowTest::render(bool* open)
{
	//ImGui::SetNextWindowPos(getViewportCenter(ImGui::GetWindowViewport()),
	//		ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Markdown Test", open))
	{
		ImGui::MarkdownConfig config = {
		};
		std::string data = "# H1 Header Test\n\nTest lorem ipsum from someone to lazy to google it but needs text anyway to be here to test shit.\n\n## H2 Header Test\n\nAnother test this time for *emphasis* and additionally **strong emphasis**.\n\n### H3 Header Test\n\nThis time there is a line below this text:\n\n___\n\n# List Test\n\n  * Entry one\n  * Entry two\n    * Sub entry\n  * Entry three\n";
		ImGui::Markdown(data.c_str(), data.length(), config);
	}
	ImGui::End();
}
