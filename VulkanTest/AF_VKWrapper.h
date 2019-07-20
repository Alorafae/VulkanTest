
#pragma once

#include <Windows.h>
#pragma comment(linker, "/subsystem:console")
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include <fstream>

#define GLM_FORCE_RADIANS
#include "utils\glm\glm.hpp"
#include "utils\glm\gtc\matrix_transform.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#define U_ASSERT_ONLY

#include "vulkan.h"
#include "vk_platform.h"
//#include "vk_sdk_platform.h"
#include "glslang\Include\Common.h"
#include "glslang\Include\revision.h"
#include "glslang\MachineIndependent\localintermediate.h"
#include "glslang\MachineIndependent\SymbolTable.h"
#include "SPIRV\GlslangToSpv.h"

#define MAX_APP_NAME_LEN 80
#define FRAME_TIME 0.0069444f
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT
#define NUM_DESCRIPTOR_SETS 1
/* Number of viewports and number of scissors have to be the same */
/* at pipeline creation and in any call to set them dynamically   */
/* They also have to be the same as each other                    */
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS

/* Amount of time, in nanoseconds, to wait for a command buffer to complete */
#define FENCE_TIMEOUT 100000000

static const char *vShdTxt =
"#version 400\n"
"#extension GL_ARB_separate_shader_objects : enable\n"
"#extension GL_ARB_shading_language_420pack : enable\n"
"layout (std140, binding = 0) uniform bufferVals {\n"
"    mat4 mvp;\n"
"} myBufferVals;\n"
"layout (location = 0) in vec4 pos;\n"
"layout (location = 1) in vec4 inColor;\n"
"layout (location = 0) out vec4 outColor;\n"
"void main() {\n"
"   outColor = inColor;\n"
"   gl_Position = myBufferVals.mvp * pos;\n"
"}\n";

static const char *fShdTxt =
"#version 400\n"
"#extension GL_ARB_separate_shader_objects : enable\n"
"#extension GL_ARB_shading_language_420pack : enable\n"
"layout (location = 0) in vec4 color;\n"
"layout (location = 0) out vec4 outColor;\n"
"void main() {\n"
"   outColor = color;\n"
"}\n";

// vertex data for a cube
#include "cube_data.h"

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
  wchar_t name[MAX_APP_NAME_LEN]; // Name to put on the window/icon
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

// utility functions
bool memory_type_from_properties(DeviceInfo &info, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv);
bool GLSLtoSPV_Fix(const char *pshader, std::vector<unsigned int> &spirv, glslang::TShader &shader);



EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type);
void VK_Shader_Init_Resources(TBuiltInResource &Resources);

// startup functions
VkResult VK_Start_Sequence(DeviceInfo &info);




VkResult VK_Global_Ext_Props(layer_properties &layer_props); // done

VkResult VK_Global_Layer_Props(DeviceInfo &info); // done
VkResult VK_Instance_Ext_Names(DeviceInfo &info); // done
VkResult VK_Device_Ext_Names(DeviceInfo &info); // done
VkResult VK_Create_Instance(DeviceInfo &info); // done
VkResult VK_Enumerate_Device(DeviceInfo &info, uint32_t gpu_count = 1); // done
VkResult VK_Create_Window(DeviceInfo &info); // done
VkResult VK_Swapchain_Ext(DeviceInfo &info); // done
VkResult VK_Create_Device(DeviceInfo &info); // done

VkResult VK_Cmd_Pool(DeviceInfo &info);
VkResult VK_Cmd_Buffer(DeviceInfo &info);
VkResult VK_Exec_Cmd_Buffer(DeviceInfo &info);
VkResult VK_Device_Queue(DeviceInfo &info);
VkResult VK_Swapchain(DeviceInfo &info, VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
VkResult VK_Depth_Buffer(DeviceInfo &info); // done
VkResult VK_Uniform_Buffer(DeviceInfo &info); //done

VkResult VK_Descriptor_Pipeline_Layouts(DeviceInfo &info, bool use_texture, VkDescriptorSetLayoutCreateFlags descSetLayoutCreateFlags = 0);
VkResult VK_Renderpass(DeviceInfo &info, bool include_depth, bool clear = true, VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

// glslang
VkResult VK_Shaders(DeviceInfo &info, const char *vertShaderText, const char *fragShaderText);

VkResult VK_Framebuffers(DeviceInfo &info, bool IncludeDepth);

VkResult VK_VertexBuffer(DeviceInfo &info, const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool IsTextured);

VkResult VK_DescriptorPool(DeviceInfo &info, bool IsTextured);
VkResult VK_DescriptorSet(DeviceInfo &info, bool IsTextured);

VkResult VK_PipelineCache(DeviceInfo &info);
VkResult VK_Pipeline(DeviceInfo &info, VkBool32 include_depth, VkBool32 include_vi = true);



// end of startup functions

// render?
void VK_Viewports(DeviceInfo &info);
void VK_Scissors(DeviceInfo &info);
VkResult VK_RenderCube(DeviceInfo &info);

// end render

// shutdown

VkResult VK_Destroy_Pipeline(DeviceInfo &info);



VkResult VK_Shutdown_Sequence(DeviceInfo& info);

/*
vkDestroySemaphore(info.device, imageAcquiredSemaphore, NULL);
vkDestroyFence(info.device, drawFence, NULL);
destroy_pipeline(info);
destroy_pipeline_cache(info);
destroy_descriptor_pool(info);
destroy_vertex_buffer(info);
destroy_framebuffers(info);
destroy_shaders(info);
destroy_renderpass(info);
destroy_descriptor_and_pipeline_layouts(info);
destroy_uniform_buffer(info);
destroy_depth_buffer(info);
destroy_swap_chain(info);
destroy_command_buffer(info);
destroy_command_pool(info);
destroy_device(info);
destroy_window(info);
destroy_instance(info);
*/


