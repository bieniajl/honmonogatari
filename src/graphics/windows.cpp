#include "imgui_tools.h"
#include "windows.h"

void graphics::LibraryWindow::show()
{
	if (!active) return;

	ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Library Viewer", &active, ImGuiWindowFlags_MenuBar))
	{
		renderMenuBar();
		renderLibrary(library);
	}
	ImGui::End();
}

bool graphics::LibraryWindow::showWindowMenuEntry()
{
	return ImGui::MenuItem("Library Viewer", NULL, &active);
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
