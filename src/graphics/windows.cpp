#include <cmath>
#include "windows.h"
#include "imgui_tools.h"
#include "imgui_markdown.h"

#ifdef DEBUG
	#ifndef SHOW_FPS_COUNTER
		#define SHOW_FPS_COUNTER
	#endif
#endif

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

			for (StaticWindow* window : settingsWindows)
				ImGui::MenuItem(window->getName().data(), NULL, &window->_active);

#ifdef DEBUG
			ImGui::MenuItem("Demo Window", NULL, &active_demo_window);
			ImGui::MenuItem("Stack Tool", NULL, &active_stack_tool_window);
#endif
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

	// Render all settings windows
	for (StaticWindow* window : settingsWindows)
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

void graphics::StyleEditorWindow::render(bool* open)
{
	if (ImGui::Begin("Style Editor", open))
	{
		if (ImGui::BeginPopupModal("Set Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please set the name of the current style:");
			ImGui::EndPopup();
		}

		if (!_context->isStyleFile("default"))
			ImGui::BeginDisabled();
		if (ImGui::Button("Reload"))
			_context->loadStyleFile("default");
		if (!_context->isStyleFile("default"))
			ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button("Save"))
			_context->saveStyleFile("default");

		ImGui::Separator();

		ImGui::PushItemWidth(std::max(150.0f, ImGui::GetWindowWidth() - 190.0f));
		if (ImGui::BeginTabBar("##style_editor_tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Sizes"))
			{
				ImGui::Text("Main");
				ImGui::SliderFloat2("WindowPadding", (float*)&_context->current_style->WindowPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("FramePadding", (float*)&_context->current_style->FramePadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("CellPadding", (float*)&_context->current_style->CellPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemSpacing", (float*)&_context->current_style->ItemSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemInnerSpacing", (float*)&_context->current_style->ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("TouchExtraPadding", (float*)&_context->current_style->TouchExtraPadding, 0.0f, 10.0f, "%.0f");
				ImGui::SliderFloat("IndentSpacing", &_context->current_style->IndentSpacing, 0.0f, 30.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarSize", &_context->current_style->ScrollbarSize, 1.0f, 20.0f, "%.0f");
				ImGui::SliderFloat("GrabMinSize", &_context->current_style->GrabMinSize, 1.0f, 20.0f, "%.0f");
				ImGui::Text("Borders");
				ImGui::SliderFloat("WindowBorderSize", &_context->current_style->WindowBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("ChildBorderSize", &_context->current_style->ChildBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("PopupBorderSize", &_context->current_style->PopupBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("FrameBorderSize", &_context->current_style->FrameBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("TabBorderSize", &_context->current_style->TabBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::Text("Rounding");
				ImGui::SliderFloat("WindowRounding", &_context->current_style->WindowRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ChildRounding", &_context->current_style->ChildRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("FrameRounding", &_context->current_style->FrameRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("PopupRounding", &_context->current_style->PopupRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarRounding", &_context->current_style->ScrollbarRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("GrabRounding", &_context->current_style->GrabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("LogSliderDeadzone", &_context->current_style->LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("TabRounding", &_context->current_style->TabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::Text("Alignment");
				ImGui::SliderFloat2("WindowTitleAlign", (float*)&_context->current_style->WindowTitleAlign, 0.0f, 1.0f, "%.2f");
				int window_menu_button_position = _context->current_style->WindowMenuButtonPosition + 1;
				if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
					_context->current_style->WindowMenuButtonPosition = window_menu_button_position - 1;
				ImGui::Combo("ColorButtonPosition", (int*)&_context->current_style->ColorButtonPosition, "Left\0Right\0");
				ImGui::SliderFloat2("ButtonTextAlign", (float*)&_context->current_style->ButtonTextAlign, 0.0f, 1.0f, "%.2f");
				ImGui::SliderFloat2("SelectableTextAlign", (float*)&_context->current_style->SelectableTextAlign, 0.0f, 1.0f, "%.2f");
				ImGui::Text("Safe Area Padding");
				ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&_context->current_style->DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Colors"))
			{
				_filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

				ImGui::BeginChild("##maincolors", ImVec2(0, 0), true,
						ImGuiWindowFlags_AlwaysVerticalScrollbar
						| ImGuiWindowFlags_NavFlattened);
				ImGui::PushItemWidth(-160);

				for (std::size_t i = 0; i < ImGuiCol_COUNT; ++i)
				{
					if (!_filter.PassFilter(ImGui::GetStyleColorName(i)))
						continue;

					ImGui::PushID(i);
					ImGui::ColorEdit4("##color", &_context->current_style->Colors[i].x,
							ImGuiColorEditFlags_AlphaPreviewHalf
							| ImGuiColorEditFlags_AlphaBar);
					ImGui::SameLine(0.0f, _context->current_style->ItemInnerSpacing.x);
					ImGui::TextUnformatted(ImGui::GetStyleColorName(i));
					ImGui::PopID();
				}

				ImGui::PopItemWidth();
				ImGui::EndChild();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Rendering"))
			{
				ImGui::Checkbox("Anti-aliased lines", &_context->current_style->AntiAliasedLines);
				ImGui::Checkbox("Anti-aliased lines use texture", &_context->current_style->AntiAliasedLinesUseTex);
				ImGui::Checkbox("Anti-aliased fill", &_context->current_style->AntiAliasedFill);

				ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
				ImGui::DragFloat("Curve Tessellation Tolerance", &_context->current_style->CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
				if (_context->current_style->CurveTessellationTol < 0.10f) _context->current_style->CurveTessellationTol = 0.10f;

				ImGui::DragFloat("Circle Tessellation Max Error", &_context->current_style->CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				if (ImGui::IsItemActive())
				{
					ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
					ImGui::BeginTooltip();
					ImGui::TextUnformatted("(R = radius, N = number of segments)");
					ImGui::Spacing();
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					const float min_widget_width = ImGui::CalcTextSize("N: MMM\nR: MMM").x;
					for (int n = 0; n < 8; n++)
					{
						const float RAD_MIN = 5.0f;
						const float RAD_MAX = 70.0f;
						const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

						ImGui::BeginGroup();

						ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

						const float canvas_width = std::max(min_widget_width, rad * 2.0f);
						const float offset_x     = std::floor(canvas_width * 0.5f);
						const float offset_y     = std::floor(RAD_MAX);

						const ImVec2 p1 = ImGui::GetCursorScreenPos();
						draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
						ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

						const ImVec2 p2 = ImGui::GetCursorScreenPos();
						draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
						ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

						ImGui::EndGroup();
						ImGui::SameLine();
					}
					ImGui::EndTooltip();
				}

				ImGui::DragFloat("Global Alpha", &_context->current_style->Alpha, 0.005f, 0.20f, 1.0f, "%.2f");
				ImGui::DragFloat("Disabled Alpha", &_context->current_style->DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f");

				ImGui::PopItemWidth();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::PopItemWidth();
	}
	ImGui::End();
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
