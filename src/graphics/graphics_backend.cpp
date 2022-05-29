#include <iostream>
#include "imgui_impl_glfw.h"
#include "graphics_backend.h"

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR))))


namespace graphics
{
	std::mutex GlfwWindow::globalOperationMutex;
	short GlfwWindow::globalWindowCount(0);

	GlfwWindow::GlfwWindow(const char* title) : glfwWindow(nullptr)
	{
		std::scoped_lock ​(GlfwWindow::globalOperationMutex);

		// Setup GLFW
		if (globalWindowCount++ == 0)
			[[unlikely]] if (!glfwInit())
				throw std::runtime_error("Can't initialize GLFW!");

		// Check for vulkan support
		[[unlikely]] if (!glfwVulkanSupported())
			throw std::runtime_error("GLFW: Vulkan Not Supported!");

		// set window hints
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// setup window in fullscreen on the primary monitor
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindow = glfwCreateWindow(
				mode->width, mode->height, title, monitor, NULL);
	}

	GlfwWindow::~GlfwWindow()
	{
		glfwDestroyWindow(glfwWindow);

		std::scoped_lock ​(GlfwWindow::globalOperationMutex);
		if (--globalWindowCount == 0)
			glfwTerminate();
	}

	bool GlfwWindow::isActive()
	{
		bool value = !glfwWindowShouldClose(glfwWindow);

		[[likely]] if (value)
			glfwPollEvents();

		return value;
	}

#ifdef IMGUI_VULKAN_DEBUG_REPORT
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location,
			int32_t messageCode, const char* pLayerPrefix, const char* pMessage,
			void* pUserData)
	{
		std::cerr << "[vulkan] Debug report from ObjectType: " << objectType << std::endl;
		std::cerr << "Message: " << pMessage << "\n" << std::endl;
		return VK_FALSE;
	}
