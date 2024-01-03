#ifndef GRAPHICS_BACKEND_H
#define GRAPHICS_BACKEND_H

#include <mutex>
#include <functional>
#include <string_view>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "implementation_vulkan.h"

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif


namespace graphics
{
	// forward declaration
	class VulkanInstance;

	/**
	 * @brief A wrapper for the glfw window pointer.
	 *
	 * This contains the basic glfw setup for the programm.
	 */
	class SystemWindow
	{
	public:
		friend VulkanInstance;
		using ErrorFunction = GLFWerrorfun;
		static inline void setErrorCallback(ErrorFunction function)
		{
			glfwSetErrorCallback(function);
		}

	private:
		static std::mutex globalOperationMutex;
		static short globalWindowCount;

	public:
		SystemWindow(std::string_view title = "");
		SystemWindow(SystemWindow&&) = delete;
		SystemWindow(const SystemWindow&) = delete;
		~SystemWindow();

		SystemWindow& operator=(SystemWindow&&) = delete;
		SystemWindow& operator=(const SystemWindow&) = delete;

		bool isActive();
	private:
		::GLFWwindow* glfwWindow;
	};



	/**
	 * @brief A wrapper for all of the vulkan functions and objects.
	 *
	 * This class is intended to be used with a glfw window and will call some
	 * glfw functions itself.
	 */
	class VulkanInstance
	{
	public:
		using ErrorFunction = std::function<void(VkResult)>;

		static void setErrorHandler(ErrorFunction function)
		{
			error_handler = function;
		}

		static void callErrorHandler(VkResult result)
		{
			if (!result) { return; }

			if (error_handler != nullptr)
			{
				error_handler(result);
			}
		}

	private:
		static ErrorFunction error_handler;

	public:
		VulkanInstance(SystemWindow& window);
		VulkanInstance(VulkanInstance&&) = delete;
		VulkanInstance(const VulkanInstance&) = delete;
		~VulkanInstance();

		VulkanInstance& operator=(VulkanInstance&&) = delete;
		VulkanInstance& operator=(const VulkanInstance&) = delete;

		// FIXME: find a way to change clear color at runtime

		void init(SystemWindow& window);
		void rebuildSwapchain(SystemWindow& window);
		void render();
		void presentFrame();

	private:
		ImGui_ImplVulkanH_Window mainData;

		VkAllocationCallbacks* g_Allocator;
		VkInstance g_Instance;
#ifdef IMGUI_VULKAN_DEBUG_REPORT
		VkDebugReportCallbackEXT g_DebugReport;
#endif
		VkPhysicalDevice g_PhysicalDevice;
		uint32_t g_QueueFamily;
		VkQueue g_Queue;
		VkDevice g_Device;
		VkDescriptorPool g_DescriptorPool;
		unsigned int g_MinImageCount;
		bool g_SwapChainRebuild;
		bool skipPresentFrame;
		VkPipelineCache g_PipelineCache;
	};


	/**
	 * @brief Generate a new frame.
	 *
	 * This function calls all needed glfw / vulkan and ImGui functions in order
	 * to create a new frame.
	 *
	 * This function removes the need for the backend glfw and vulkan functions
	 * from the main programm.
	 */
	void NewFrame();
} // namespace graphics

#endif // GRAPHICS_BACKEND_H
