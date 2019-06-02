#pragma once

#include <Windows.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <stdio.h>
#include <vector>
#include "utils\glm\glm.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan.h"
#include "vk_platform.h"

#define MAX_APP_NAME_LEN 80

/*
* structure to track all objects related to a texture.
*/
struct texture_object {
  VkSampler sampler;

  VkImage image;
  VkImageLayout imageLayout;

  VkDeviceMemory mem;
  VkImageView view;
  int32_t tex_width, tex_height;
};

/*
* Keep each of our swap chain buffers' image, command buffer and view in one
* spot
*/
typedef struct _swap_chain_buffers {
  VkImage image;
  VkImageView view;
} swap_chain_buffer;

/*
* A layer can expose extensions, keep track of those
* extensions here.
*/
typedef struct {
  VkLayerProperties properties;
  std::vector<VkExtensionProperties> extensions;
} layer_properties;

struct DeviceInfo
{
  HINSTANCE connection;        // hInstance - Windows Instance
  char name[MAX_APP_NAME_LEN]; // Name to put on the window/icon
  HWND window;                 // hWnd - window handle

  VkSurfaceKHR surface;
  bool prepared;
  bool use_staging_buffer;
  bool save_images;

  std::vector<const char *> instance_layer_names;
  std::vector<const char *> instance_extension_names;
  std::vector<layer_properties> instance_layer_properties;
  std::vector<VkExtensionProperties> instance_extension_properties;
  VkInstance inst;

  std::vector<const char *> device_extension_names;
  std::vector<VkExtensionProperties> device_extension_properties;
  std::vector<VkPhysicalDevice> gpus;
  VkDevice device;
  VkQueue graphics_queue;
  VkQueue present_queue;
  uint32_t graphics_queue_family_index;
  uint32_t present_queue_family_index;
  VkPhysicalDeviceProperties gpu_props;
  std::vector<VkQueueFamilyProperties> queue_props;
  VkPhysicalDeviceMemoryProperties memory_properties;

  VkFramebuffer *framebuffers;
  int width, height;
  VkFormat format;

  uint32_t swapchainImageCount;
  VkSwapchainKHR swap_chain;
  std::vector<swap_chain_buffer> buffers;
  VkSemaphore imageAcquiredSemaphore;

  VkCommandPool cmd_pool;

  struct {
    VkFormat format;

    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
  } depth;

  std::vector<struct texture_object> textures;

  struct {
    VkBuffer buf;
    VkDeviceMemory mem;
    VkDescriptorBufferInfo buffer_info;
  } uniform_data;

  struct {
    VkDescriptorImageInfo image_info;
  } texture_data;
  VkDeviceMemory stagingMemory;
  VkImage stagingImage;

  struct {
    VkBuffer buf;
    VkDeviceMemory mem;
    VkDescriptorBufferInfo buffer_info;
  } vertex_buffer;
  VkVertexInputBindingDescription vi_binding;
  VkVertexInputAttributeDescription vi_attribs[2];

  glm::mat4 Projection;
  glm::mat4 View;
  glm::mat4 Model;
  glm::mat4 Clip;
  glm::mat4 MVP;

  VkCommandBuffer cmd; // Buffer for initialization commands
  VkPipelineLayout pipeline_layout;
  std::vector<VkDescriptorSetLayout> desc_layout;
  VkPipelineCache pipelineCache;
  VkRenderPass render_pass;
  VkPipeline pipeline;

  VkPipelineShaderStageCreateInfo shaderStages[2];

  VkDescriptorPool desc_pool;
  std::vector<VkDescriptorSet> desc_set;

  PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;
  PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;
  PFN_vkDebugReportMessageEXT dbgBreakCallback;
  std::vector<VkDebugReportCallbackEXT> debug_report_callbacks;

  uint32_t current_buffer;
  uint32_t queue_family_count;

  VkViewport viewport;
  VkRect2D scissor;
};

/*
init_instance(info, sample_title);
init_enumerate_device(info);
init_window_size(info, 500, 500);
init_connection(info);
init_window(info);
init_swapchain_extension(info);
init_device(info);
...
// VULKAN_KEY_START 

... code of interest

// VULKAN_KEY_END 
...
destroy_device(info);
destroy_window(info);
destroy_instance(info);

*/

/*
  Startup functions
*/

bool VK_Set_Extensions(DeviceInfo info);
bool VK_Vulkan_Init(void);
//bool VK_Vulkan_Init(DeviceInfo info);

bool VK_Init_Instance(DeviceInfo info, char* title);
//bool VK_Device_Enumerate(DeviceInfo info);
bool VK_Create_Queue(DeviceInfo info);
//bool VK_Create_Command(DeviceInfo info);
//bool VK_Window_Size(DeviceInfo info, u_int height, u_int width);
//bool VK_Connection(DeviceInfo info);
bool VK_Window(DeviceInfo info);
bool VK_Init_Swapchain(DeviceInfo& info);
bool VK_Init_Device(DeviceInfo info);


/*
  Closing functions
*/

bool VK_Vulkan_Shutdown(void);
bool VK_Destroy_Device(DeviceInfo info);
bool VK_Destroy_Window(DeviceInfo info);
bool VK_Destroy_Instance(DeviceInfo info);
