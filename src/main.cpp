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

#include "graphics/backend.h"
#include "graphics/imgui_tools.h"
#include "graphics/windows.h"
#include "settings.h"
#include "storage.h"



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
	// Setup backend classes
	FileLocationService rootFLS;
	graphics::ViewportRenderer viewportRender;
	storage::Library library("/tmp/test.library");

	// Setup windows
	graphics::LibraryWindow libraryWindow(&library, true);
	viewportRender.registerStaticWindow(&libraryWindow);
	graphics::EditorWindowTest editorWindow;
	viewportRender.registerStaticWindow(&editorWindow);
	graphics::MarkdownWindowTest markdownWindow;
	viewportRender.registerStaticWindow(&markdownWindow);

	// Setup the main system window
	graphics::SystemWindow::setErrorCallback(
		[] (int error, const char* description)
		{
			std::cerr << "GLFW Error " << +error << ": " << description << std::endl;
		}
	);
	graphics::SystemWindow window("Honmonogatari");

	// Setup ImGui-Renderer
	graphics::VulkanInstance::setErrorHandler(
		[] (VkResult err)
		{
			std::cerr << "[vulkan] Error: VkResult = " << err << std::endl;

			if (err < 0)
			{
				throw std::runtime_error("A vulkan error occurred");
			}
		}
	);
	graphics::VulkanInstance vulkan(window);

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
	}

	// Setup the styling context
	StyleContext styleContext(rootFLS.getConfigLocation("styles"));
	graphics::StyleEditorWindow styleEditorWindow(&styleContext);
	viewportRender.registerSettingsWindow(&styleEditorWindow);

	// Initialize the vulkan rendering
	vulkan.init(window);

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
		graphics::NewFrame();
		ImGui::DockSpaceOverViewport();

		// Use the viewport renderer to fill the main Dear ImGui frame
		viewportRender.renderMainMenuBar();
		viewportRender.renderWindows();

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
