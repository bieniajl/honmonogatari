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

#include "imgui.h"
#include "graphics_backend.h"

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
	vulkan.setClearColor(0.188f, 0.25f, 0.25f);

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

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform
	// windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Initialize the vulkan rendering
	vulkan.init(window);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;

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
		//ImGui::DockSpaceOverViewport();

		// 1. Show the big demo window
		// (Most of the sample code is in ImGui::ShowDemoWindow()! You can
		// browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a
		{ // Begin/End pair to created a named window.
			static float f = 0.0f;
			static int counter = 0;

			// Create a window called "Hello, world!" and append into it.
			ImGui::Begin("Hello, world!");

			// Display some text (you can use a format strings too)
			ImGui::Text("This is some useful text.");
			// Edit bools storing our window open/close state
			ImGui::Checkbox("Demo Window", &show_demo_window);
			ImGui::Checkbox("Another Window", &show_another_window);

			// Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			// Edit 3 floats representing a color
			ImGui::ColorEdit3("clear color", vulkan.getClearColorPointer());

			// Buttons return true when clicked (most widgets return true when edited/activated)
			if (ImGui::Button("Button"))
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
					1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			// Pass a pointer to our bool variable (the window will have a closing
			// button that will clear the bool when clicked)
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

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