#endif // IMGUI_VULKAN_DEBUG_REPORT

	VulkanInstance::ErrorFunction VulkanInstance::error_handler = nullptr;

	VulkanInstance::VulkanInstance(GlfwWindow& window) : g_Allocator(VK_NULL_HANDLE),
			g_Instance(VK_NULL_HANDLE), g_PhysicalDevice(VK_NULL_HANDLE),
			g_QueueFamily(static_cast<uint32_t>(-1)), g_Queue(VK_NULL_HANDLE),
			g_Device(VK_NULL_HANDLE), g_DescriptorPool(VK_NULL_HANDLE),
			g_MinImageCount(2), g_SwapChainRebuild(false), skipPresentFrame(false),
			g_PipelineCache(VK_NULL_HANDLE)
	{
		// Get Required Extrensions
		uint32_t extensions_count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

		{ // Create Vulkan Instance
#ifdef IMGUI_VULKAN_DEBUG_REPORT
			// Enabling validation layers
			const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
			std::vector<const char*> extensions_ext(extensions, extensions + extensions_count);
			extensions_ext.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			extensions_ext.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			// Create Vulkan Instance
			VkInstanceCreateInfo create_info = {
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.enabledLayerCount = 1,
				.ppEnabledLayerNames = layers,
				.enabledExtensionCount = static_cast<uint32_t>(extensions_ext.size()),
				.ppEnabledExtensionNames = extensions_ext.data()
			};
			callErrorHandler(vkCreateInstance(&create_info, g_Allocator, &g_Instance));

			// Setup the debug report callback
			VkDebugReportCallbackCreateInfoEXT debug_report_ci = {
				.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
				.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT
						| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
				.pfnCallback = debug_report,
				.pUserData = nullptr
			};

			// Get the function pointer (required for any extensions)
			auto cdrc = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
					g_Instance, "vkCreateDebugReportCallbackEXT");
			if (cdrc == nullptr)
				throw std::runtime_error("Can't find the vkCreateDebugReportCallbackEXT function");
			callErrorHandler(cdrc(g_Instance, &debug_report_ci, g_Allocator, &g_DebugReport));
#else
			// Create Vulkan Instance without any debug feature
			VkInstanceCreateInfo create_info = {
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.enabledExtensionCount = extensions_count,
				.ppEnabledExtensionNames = extensions
			};
			callErrorHandler(vkCreateInstance(&create_info, g_Allocator, &g_Instance));
#endif
		}

		{ // Select GPU
			uint32_t gpu_count;
			callErrorHandler(vkEnumeratePhysicalDevices(g_Instance, &gpu_count, NULL));
			if (gpu_count == 0)
				throw std::runtime_error("Can't find suitable gpu");

			std::vector<VkPhysicalDevice> gpus(gpu_count);
			callErrorHandler(vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.data()));

			// If a number >1 of GPUs got reported, find discrete GPU if present, or
			// use first one available. This covers most common cases
			// (multi-gpu/integrated+dedicated graphics). Handling more complicated
			// setups (multiple dedicated GPUs) is out of scope of this sample.
			g_PhysicalDevice = gpus.front();
			for (auto& gpu : gpus)
			{
				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(gpu, &properties);
				if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					g_PhysicalDevice = gpu;
					break;
				}
			}
		}

		{ // Select graphics queue family
			uint32_t count;
			vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, NULL);
			std::vector<VkQueueFamilyProperties> queues(count);
			vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues.data());

			for (uint32_t i = 0; i < count; ++i)
			{
				if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					g_QueueFamily = i;
					break;
				}
			}

			if (g_QueueFamily == static_cast<uint32_t>(-1))
				throw std::runtime_error("Can't find correct queue family");
		}

		{ // Create Logical Device (with 1 queue)
			unsigned int device_extension_count = 1;
			const char* device_extensions[] = { "VK_KHR_swapchain" };
			const float queue_priority[] = { 1.0f };
			VkDeviceQueueCreateInfo queue_info[1] = {{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = g_QueueFamily,
				.queueCount = 1,
				.pQueuePriorities = queue_priority
			}};
			VkDeviceCreateInfo create_info = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]),
				.pQueueCreateInfos = queue_info,
				.enabledExtensionCount = device_extension_count,
				.ppEnabledExtensionNames = device_extensions
			};
			callErrorHandler(vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device));
			vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
		}

		{ // Create Descriptor Pool
			VkDescriptorPoolSize pool_sizes[] = {
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};
			VkDescriptorPoolCreateInfo pool_info = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
				.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes),
				.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes)),
				.pPoolSizes = pool_sizes
			};
			callErrorHandler(vkCreateDescriptorPool(g_Device, &pool_info,
					g_Allocator, &g_DescriptorPool));
		}

		{ // Setup Vulkan Window
			// Create Window Surface
			callErrorHandler(glfwCreateWindowSurface(g_Instance,
					window.glfwWindow, g_Allocator, &mainData.Surface));

			// Check for WSI support
			VkBool32 res;
			vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily,
					mainData.Surface, &res);
			if (res != VK_TRUE)
				throw std::runtime_error("Error no WSI support on physical device 0");

			// Select Surface Format
			const VkFormat requestSurfaceImageFormat[] = {
				VK_FORMAT_B8G8R8A8_UNORM,
				VK_FORMAT_R8G8B8A8_UNORM,
				VK_FORMAT_B8G8R8_UNORM,
				VK_FORMAT_R8G8B8_UNORM
			};
			const VkColorSpaceKHR requestSurfaceColorSpace =
					VK_COLORSPACE_SRGB_NONLINEAR_KHR;
			mainData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
					g_PhysicalDevice, mainData.Surface, requestSurfaceImageFormat,
					static_cast<size_t>(IM_ARRAYSIZE(requestSurfaceImageFormat)),
					requestSurfaceColorSpace);

			// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
			VkPresentModeKHR present_modes[] = {
				VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR,
				VK_PRESENT_MODE_FIFO_KHR
			};
