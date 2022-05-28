// Dear ImGui: standalone example application for Glfw + Vulkan
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// Important note to the reader who wish to integrate imgui_impl_vulkan.cpp/.h in their own engine/app.
// - Common ImGui_ImplVulkan_XXX functions and structures are used to interface with imgui_impl_vulkan.cpp/.h.
//   You will use those if you want to use this rendering backend in your engine/app.
// - Helper ImGui_ImplVulkanH_XXX functions and structures are only used by this example (main.cpp) and by
//   the backend itself (imgui_impl_vulkan.cpp), but should PROBABLY NOT be used by your own engine/app code.
// Read comments in imgui_impl_vulkan.h.

#include <iostream>
#include <stdexcept>

#include "imgui_tools.h"
#include "imgui_markdown.h"
#include "TextEditor.h"
#include "graphics_backend.h"
#include "settings.h"
#include "windows.h"
#include "storage.h"

#ifdef DEBUG
	#define SHOW_FPS_COUNTER
#endif

/**
 * @brief Get the center point of a viewport
 *
 * @param viewport The viewport to get the center of
 * @return ImVec2 Return the center point
 */
inline ImVecN<2> getViewportCenter(ImGuiViewport* viewport)
{
	return viewport->Pos + (viewport->Size / 2.0f);
}

/**
 * @brief Main entry point of the programm.
 *
 * @return int The error code of the program. A zero indicates normal termination.
 */
int main(int, char**)
{
	// Setup glfw
	graphics_backend::GlfwWindow::setErrorCallback(
		[] (int error, const char* description)
		{
			std::cerr << "GLFW Error " << +error << ": " << description << std::endl;
		}
	);
	graphics_backend::GlfwWindow window("Honmonogatari");

	// Setup Vulkan
	graphics_backend::VulkanInstance::setErrorHandler(
		[] (VkResult err)
		{
			std::cerr << "[vulkan] Error: VkResult = " << err << std::endl;
			if (err < 0) throw std::runtime_error("A vulkan error occured");
		}
	);
	graphics_backend::VulkanInstance vulkan(window);

	{ // Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		// Enable Multi-Viewport / Platform Windows
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		// Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		// Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		// Disable auto settings
		io.IniFilename = nullptr;

		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark(); //or ImGui::StyleColorsClassic();
	}

	StyleContext styleContext;

	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.188f, 0.25f, 0.25f, 1.0f);
	}

	storage::Library library("/tmp/test.library");
	graphics::LibraryWindow libraryWindow(&library, true);

	// Initialize the vulkan rendering
	vulkan.init(window);

	// Dear ImGui Window Flags
#ifdef DEBUG
	bool show_demo_window = false;
	bool show_stack_tool_window = false;
#endif
	bool show_style_editor_window = false;
	bool show_metrics_window = false;
	bool show_about_window = false;

	// Our state
	bool show_another_window = false;
	bool show_editor_window = false;
	bool show_markdown_window = false;

	TextEditor editor;

	// Main loop
	while (window.isActive())
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags
		// to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data
		//     to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
		//     data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them
		// from your application based on those two flags.
		vulkan.rebuildSwapchain(window);

		// Start a new Dear ImGui frame
		graphics_backend::NewFrame();
		ImGui::DockSpaceOverViewport();

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
				ImGui::MenuItem("Editor Test", NULL, &show_editor_window);
				ImGui::MenuItem("Markdown Test", NULL, &show_markdown_window);
				libraryWindow.showWindowMenuEntry();

				ImGui::Separator();

#ifdef DEBUG
				ImGui::MenuItem("Demo Window", NULL, &show_demo_window);
				ImGui::MenuItem("Stack Tool", NULL, &show_stack_tool_window);
#endif
				ImGui::MenuItem("Style Editor", NULL, &show_style_editor_window);
				ImGui::MenuItem("UI Performance Metrics", NULL, &show_metrics_window);
				ImGui::MenuItem("About Dear ImGui", NULL, &show_about_window);
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

#ifdef DEBUG
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		if (show_stack_tool_window)
			ImGui::ShowStackToolWindow(&show_stack_tool_window);
#endif
		if (show_style_editor_window)
			styleContext.ShowStyleEditorWindow(&show_style_editor_window);

		if (show_style_editor_window)
		{
			if (ImGui::Begin("Default Style Editor", &show_style_editor_window))
			{
				ImGui::ShowStyleEditor(&ImGui::GetStyle());
			}
			ImGui::End();
		}

		if (show_metrics_window)
			ImGui::ShowMetricsWindow(&show_metrics_window);
		if (show_about_window)
			ImGui::ShowAboutWindow(&show_about_window);

		// 2. Show a simple window that we create ourselves. We use a
		{ // Begin/End pair to created a named window.
			static float f = 0.0f;
			static int counter = 0;

			// Create a window called "Hello, world!" and append into it.
			if (ImGui::Begin("Hello, world!"))
			{
				// Display some text (you can use a format strings too)
				ImGui::Text("This is some useful text.");
				// Edit bools storing our window open/close state
				ImGui::Checkbox("Demo Window", &show_demo_window);
				ImGui::Checkbox("Another Window", &show_another_window);
				ImGui::Checkbox("Show the Editor", &show_editor_window);

				// Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
				// Edit 3 floats representing a color
				ImGui::ColorEdit3("clear color", &ImGui::GetStyle().Colors[ImGuiCol_DockingEmptyBg].x);

				// Buttons return true when clicked (most widgets return true when edited/activated)
				if (ImGui::Button("Button"))
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
						1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			// Pass a pointer to our bool variable (the window will have a closing
			// button that will clear the bool when clicked)
			if (ImGui::Begin("Another Window", &show_another_window))
			{
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
			}
			ImGui::End();
		}

		if (show_editor_window)
		{
			ImGui::SetNextWindowSize(ImVecN<2>(0.0f, 500.0f), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Editor Test", &show_editor_window, ImGuiWindowFlags_MenuBar))
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

		if (show_markdown_window)
		{
			ImGui::SetNextWindowPos(getViewportCenter(ImGui::GetWindowViewport()),
					ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Markdown Test", &show_markdown_window))
			{
				ImGui::MarkdownConfig config = {
				};
				std::string data = "# H1 Header Test\n\nTest lorem ipsum from someone to lazy to google it but needs text anyway to be here to test shit.\n\n## H2 Header Test\n\nAnother test this time for *emphasis* and additionally **strong emphasis**.\n\n### H3 Header Test\n\nThis time there is a line below this text:\n\n___\n\n# List Test\n\n  * Entry one\n  * Entry two\n    * Sub entry\n  * Entry three\n";
				ImGui::Markdown(data.c_str(), data.length(), config);
			}
			ImGui::End();
		}

		libraryWindow.show();

		// Rendering
		ImGui::Render();
		vulkan.render();

		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		vulkan.presentFrame();
	}
	return 0;
}