#else
			VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
			mainData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
					g_PhysicalDevice, mainData.Surface, &present_modes[0],
					IM_ARRAYSIZE(present_modes));
			//printf("[vulkan] Selected PresentMode = %d\n", mainData.PresentMode);

			// Create SwapChain, RenderPass, Framebuffer, etc.
			if (g_MinImageCount < 2)
				throw std::runtime_error("Not enought swapchain images");

			// Get FramebufferSize
			int width;
			int height;
			glfwGetFramebufferSize(window.glfwWindow, &width, &height);

			ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice,
					g_Device, &mainData, g_QueueFamily, g_Allocator, width,
					height, g_MinImageCount);
		}
	}

	VulkanInstance::~VulkanInstance()
	{
		callErrorHandler(vkDeviceWaitIdle(g_Device));
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &mainData, g_Allocator);
		vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
		// Remove the debug report callback
		auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)
				vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugReportCallbackEXT");
		vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport, g_Allocator);
#endif // IMGUI_VULKAN_DEBUG_REPORT

		vkDestroyDevice(g_Device, g_Allocator);
		vkDestroyInstance(g_Instance, g_Allocator);
	}

	void VulkanInstance::setClearColor(float red, float green, float blue)
	{
		mainData.ClearValue.color.float32[0] = red;
		mainData.ClearValue.color.float32[1] = green;
		mainData.ClearValue.color.float32[2] = blue;
		mainData.ClearValue.color.float32[3] = 1.0f;
	}

	void VulkanInstance::init(GlfwWindow& window)
	{
		{ // Setup Platform/Renderer backends
			ImGui_ImplGlfw_InitForVulkan(window.glfwWindow, true);
			ImGui_ImplVulkan_InitInfo init_info = {
				.Instance = g_Instance,
				.PhysicalDevice = g_PhysicalDevice,
				.Device = g_Device,
				.QueueFamily = g_QueueFamily,
				.Queue = g_Queue,
				.PipelineCache = g_PipelineCache,
				.DescriptorPool = g_DescriptorPool,
				.Subpass = 0,
				.MinImageCount = g_MinImageCount,
				.ImageCount = mainData.ImageCount,
				.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
				.Allocator = g_Allocator,
				.CheckVkResultFn = callErrorHandler
			};
			ImGui_ImplVulkan_Init(&init_info, mainData.RenderPass);
		}

		{ // Load Fonts
			// - If no fonts are loaded, dear imgui will use the default font.
			//     You can also load multiple fonts and use
			//     ImGui::PushFont()/PopFont() to select them.
			// - AddFontFromFileTTF() will return the ImFont* so you can store
			//     it if you need to select the font among multiple.
			// - If the file cannot be loaded, the function will return NULL.
			//     Please handle those errors in your application (e.g. use an
			//     assertion, or display an error and quit).
			// - The fonts will be rasterized at a given size (w/ oversampling)
			//     and stored into a texture when calling
			//     ImFontAtlas::Build()/GetTexDataAsXXXX(), which
			//     ImGui_ImplXXXX_NewFrame below will call.
			// - Read 'docs/FONTS.md' for more instructions and details.
			// - Remember that in C/C++ if you want to include a backslash \ in
			//     a string literal you need to write a double backslash \\ !
			//io.Fonts->AddFontDefault();
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
			//ImFont* font = io.Fonts->AddFontFromFileTTF(
			//		"c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL,
			//		io.Fonts->GetGlyphRangesJapanese());
			//IM_ASSERT(font != NULL);
		}

		{ // Upload Fonts
			// Use any command queue
			VkCommandPool command_pool =
					mainData.Frames[mainData.FrameIndex].CommandPool;
			VkCommandBuffer command_buffer =
					mainData.Frames[mainData.FrameIndex].CommandBuffer;

			callErrorHandler(vkResetCommandPool(g_Device, command_pool, 0));
			VkCommandBufferBeginInfo begin_info = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
			};
			callErrorHandler(vkBeginCommandBuffer(command_buffer, &begin_info));

			ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

			VkSubmitInfo end_info = {
				.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.commandBufferCount = 1,
				.pCommandBuffers = &command_buffer
			};
			callErrorHandler(vkEndCommandBuffer(command_buffer));
			callErrorHandler(vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE));
			callErrorHandler(vkDeviceWaitIdle(g_Device));

			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}

	void VulkanInstance::rebuildSwapchain(GlfwWindow& window)
	{
		// Resize swap chain?
		if (!g_SwapChainRebuild) return;

		int width;
		int height;
		glfwGetFramebufferSize(window.glfwWindow, &width, &height);
		if (width > 0 && height > 0)
		{
			ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
			ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice,
					g_Device, &mainData, g_QueueFamily, g_Allocator,
					width, height, g_MinImageCount);
			mainData.FrameIndex = 0;
			g_SwapChainRebuild = false;
		}
	}

	void VulkanInstance::render()
	{
		ImDrawData* draw_data = ImGui::GetDrawData();

		if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
		{
			skipPresentFrame = true;
			return;
		}
		skipPresentFrame = false;

		VkSemaphore image_acquired_semaphore =
				mainData.FrameSemaphores[mainData.SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore =
				mainData.FrameSemaphores[mainData.SemaphoreIndex].RenderCompleteSemaphore;
		VkResult err = vkAcquireNextImageKHR(g_Device, mainData.Swapchain, UINT64_MAX,
				image_acquired_semaphore, VK_NULL_HANDLE, &mainData.FrameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			g_SwapChainRebuild = true;
			return;
		}
		callErrorHandler(err);

		ImGui_ImplVulkanH_Frame* fd = &mainData.Frames[mainData.FrameIndex];
		{
			// wait indefinitely instead of periodically checking
			callErrorHandler(vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX));
			callErrorHandler(vkResetFences(g_Device, 1, &fd->Fence));
		}
		{
			callErrorHandler(vkResetCommandPool(g_Device, fd->CommandPool, 0));
			VkCommandBufferBeginInfo info = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
			};
			callErrorHandler(vkBeginCommandBuffer(fd->CommandBuffer, &info));
		}
		{
			VkRenderPassBeginInfo info = {
				.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				.renderPass = mainData.RenderPass,
				.framebuffer = fd->Framebuffer,
				.renderArea = {
					.extent = {
						.width = static_cast<uint32_t>(mainData.Width),
						.height = static_cast<uint32_t>(mainData.Height)
					}
				},
				.clearValueCount = 1,
				.pClearValues = &mainData.ClearValue
			};
			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {
				.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = &image_acquired_semaphore,
				.pWaitDstStageMask = &wait_stage,
				.commandBufferCount = 1,
				.pCommandBuffers = &fd->CommandBuffer,
				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &render_complete_semaphore
			};
			callErrorHandler(vkEndCommandBuffer(fd->CommandBuffer));
			callErrorHandler(vkQueueSubmit(g_Queue, 1, &info, fd->Fence));
		}
	}

	void VulkanInstance::presentFrame()
	{
		if (skipPresentFrame || g_SwapChainRebuild) return;

		VkSemaphore render_complete_semaphore =
				mainData.FrameSemaphores[mainData.SemaphoreIndex].RenderCompleteSemaphore;
		VkPresentInfoKHR info = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &render_complete_semaphore,
			.swapchainCount = 1,
			.pSwapchains = &mainData.Swapchain,
			.pImageIndices = &mainData.FrameIndex
		};
		VkResult err = vkQueuePresentKHR(g_Queue, &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			g_SwapChainRebuild = true;
			return;
		}
		callErrorHandler(err);
		// Now we can use the next set of semaphores
		mainData.SemaphoreIndex = (mainData.SemaphoreIndex + 1) % mainData.ImageCount;
	}

	void NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
} // namespace graphics
