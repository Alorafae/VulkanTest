
#include "AF_VKWrapper.h"

static DeviceInfo g_VK_Engine_AF;

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  //struct sample_info *info = reinterpret_cast<struct sample_info *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

  switch (uMsg) {
  case WM_CLOSE:
    PostQuitMessage(0);
    break;
  case WM_PAINT:
    //run(info);
    return 0;
  default:
    break;
  }
  return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

bool memory_type_from_properties(DeviceInfo & info, uint32_t typeBits, VkFlags requirements_mask, uint32_t * typeIndex)
{
  // Search memtypes to find first index with those properties
  for (uint32_t i = 0; i < info.memory_properties.memoryTypeCount; i++) {
    if ((typeBits & 1) == 1) {
      // Type is available, does it match user properties?
      if ((info.memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
        *typeIndex = i;
        return true;
      }
    }
    typeBits >>= 1;
  }
  // No memory types matched, return failure
  return false;
}

bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv)
{
  EShLanguage stage = FindLanguage(shader_type);
  glslang::TShader shader(stage);
  glslang::TProgram program;
  const char *shaderStrings[1];
  TBuiltInResource Resources;
  VK_Shader_Init_Resources(Resources);

  // Enable SPIR-V and Vulkan rules when parsing GLSL
  EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

  shaderStrings[0] = pshader;
  shader.setStrings(shaderStrings, 1);
  
  if (!shader.parse(&Resources, 100, false, messages))
  {
    puts(shader.getInfoLog());
    puts(shader.getInfoDebugLog());
    return false;  // something didn't work
  }
  //glslang::TProgram program;
  program.addShader(&shader);

  //
  // Program-level processing...
  //

  if (!program.link(messages))
  {
    puts(shader.getInfoLog());
    puts(shader.getInfoDebugLog());
    fflush(stdout);
    return false;
  }
  
  //glslang::TIntermediate *glti = program.getIntermediate(stage);

  glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
  //glslang::GlslangToSpv(*glti, spirv);
}

bool GLSLtoSPV_Fix(const char * pshader, std::vector<unsigned int>& spirv, glslang::TShader &shader)
{
  const char *shaderStrings[1];
  TBuiltInResource Resources;
  VK_Shader_Init_Resources(Resources);

  // Enable SPIR-V and Vulkan rules when parsing GLSL
  EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

  shaderStrings[0] = pshader;
  shader.setStrings(shaderStrings, 1);


  if (!shader.parse(&Resources, 100, false, messages))
  {
    puts(shader.getInfoLog());
    puts(shader.getInfoDebugLog());
    return false;  // something didn't work
  }

  return true;
}

void VK_Shader_Init_Resources(TBuiltInResource &Resources)
{
  Resources.maxLights = 32;
  Resources.maxClipPlanes = 6;
  Resources.maxTextureUnits = 32;
  Resources.maxTextureCoords = 32;
  Resources.maxVertexAttribs = 64;
  Resources.maxVertexUniformComponents = 4096;
  Resources.maxVaryingFloats = 64;
  Resources.maxVertexTextureImageUnits = 32;
  Resources.maxCombinedTextureImageUnits = 80;
  Resources.maxTextureImageUnits = 32;
  Resources.maxFragmentUniformComponents = 4096;
  Resources.maxDrawBuffers = 32;
  Resources.maxVertexUniformVectors = 128;
  Resources.maxVaryingVectors = 8;
  Resources.maxFragmentUniformVectors = 16;
  Resources.maxVertexOutputVectors = 16;
  Resources.maxFragmentInputVectors = 15;
  Resources.minProgramTexelOffset = -8;
  Resources.maxProgramTexelOffset = 7;
  Resources.maxClipDistances = 8;
  Resources.maxComputeWorkGroupCountX = 65535;
  Resources.maxComputeWorkGroupCountY = 65535;
  Resources.maxComputeWorkGroupCountZ = 65535;
  Resources.maxComputeWorkGroupSizeX = 1024;
  Resources.maxComputeWorkGroupSizeY = 1024;
  Resources.maxComputeWorkGroupSizeZ = 64;
  Resources.maxComputeUniformComponents = 1024;
  Resources.maxComputeTextureImageUnits = 16;
  Resources.maxComputeImageUniforms = 8;
  Resources.maxComputeAtomicCounters = 8;
  Resources.maxComputeAtomicCounterBuffers = 1;
  Resources.maxVaryingComponents = 60;
  Resources.maxVertexOutputComponents = 64;
  Resources.maxGeometryInputComponents = 64;
  Resources.maxGeometryOutputComponents = 128;
  Resources.maxFragmentInputComponents = 128;
  Resources.maxImageUnits = 8;
  Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
  Resources.maxCombinedShaderOutputResources = 8;
  Resources.maxImageSamples = 0;
  Resources.maxVertexImageUniforms = 0;
  Resources.maxTessControlImageUniforms = 0;
  Resources.maxTessEvaluationImageUniforms = 0;
  Resources.maxGeometryImageUniforms = 0;
  Resources.maxFragmentImageUniforms = 8;
  Resources.maxCombinedImageUniforms = 8;
  Resources.maxGeometryTextureImageUnits = 16;
  Resources.maxGeometryOutputVertices = 256;
  Resources.maxGeometryTotalOutputComponents = 1024;
  Resources.maxGeometryUniformComponents = 1024;
  Resources.maxGeometryVaryingComponents = 64;
  Resources.maxTessControlInputComponents = 128;
  Resources.maxTessControlOutputComponents = 128;
  Resources.maxTessControlTextureImageUnits = 16;
  Resources.maxTessControlUniformComponents = 1024;
  Resources.maxTessControlTotalOutputComponents = 4096;
  Resources.maxTessEvaluationInputComponents = 128;
  Resources.maxTessEvaluationOutputComponents = 128;
  Resources.maxTessEvaluationTextureImageUnits = 16;
  Resources.maxTessEvaluationUniformComponents = 1024;
  Resources.maxTessPatchComponents = 120;
  Resources.maxPatchVertices = 32;
  Resources.maxTessGenLevel = 64;
  Resources.maxViewports = 16;
  Resources.maxVertexAtomicCounters = 0;
  Resources.maxTessControlAtomicCounters = 0;
  Resources.maxTessEvaluationAtomicCounters = 0;
  Resources.maxGeometryAtomicCounters = 0;
  Resources.maxFragmentAtomicCounters = 8;
  Resources.maxCombinedAtomicCounters = 8;
  Resources.maxAtomicCounterBindings = 1;
  Resources.maxVertexAtomicCounterBuffers = 0;
  Resources.maxTessControlAtomicCounterBuffers = 0;
  Resources.maxTessEvaluationAtomicCounterBuffers = 0;
  Resources.maxGeometryAtomicCounterBuffers = 0;
  Resources.maxFragmentAtomicCounterBuffers = 1;
  Resources.maxCombinedAtomicCounterBuffers = 1;
  Resources.maxAtomicCounterBufferSize = 16384;
  Resources.maxTransformFeedbackBuffers = 4;
  Resources.maxTransformFeedbackInterleavedComponents = 64;
  Resources.maxCullDistances = 8;
  Resources.maxCombinedClipAndCullDistances = 8;
  Resources.maxSamples = 4;
  Resources.limits.nonInductiveForLoops = 1;
  Resources.limits.whileLoops = 1;
  Resources.limits.doWhileLoops = 1;
  Resources.limits.generalUniformIndexing = 1;
  Resources.limits.generalAttributeMatrixVectorIndexing = 1;
  Resources.limits.generalVaryingIndexing = 1;
  Resources.limits.generalSamplerIndexing = 1;
  Resources.limits.generalVariableIndexing = 1;
  Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type)
{
  switch (shader_type)
  {
  case VK_SHADER_STAGE_VERTEX_BIT:
    return EShLangVertex;

  case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
    return EShLangTessControl;

  case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
    return EShLangTessEvaluation;

  case VK_SHADER_STAGE_GEOMETRY_BIT:
    return EShLangGeometry;

  case VK_SHADER_STAGE_FRAGMENT_BIT:
    return EShLangFragment;

  case VK_SHADER_STAGE_COMPUTE_BIT:
    return EShLangCompute;

  default:
    return EShLangVertex;
  }
}

VkResult VK_Start_Sequence(DeviceInfo& info)
{
  info.width = 500;
  info.height = 500;

  VK_Global_Layer_Props(info);
  VK_Instance_Ext_Names(info);
  VK_Device_Ext_Names(info);
  VK_Create_Instance(info);
  VK_Enumerate_Device(info);
  VK_Create_Window(info);
  VK_Swapchain_Ext(info);
  VK_Create_Device(info);

  VK_Cmd_Pool(info);
  VK_Cmd_Buffer(info);
  VK_Exec_Cmd_Buffer(info);
  VK_Device_Queue(info);
  VK_Swapchain(info);

  VK_Depth_Buffer(info);
  VK_Uniform_Buffer(info);
  VK_Descriptor_Pipeline_Layouts(info, false);
  VK_Renderpass(info, true);

  VK_Shaders(info, vShdTxt, fShdTxt);
  //VK_Shaders_Fix(info, vShdTxt, fShdTxt);
  /*
  std::vector<char> vertexShaderCode;
  VK_Shaders_From_SPIRV(info, "vert.spv", vertexShaderCode);
  std::vector<char> fragShaderCode;
  VK_Shaders_From_SPIRV(info, "frag.spv", fragShaderCode);
  */

  VK_Framebuffers(info, true);

  VK_VertexBuffer(info, g_vb_solid_face_colors_Data,  sizeof(g_vb_solid_face_colors_Data),  sizeof(g_vb_solid_face_colors_Data[0]), false);

  VK_DescriptorPool(info, false);
  VK_DescriptorSet(info, false);

  VK_PipelineCache(info);
  VK_Pipeline(info,  true);

  // COOL ready to start rendering!!!

  return VK_SUCCESS;
}

VkResult VK_Global_Ext_Props(layer_properties &layer_props)
{
  VkExtensionProperties *instance_extensions;
  uint32_t instance_extension_count;
  VkResult res;
  char *layer_name = NULL;

  layer_name = layer_props.properties.layerName;

  do {
    res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, NULL);
    if (res) return res;

    if (instance_extension_count == 0) {
      return VK_SUCCESS;
    }

    layer_props.extensions.resize(instance_extension_count);
    instance_extensions = layer_props.extensions.data();
    res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, instance_extensions);
  } while (res == VK_INCOMPLETE);

  return res;
}

VkResult VK_Global_Layer_Props(DeviceInfo &info)
{
  uint32_t instance_layer_count;
  VkLayerProperties *vk_props = NULL;
  VkResult res;

  /*
  * It's possible, though very rare, that the number of
  * instance layers could change. For example, installing something
  * could include new layers that the loader would pick up
  * between the initial query for the count and the
  * request for VkLayerProperties. The loader indicates that
  * by returning a VK_INCOMPLETE status and will update the
  * the count parameter.
  * The count parameter will be updated with the number of
  * entries loaded into the data pointer - in case the number
  * of layers went down or is smaller than the size given.
  */
  do {
    res = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
    if (res)
      return res;

    if (instance_layer_count == 0)
    {
      return VK_SUCCESS;
    }

    vk_props = (VkLayerProperties *)realloc(vk_props, instance_layer_count * sizeof(VkLayerProperties));

    res = vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props);
  } while (res == VK_INCOMPLETE);

  /*
  * Now gather the extension list for each instance layer.
  */
  for (uint32_t i = 0; i < instance_layer_count; i++)
  {
    layer_properties layer_props;
    layer_props.properties = vk_props[i];
    res = VK_Global_Ext_Props(layer_props);

    if (res)
      return res;

    info.instance_layer_properties.push_back(layer_props);
  }
  free(vk_props);

  return res;
}

VkResult VK_Instance_Ext_Names(DeviceInfo &info)
{
  info.instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  info.instance_extension_names.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

  return VK_SUCCESS;
}

VkResult VK_Device_Ext_Names(DeviceInfo &info)
{
  info.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  return VK_SUCCESS;
}

VkResult VK_Create_Instance(DeviceInfo &info)
{
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = NULL;
  app_info.pApplicationName = "Alora's Vulkan";
  app_info.applicationVersion = 1;
  app_info.pEngineName = "Alora's Vulkan";
  app_info.engineVersion = 1;
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo inst_info = {};
  inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  inst_info.pNext = NULL;
  inst_info.flags = 0;
  inst_info.pApplicationInfo = &app_info;
  inst_info.enabledLayerCount = info.instance_layer_names.size();
  inst_info.ppEnabledLayerNames = info.instance_layer_names.size() ? info.instance_layer_names.data() : NULL; // change for readability
  inst_info.enabledExtensionCount = info.instance_extension_names.size();
  inst_info.ppEnabledExtensionNames = info.instance_extension_names.data();

  VkResult res = vkCreateInstance(&inst_info, NULL, &info.inst);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_Enumerate_Device(DeviceInfo &info, uint32_t gpu_count)
{
  uint32_t const U_ASSERT_ONLY req_count = gpu_count;
  VkResult res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, NULL);
  assert(gpu_count);
  info.gpus.resize(gpu_count);

  res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, info.gpus.data());
  assert(!res && gpu_count >= req_count);

  vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_family_count, NULL);
  assert(info.queue_family_count >= 1);

  info.queue_props.resize(info.queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_family_count, info.queue_props.data());
  assert(info.queue_family_count >= 1);

  /* This is as good a place as any to do this */
  vkGetPhysicalDeviceMemoryProperties(info.gpus[0], &info.memory_properties);
  vkGetPhysicalDeviceProperties(info.gpus[0], &info.gpu_props);

  return res;
}

VkResult VK_Create_Window(DeviceInfo &info)
{
  WNDCLASSEX win_class;
  assert(info.width > 0);
  assert(info.height > 0);

  info.connection = GetModuleHandle(NULL);
  sprintf(info.name, "Sample");

  // Initialize the window class structure:
  win_class.cbSize = sizeof(WNDCLASSEX);
  win_class.style = CS_HREDRAW | CS_VREDRAW;
  win_class.lpfnWndProc = WndProc;
  win_class.cbClsExtra = 0;
  win_class.cbWndExtra = 0;
  win_class.hInstance = info.connection;  // hInstance
  win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  win_class.lpszMenuName = NULL;
  win_class.lpszClassName = (LPCWSTR)info.name;
  win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
  // Register window class:
  if (!RegisterClassEx(&win_class))
  {
    // It didn't work, so try to give a useful error:
    printf("Unexpected error trying to start the application!\n");
    fflush(stdout);
    exit(1);
  }
  // Create window with the registered class:
  RECT wr = { 0, 0, info.width, info.height };
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
  info.window = CreateWindowEx(0,
    (LPCWSTR)info.name,             // class name
    (LPCWSTR)info.name,             // app name
    WS_OVERLAPPEDWINDOW |  // window style
    WS_VISIBLE | WS_SYSMENU,
    100, 100,            // x/y coords
    wr.right - wr.left,  // width
    wr.bottom - wr.top,  // height
    NULL,                // handle to parent
    NULL,                // handle to menu
    info.connection,     // hInstance
    NULL);               // no extra parameters
  if (!info.window)
  {
    // It didn't work, so try to give a useful error:
    printf("Cannot create a window in which to draw!\n");
    fflush(stdout);
    exit(1);
  }
  SetWindowLongPtr(info.window, GWLP_USERDATA, (LONG_PTR)&info);

  return VK_SUCCESS;
}

VkResult VK_Swapchain_Ext(DeviceInfo &info)
{
  /* DEPENDS on init_connection() and init_window() */

  VkResult U_ASSERT_ONLY res;

  // Construct the surface description:

  VkWin32SurfaceCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  createInfo.pNext = NULL;
  createInfo.hinstance = info.connection;
  createInfo.hwnd = info.window;
  res = vkCreateWin32SurfaceKHR(info.inst, &createInfo, NULL, &info.surface);

  assert(res == VK_SUCCESS);

  // Iterate over each queue to learn whether it supports presenting:
  VkBool32 *pSupportsPresent = (VkBool32 *)malloc(info.queue_family_count * sizeof(VkBool32));
  for (uint32_t i = 0; i < info.queue_family_count; i++)
  {
    vkGetPhysicalDeviceSurfaceSupportKHR(info.gpus[0], i, info.surface, &pSupportsPresent[i]);
  }

  // Search for a graphics and a present queue in the array of queue
  // families, try to find one that supports both
  info.graphics_queue_family_index = UINT32_MAX;
  info.present_queue_family_index = UINT32_MAX;
  for (uint32_t i = 0; i < info.queue_family_count; ++i)
  {
    if ((info.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
    {
      if (info.graphics_queue_family_index == UINT32_MAX) info.graphics_queue_family_index = i;

      if (pSupportsPresent[i] == VK_TRUE)
      {
        info.graphics_queue_family_index = i;
        info.present_queue_family_index = i;
        break;
      }
    }
  }

  if (info.present_queue_family_index == UINT32_MAX)
  {
    // If didn't find a queue that supports both graphics and present, then
    // find a separate present queue.
    for (size_t i = 0; i < info.queue_family_count; ++i)
      if (pSupportsPresent[i] == VK_TRUE)
      {
        info.present_queue_family_index = i;
        break;
      }
  }
  free(pSupportsPresent);

  // Generate error if could not find queues that support graphics
  // and present
  if (info.graphics_queue_family_index == UINT32_MAX || info.present_queue_family_index == UINT32_MAX)
  {
    printf("Could not find a queues for both graphics and present");
    exit(-1);
  }

  // Get the list of VkFormats that are supported:
  uint32_t formatCount;
  res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0], info.surface, &formatCount, NULL);
  assert(res == VK_SUCCESS);
  VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
  res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0], info.surface, &formatCount, surfFormats);
  assert(res == VK_SUCCESS);
  // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
  // the surface has no preferred format.  Otherwise, at least one
  // supported format will be returned.
  if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
  {
    info.format = VK_FORMAT_B8G8R8A8_UNORM;
  }
  else
  {
    assert(formatCount >= 1);
    info.format = surfFormats[0].format;
  }
  free(surfFormats);

  return res;
}

VkResult VK_Create_Device(DeviceInfo &info)
{
  VkResult res;
  VkDeviceQueueCreateInfo queue_info = {};

  float queue_priorities[1] = { 0.0 };
  queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info.pNext = NULL;
  queue_info.queueCount = 1;
  queue_info.pQueuePriorities = queue_priorities;
  queue_info.queueFamilyIndex = info.graphics_queue_family_index;

  VkDeviceCreateInfo device_info = {};
  device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info.pNext = NULL;
  device_info.queueCreateInfoCount = 1;
  device_info.pQueueCreateInfos = &queue_info;
  device_info.enabledExtensionCount = info.device_extension_names.size();
  device_info.ppEnabledExtensionNames = device_info.enabledExtensionCount ? info.device_extension_names.data() : NULL;
  device_info.pEnabledFeatures = NULL;

  res = vkCreateDevice(info.gpus[0], &device_info, NULL, &info.device);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_Cmd_Pool(DeviceInfo &info)
{
  /* DEPENDS on init_swapchain_extension() */
  VkResult U_ASSERT_ONLY res;

  VkCommandPoolCreateInfo cmd_pool_info = {};
  cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmd_pool_info.pNext = NULL;
  cmd_pool_info.queueFamilyIndex = info.graphics_queue_family_index;
  cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  res = vkCreateCommandPool(info.device, &cmd_pool_info, NULL, &info.cmd_pool);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_Cmd_Buffer(DeviceInfo &info)
{
  /* DEPENDS on init_swapchain_extension() and init_command_pool() */
  VkResult U_ASSERT_ONLY res;

  VkCommandBufferAllocateInfo cmd = {};
  cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmd.pNext = NULL;
  cmd.commandPool = info.cmd_pool;
  cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmd.commandBufferCount = 1;

  res = vkAllocateCommandBuffers(info.device, &cmd, &info.cmd);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_Exec_Cmd_Buffer(DeviceInfo &info)
{
  /* DEPENDS on init_command_buffer() */
  VkResult U_ASSERT_ONLY res;

  VkCommandBufferBeginInfo cmd_buf_info = {};
  cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  cmd_buf_info.pNext = NULL;
  cmd_buf_info.flags = 0;
  cmd_buf_info.pInheritanceInfo = NULL;

  res = vkBeginCommandBuffer(info.cmd, &cmd_buf_info);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_Device_Queue(DeviceInfo &info)
{
  /* DEPENDS on init_swapchain_extension() */

  vkGetDeviceQueue(info.device, info.graphics_queue_family_index, 0, &info.graphics_queue);
  if (info.graphics_queue_family_index == info.present_queue_family_index)
  {
    info.present_queue = info.graphics_queue;
  }
  else
  {
    vkGetDeviceQueue(info.device, info.present_queue_family_index, 0, &info.present_queue);
  }

  return VK_SUCCESS;
}

VkResult VK_Swapchain(DeviceInfo &info, VkImageUsageFlags usageFlags)
{
  /* DEPENDS on info.cmd and info.queue initialized */

  VkResult U_ASSERT_ONLY res;
  VkSurfaceCapabilitiesKHR surfCapabilities;

  res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info.gpus[0], info.surface, &surfCapabilities);
  assert(res == VK_SUCCESS);

  uint32_t presentModeCount;
  res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0], info.surface, &presentModeCount, NULL);
  assert(res == VK_SUCCESS);
  VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
  assert(presentModes);
  res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0], info.surface, &presentModeCount, presentModes);
  assert(res == VK_SUCCESS);

  VkExtent2D swapchainExtent;
  // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
  if (surfCapabilities.currentExtent.width == 0xFFFFFFFF)
  {
    // If the surface size is undefined, the size is set to
    // the size of the images requested.
    swapchainExtent.width = info.width;
    swapchainExtent.height = info.height;
    if (swapchainExtent.width < surfCapabilities.minImageExtent.width)
    {
      swapchainExtent.width = surfCapabilities.minImageExtent.width;
    }
    else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width)
    {
      swapchainExtent.width = surfCapabilities.maxImageExtent.width;
    }

    if (swapchainExtent.height < surfCapabilities.minImageExtent.height)
    {
      swapchainExtent.height = surfCapabilities.minImageExtent.height;
    }
    else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height)
    {
      swapchainExtent.height = surfCapabilities.maxImageExtent.height;
    }
  }
  else
  {
    // If the surface size is defined, the swap chain size must match
    swapchainExtent = surfCapabilities.currentExtent;
  }

  // The FIFO present mode is guaranteed by the spec to be supported
  // Also note that current Android driver only supports FIFO
  VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

  // Determine the number of VkImage's to use in the swap chain.
  // We need to acquire only 1 presentable image at at time.
  // Asking for minImageCount images ensures that we can acquire
  // 1 presentable image as long as we present it before attempting
  // to acquire another.
  uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount;

  VkSurfaceTransformFlagBitsKHR preTransform;
  if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
  {
    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  }
  else
  {
    preTransform = surfCapabilities.currentTransform;
  }

  // Find a supported composite alpha mode - one of these is guaranteed to be set
  VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
  };

  for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++)
  {
    if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i])
    {
      compositeAlpha = compositeAlphaFlags[i];
      break;
    }
  }

  VkSwapchainCreateInfoKHR swapchain_ci = {};
  swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_ci.pNext = NULL;
  swapchain_ci.surface = info.surface;
  swapchain_ci.minImageCount = desiredNumberOfSwapChainImages;
  swapchain_ci.imageFormat = info.format;
  swapchain_ci.imageExtent.width = swapchainExtent.width;
  swapchain_ci.imageExtent.height = swapchainExtent.height;
  swapchain_ci.preTransform = preTransform;
  swapchain_ci.compositeAlpha = compositeAlpha;
  swapchain_ci.imageArrayLayers = 1;
  swapchain_ci.presentMode = swapchainPresentMode;
  swapchain_ci.oldSwapchain = VK_NULL_HANDLE;

  swapchain_ci.clipped = true;

  swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  swapchain_ci.imageUsage = usageFlags;
  swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_ci.queueFamilyIndexCount = 0;
  swapchain_ci.pQueueFamilyIndices = NULL;
  uint32_t queueFamilyIndices[2] = { (uint32_t)info.graphics_queue_family_index, (uint32_t)info.present_queue_family_index };
  if (info.graphics_queue_family_index != info.present_queue_family_index)
  {
    // If the graphics and present queues are from different queue families,
    // we either have to explicitly transfer ownership of images between the
    // queues, or we have to create the swapchain with imageSharingMode
    // as VK_SHARING_MODE_CONCURRENT
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchain_ci.queueFamilyIndexCount = 2;
    swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;
  }

  res = vkCreateSwapchainKHR(info.device, &swapchain_ci, NULL, &info.swap_chain);
  assert(res == VK_SUCCESS);

  res = vkGetSwapchainImagesKHR(info.device, info.swap_chain, &info.swapchainImageCount, NULL);
  assert(res == VK_SUCCESS);

  VkImage *swapchainImages = (VkImage *)malloc(info.swapchainImageCount * sizeof(VkImage));
  assert(swapchainImages);
  res = vkGetSwapchainImagesKHR(info.device, info.swap_chain, &info.swapchainImageCount, swapchainImages);
  assert(res == VK_SUCCESS);

  for (uint32_t i = 0; i < info.swapchainImageCount; i++)
  {
    swap_chain_buffer sc_buffer;

    VkImageViewCreateInfo color_image_view = {};
    color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    color_image_view.pNext = NULL;
    color_image_view.format = info.format;
    color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
    color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
    color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
    color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
    color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    color_image_view.subresourceRange.baseMipLevel = 0;
    color_image_view.subresourceRange.levelCount = 1;
    color_image_view.subresourceRange.baseArrayLayer = 0;
    color_image_view.subresourceRange.layerCount = 1;
    color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    color_image_view.flags = 0;

    sc_buffer.image = swapchainImages[i];

    color_image_view.image = sc_buffer.image;

    res = vkCreateImageView(info.device, &color_image_view, NULL, &sc_buffer.view);
    info.buffers.push_back(sc_buffer);
    assert(res == VK_SUCCESS);
  }
  free(swapchainImages);
  info.current_buffer = 0;

  if (NULL != presentModes)
  {
    free(presentModes);
  }

  return res;
}

VkResult VK_Depth_Buffer(DeviceInfo &info)
{
  VkResult U_ASSERT_ONLY res;
  bool U_ASSERT_ONLY pass;
  VkImageCreateInfo image_info = {};

  /* allow custom depth formats */

  if (info.depth.format == VK_FORMAT_UNDEFINED)
    info.depth.format = VK_FORMAT_D16_UNORM;


  const VkFormat depth_format = info.depth.format;
  VkFormatProperties props;
  vkGetPhysicalDeviceFormatProperties(info.gpus[0], depth_format, &props);
  if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
  {
    image_info.tiling = VK_IMAGE_TILING_LINEAR;
  }
  else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
  {
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  }
  else
  {
    /* Try other depth formats? */
    std::cout << "depth_format " << depth_format << " Unsupported.\n";
    exit(-1);
  }

  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.pNext = NULL;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.format = depth_format;
  image_info.extent.width = info.width;
  image_info.extent.height = info.height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.samples = NUM_SAMPLES;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.queueFamilyIndexCount = 0;
  image_info.pQueueFamilyIndices = NULL;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  image_info.flags = 0;

  VkMemoryAllocateInfo mem_alloc = {};
  mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  mem_alloc.pNext = NULL;
  mem_alloc.allocationSize = 0;
  mem_alloc.memoryTypeIndex = 0;

  VkImageViewCreateInfo view_info = {};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.pNext = NULL;
  view_info.image = VK_NULL_HANDLE;
  view_info.format = depth_format;
  view_info.components.r = VK_COMPONENT_SWIZZLE_R;
  view_info.components.g = VK_COMPONENT_SWIZZLE_G;
  view_info.components.b = VK_COMPONENT_SWIZZLE_B;
  view_info.components.a = VK_COMPONENT_SWIZZLE_A;
  view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.flags = 0;

  if (depth_format == VK_FORMAT_D16_UNORM_S8_UINT || depth_format == VK_FORMAT_D24_UNORM_S8_UINT || depth_format == VK_FORMAT_D32_SFLOAT_S8_UINT)
  {
    view_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
  }

  VkMemoryRequirements mem_reqs;

  /* Create image */
  res = vkCreateImage(info.device, &image_info, NULL, &info.depth.image);
  assert(res == VK_SUCCESS);

  vkGetImageMemoryRequirements(info.device, info.depth.image, &mem_reqs);

  mem_alloc.allocationSize = mem_reqs.size;
  /* Use the memory properties to determine the type of memory required */
  pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);
  assert(pass);

  /* Allocate memory */
  res = vkAllocateMemory(info.device, &mem_alloc, NULL, &info.depth.mem);
  assert(res == VK_SUCCESS);

  /* Bind memory */
  res = vkBindImageMemory(info.device, info.depth.image, info.depth.mem, 0);
  assert(res == VK_SUCCESS);

  /* Create image view */
  view_info.image = info.depth.image;
  res = vkCreateImageView(info.device, &view_info, NULL, &info.depth.view);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_Uniform_Buffer(DeviceInfo & info)
{
  VkResult U_ASSERT_ONLY res;
  bool U_ASSERT_ONLY pass;
  float fov = glm::radians(45.0f);

  if (info.width > info.height)
  {
    fov *= static_cast<float>(info.height) / static_cast<float>(info.width);
  }

  info.Projection = glm::perspective(fov, static_cast<float>(info.width) / static_cast<float>(info.height), 0.1f, 100.0f);
  info.View = glm::lookAt(glm::vec3(-5, 3, -10),  // Camera is at (-5,3,-10), in World Space
    glm::vec3(0, 0, 0),     // and looks at the origin
    glm::vec3(0, -1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
  );

  info.Model = glm::mat4(1.0f);
  // Vulkan clip space has inverted Y and half Z.
  info.Clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

  info.MVP = info.Clip * info.Projection * info.View * info.Model;

  /* VULKAN_KEY_START */
  VkBufferCreateInfo buf_info = {};
  buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buf_info.pNext = NULL;
  buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  buf_info.size = sizeof(info.MVP);
  buf_info.queueFamilyIndexCount = 0;
  buf_info.pQueueFamilyIndices = NULL;
  buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  buf_info.flags = 0;
  res = vkCreateBuffer(info.device, &buf_info, NULL, &info.uniform_data.buf);
  assert(res == VK_SUCCESS);

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(info.device, info.uniform_data.buf, &mem_reqs);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.memoryTypeIndex = 0;

  alloc_info.allocationSize = mem_reqs.size;
  pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &alloc_info.memoryTypeIndex);
  assert(pass && "No mappable, coherent memory");

  res = vkAllocateMemory(info.device, &alloc_info, NULL, &(info.uniform_data.mem));
  assert(res == VK_SUCCESS);

  uint8_t *pData;
  res = vkMapMemory(info.device, info.uniform_data.mem, 0, mem_reqs.size, 0, (void **)&pData);
  assert(res == VK_SUCCESS);

  memcpy(pData, &info.MVP, sizeof(info.MVP));

  vkUnmapMemory(info.device, info.uniform_data.mem);

  res = vkBindBufferMemory(info.device, info.uniform_data.buf, info.uniform_data.mem, 0);
  assert(res == VK_SUCCESS);

  info.uniform_data.buffer_info.buffer = info.uniform_data.buf;
  info.uniform_data.buffer_info.offset = 0;
  info.uniform_data.buffer_info.range = sizeof(info.MVP);

  return res;
}

VkResult VK_Descriptor_Pipeline_Layouts(DeviceInfo & info, bool use_texture, VkDescriptorSetLayoutCreateFlags descSetLayoutCreateFlags)
{
  VkDescriptorSetLayoutBinding layout_bindings[2];
  layout_bindings[0].binding = 0;
  layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layout_bindings[0].descriptorCount = 1;
  layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layout_bindings[0].pImmutableSamplers = NULL;

  if (use_texture)
  {
    layout_bindings[1].binding = 1;
    layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout_bindings[1].descriptorCount = 1;
    layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_bindings[1].pImmutableSamplers = NULL;
  }

  /* Next take layout bindings and use them to create a descriptor set layout
  */
  VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
  descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_layout.pNext = NULL;
  descriptor_layout.flags = descSetLayoutCreateFlags;
  descriptor_layout.bindingCount = use_texture ? 2 : 1;
  descriptor_layout.pBindings = layout_bindings;

  VkResult U_ASSERT_ONLY res;

  info.desc_layout.resize(NUM_DESCRIPTOR_SETS);
  res = vkCreateDescriptorSetLayout(info.device, &descriptor_layout, NULL, info.desc_layout.data());
  assert(res == VK_SUCCESS);

  /* Now use the descriptor layout to create a pipeline layout */
  VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
  pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pPipelineLayoutCreateInfo.pNext = NULL;
  pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pPipelineLayoutCreateInfo.pPushConstantRanges = NULL;
  pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
  pPipelineLayoutCreateInfo.pSetLayouts = info.desc_layout.data();

  res = vkCreatePipelineLayout(info.device, &pPipelineLayoutCreateInfo, NULL, &info.pipeline_layout);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_Renderpass(DeviceInfo & info, bool include_depth, bool clear, VkImageLayout finalLayout)
{
  /* DEPENDS on init_swap_chain() and init_depth_buffer() */

  VkResult U_ASSERT_ONLY res;
  /* Need attachments for render target and depth buffer */
  VkAttachmentDescription attachments[2];
  attachments[0].format = info.format;
  attachments[0].samples = NUM_SAMPLES;
  attachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
  attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachments[0].finalLayout = finalLayout;
  attachments[0].flags = 0;

  if (include_depth) {
    attachments[1].format = info.depth.format;
    attachments[1].samples = NUM_SAMPLES;
    attachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].flags = 0;
  }

  VkAttachmentReference color_reference = {};
  color_reference.attachment = 0;
  color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_reference = {};
  depth_reference.attachment = 1;
  depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.flags = 0;
  subpass.inputAttachmentCount = 0;
  subpass.pInputAttachments = NULL;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_reference;
  subpass.pResolveAttachments = NULL;
  subpass.pDepthStencilAttachment = include_depth ? &depth_reference : NULL;
  subpass.preserveAttachmentCount = 0;
  subpass.pPreserveAttachments = NULL;

  VkRenderPassCreateInfo rp_info = {};
  rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  rp_info.pNext = NULL;
  rp_info.attachmentCount = include_depth ? 2 : 1;
  rp_info.pAttachments = attachments;
  rp_info.subpassCount = 1;
  rp_info.pSubpasses = &subpass;
  rp_info.dependencyCount = 0;
  rp_info.pDependencies = NULL;

  res = vkCreateRenderPass(info.device, &rp_info, NULL, &info.render_pass);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_Shaders(DeviceInfo & info, const char * vertShaderText, const char * fragShaderText)
{
  VkResult U_ASSERT_ONLY res;
  bool U_ASSERT_ONLY retVal;
  
  // If no shaders were submitted, just return
  if (!(vertShaderText || fragShaderText))
    return VK_SUCCESS;
  
  glslang::InitializeProcess();

  VkShaderModuleCreateInfo moduleCreateInfo;
  
  if (vertShaderText)
  {
    std::vector<unsigned int> vtx_spv;
    info.shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.shaderStages[0].pNext = NULL;
    info.shaderStages[0].pSpecializationInfo = NULL;
    info.shaderStages[0].flags = 0;
    info.shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    info.shaderStages[0].pName = "main";

    retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, vtx_spv);
    assert(retVal);

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = vtx_spv.data();
    res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL, &info.shaderStages[0].module);
    assert(res == VK_SUCCESS);
  }

  if (fragShaderText)
  {
    std::vector<unsigned int> frag_spv;
    info.shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.shaderStages[1].pNext = NULL;
    info.shaderStages[1].pSpecializationInfo = NULL;
    info.shaderStages[1].flags = 0;
    info.shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    info.shaderStages[1].pName = "main";

    retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText, frag_spv);
    assert(retVal);

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = frag_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = frag_spv.data();
    res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL, &info.shaderStages[1].module);
    assert(res == VK_SUCCESS);
  }

  glslang::FinalizeProcess();

  return res;
}

VkResult VK_Shaders_Fix(DeviceInfo & info, const char * vertShaderText, const char * fragShaderText)
{
  VkResult U_ASSERT_ONLY res;
  bool U_ASSERT_ONLY retVal;

  // If no shaders were submitted, just return
  if (!(vertShaderText || fragShaderText))
    return VK_SUCCESS;

  glslang::InitializeProcess();

  VkShaderModuleCreateInfo moduleCreateInfo;

  std::vector<unsigned int> vtx_spv;
  EShLanguage vStage = FindLanguage(VK_SHADER_STAGE_VERTEX_BIT);
  glslang::TShader vShader(vStage);

  if (vertShaderText)
  {
    
    info.shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.shaderStages[0].pNext = NULL;
    info.shaderStages[0].pSpecializationInfo = NULL;
    info.shaderStages[0].flags = 0;
    info.shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    info.shaderStages[0].pName = "main";


    

    retVal = GLSLtoSPV_Fix(vertShaderText, vtx_spv, vShader);
    assert(retVal);

    /*
    retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, vtx_spv);
    assert(retVal);

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = vtx_spv.data();
    res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL, &info.shaderStages[0].module);
    assert(res == VK_SUCCESS);
    */
  }

  std::vector<unsigned int> frag_spv;
  EShLanguage fStage = FindLanguage(VK_SHADER_STAGE_FRAGMENT_BIT);
  glslang::TShader fShader(fStage);

  if (fragShaderText)
  {
    
    info.shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.shaderStages[1].pNext = NULL;
    info.shaderStages[1].pSpecializationInfo = NULL;
    info.shaderStages[1].flags = 0;
    info.shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    info.shaderStages[1].pName = "main";    

    retVal = GLSLtoSPV_Fix(fragShaderText, frag_spv, fShader);
    assert(retVal);


    /*
    retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText, frag_spv);
    assert(retVal);

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = frag_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = frag_spv.data();
    res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL, &info.shaderStages[1].module);
    assert(res == VK_SUCCESS);
    */
  }

  glslang::TProgram program;
  program.addShader(&vShader);
  program.addShader(&fShader);

  EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
  program.link(messages);


  glslang::FinalizeProcess();

  return res;
}

VkResult VK_Shaders_From_SPIRV(DeviceInfo & info, const char *shaderFileName, std::vector<char> &shaderCode)
{

  std::ifstream file(shaderFileName, std::ios::ate | std::ios::binary);

  if (!file.is_open())
  {
    return VK_ERROR_INVALID_SHADER_NV;
  }

  size_t fileSize = (size_t)file.tellg();
  shaderCode.resize(fileSize);

  file.seekg(0);
  file.read(shaderCode.data(), fileSize);

  file.close();

  return VK_SUCCESS;
}

VkResult VK_Framebuffers(DeviceInfo & info, bool IncludeDepth)
{
  /* DEPENDS on init_depth_buffer(), init_renderpass() and
  * init_swapchain_extension() */

  VkResult U_ASSERT_ONLY res;
  VkImageView attachments[2];
  attachments[1] = info.depth.view;

  VkFramebufferCreateInfo fb_info = {};
  fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  fb_info.pNext = NULL;
  fb_info.renderPass = info.render_pass;
  fb_info.attachmentCount = IncludeDepth ? 2 : 1;
  fb_info.pAttachments = attachments;
  fb_info.width = info.width;
  fb_info.height = info.height;
  fb_info.layers = 1;

  uint32_t i;

  info.framebuffers = (VkFramebuffer *)malloc(info.swapchainImageCount * sizeof(VkFramebuffer));

  for (i = 0; i < info.swapchainImageCount; i++) {
    attachments[0] = info.buffers[i].view;
    res = vkCreateFramebuffer(info.device, &fb_info, NULL, &info.framebuffers[i]);
    assert(res == VK_SUCCESS);
  }

  return res;
}

VkResult VK_VertexBuffer(DeviceInfo & info, const void * vertexData, uint32_t dataSize, uint32_t dataStride, bool IsTextured)
{
  VkResult U_ASSERT_ONLY res;
  bool U_ASSERT_ONLY pass;

  VkBufferCreateInfo buf_info = {};
  buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buf_info.pNext = NULL;
  buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  buf_info.size = dataSize;
  buf_info.queueFamilyIndexCount = 0;
  buf_info.pQueueFamilyIndices = NULL;
  buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  buf_info.flags = 0;
  res = vkCreateBuffer(info.device, &buf_info, NULL, &info.vertex_buffer.buf);
  assert(res == VK_SUCCESS);

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(info.device, info.vertex_buffer.buf, &mem_reqs);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.memoryTypeIndex = 0;

  alloc_info.allocationSize = mem_reqs.size;
  pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &alloc_info.memoryTypeIndex);
  assert(pass && "No mappable, coherent memory");

  res = vkAllocateMemory(info.device, &alloc_info, NULL, &(info.vertex_buffer.mem));
  assert(res == VK_SUCCESS);
  info.vertex_buffer.buffer_info.range = mem_reqs.size;
  info.vertex_buffer.buffer_info.offset = 0;

  uint8_t *pData;
  res = vkMapMemory(info.device, info.vertex_buffer.mem, 0, mem_reqs.size, 0, (void **)&pData);
  assert(res == VK_SUCCESS);

  memcpy(pData, vertexData, dataSize);

  vkUnmapMemory(info.device, info.vertex_buffer.mem);

  res = vkBindBufferMemory(info.device, info.vertex_buffer.buf, info.vertex_buffer.mem, 0);
  assert(res == VK_SUCCESS);

  info.vi_binding.binding = 0;
  info.vi_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  info.vi_binding.stride = dataStride;

  info.vi_attribs[0].binding = 0;
  info.vi_attribs[0].location = 0;
  info.vi_attribs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
  info.vi_attribs[0].offset = 0;
  info.vi_attribs[1].binding = 0;
  info.vi_attribs[1].location = 1;
  info.vi_attribs[1].format = IsTextured ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
  info.vi_attribs[1].offset = 16;

  return res;
}

VkResult VK_DescriptorPool(DeviceInfo & info, bool IsTextured)
{
  /* DEPENDS on init_uniform_buffer() and
 * init_descriptor_and_pipeline_layouts() */

  VkResult U_ASSERT_ONLY res;
  VkDescriptorPoolSize type_count[2];
  type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  type_count[0].descriptorCount = 1;
  if (IsTextured) {
    type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    type_count[1].descriptorCount = 1;
  }

  VkDescriptorPoolCreateInfo descriptor_pool = {};
  descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool.pNext = NULL;
  descriptor_pool.maxSets = 1;
  descriptor_pool.poolSizeCount = IsTextured ? 2 : 1;
  descriptor_pool.pPoolSizes = type_count;

  res = vkCreateDescriptorPool(info.device, &descriptor_pool, NULL, &info.desc_pool);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_DescriptorSet(DeviceInfo & info, bool IsTextured)
{
  /* DEPENDS on init_descriptor_pool() */

  VkResult U_ASSERT_ONLY res;

  VkDescriptorSetAllocateInfo alloc_info[1];
  alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info[0].pNext = NULL;
  alloc_info[0].descriptorPool = info.desc_pool;
  alloc_info[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
  alloc_info[0].pSetLayouts = info.desc_layout.data();

  info.desc_set.resize(NUM_DESCRIPTOR_SETS);
  res = vkAllocateDescriptorSets(info.device, alloc_info, info.desc_set.data());
  assert(res == VK_SUCCESS);

  VkWriteDescriptorSet writes[2];

  writes[0] = {};
  writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[0].pNext = NULL;
  writes[0].dstSet = info.desc_set[0];
  writes[0].descriptorCount = 1;
  writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writes[0].pBufferInfo = &info.uniform_data.buffer_info;
  writes[0].dstArrayElement = 0;
  writes[0].dstBinding = 0;

  if (IsTextured) {
    writes[1] = {};
    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = info.desc_set[0];
    writes[1].dstBinding = 1;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo = &info.texture_data.image_info;
    writes[1].dstArrayElement = 0;
  }

  vkUpdateDescriptorSets(info.device, IsTextured ? 2 : 1, writes, 0, NULL);

  return res;
}

VkResult VK_PipelineCache(DeviceInfo & info)
{
  VkResult U_ASSERT_ONLY res;

  VkPipelineCacheCreateInfo pipelineCache;
  pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  pipelineCache.pNext = NULL;
  pipelineCache.initialDataSize = 0;
  pipelineCache.pInitialData = NULL;
  pipelineCache.flags = 0;
  res = vkCreatePipelineCache(info.device, &pipelineCache, NULL, &info.pipelineCache);
  assert(res == VK_SUCCESS);

  return res;
}

VkResult VK_Pipeline(DeviceInfo & info, VkBool32 include_depth, VkBool32 include_vi)
{
  VkResult U_ASSERT_ONLY res;

  VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
  VkPipelineDynamicStateCreateInfo dynamicState = {};
  memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.pNext = NULL;
  dynamicState.pDynamicStates = dynamicStateEnables;
  dynamicState.dynamicStateCount = 0;

  VkPipelineVertexInputStateCreateInfo vi;
  memset(&vi, 0, sizeof(vi));
  vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  if (include_vi) {
    vi.pNext = NULL;
    vi.flags = 0;
    vi.vertexBindingDescriptionCount = 1;
    vi.pVertexBindingDescriptions = &info.vi_binding;
    vi.vertexAttributeDescriptionCount = 2;
    vi.pVertexAttributeDescriptions = info.vi_attribs;
  }
  VkPipelineInputAssemblyStateCreateInfo ia;
  ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  ia.pNext = NULL;
  ia.flags = 0;
  ia.primitiveRestartEnable = VK_FALSE;
  ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkPipelineRasterizationStateCreateInfo rs;
  rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rs.pNext = NULL;
  rs.flags = 0;
  rs.polygonMode = VK_POLYGON_MODE_FILL;
  rs.cullMode = VK_CULL_MODE_BACK_BIT;
  rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rs.depthClampEnable = VK_FALSE;
  rs.rasterizerDiscardEnable = VK_FALSE;
  rs.depthBiasEnable = VK_FALSE;
  rs.depthBiasConstantFactor = 0;
  rs.depthBiasClamp = 0;
  rs.depthBiasSlopeFactor = 0;
  rs.lineWidth = 1.0f;

  VkPipelineColorBlendStateCreateInfo cb;
  cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  cb.flags = 0;
  cb.pNext = NULL;
  VkPipelineColorBlendAttachmentState att_state[1];
  att_state[0].colorWriteMask = 0xf;
  att_state[0].blendEnable = VK_FALSE;
  att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
  att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
  att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  cb.attachmentCount = 1;
  cb.pAttachments = att_state;
  cb.logicOpEnable = VK_FALSE;
  cb.logicOp = VK_LOGIC_OP_NO_OP;
  cb.blendConstants[0] = 1.0f;
  cb.blendConstants[1] = 1.0f;
  cb.blendConstants[2] = 1.0f;
  cb.blendConstants[3] = 1.0f;

  VkPipelineViewportStateCreateInfo vp = {};
  vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  vp.pNext = NULL;
  vp.flags = 0;
//#ifndef __ANDROID__
  vp.viewportCount = NUM_VIEWPORTS;
  dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
  vp.scissorCount = NUM_SCISSORS;
  dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
  vp.pScissors = NULL;
  vp.pViewports = NULL;
#if 0
  // Temporary disabling dynamic viewport on Android because some of drivers doesn't
  // support the feature.
  VkViewport viewports;
  viewports.minDepth = 0.0f;
  viewports.maxDepth = 1.0f;
  viewports.x = 0;
  viewports.y = 0;
  viewports.width = info.width;
  viewports.height = info.height;
  VkRect2D scissor;
  scissor.extent.width = info.width;
  scissor.extent.height = info.height;
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  vp.viewportCount = NUM_VIEWPORTS;
  vp.scissorCount = NUM_SCISSORS;
  vp.pScissors = &scissor;
  vp.pViewports = &viewports;
#endif
  VkPipelineDepthStencilStateCreateInfo ds;
  ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  ds.pNext = NULL;
  ds.flags = 0;
  ds.depthTestEnable = include_depth;
  ds.depthWriteEnable = include_depth;
  ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  ds.depthBoundsTestEnable = VK_FALSE;
  ds.stencilTestEnable = VK_FALSE;
  ds.back.failOp = VK_STENCIL_OP_KEEP;
  ds.back.passOp = VK_STENCIL_OP_KEEP;
  ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
  ds.back.compareMask = 0;
  ds.back.reference = 0;
  ds.back.depthFailOp = VK_STENCIL_OP_KEEP;
  ds.back.writeMask = 0;
  ds.minDepthBounds = 0;
  ds.maxDepthBounds = 0;
  ds.stencilTestEnable = VK_FALSE;
  ds.front = ds.back;

  VkPipelineMultisampleStateCreateInfo ms;
  ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  ms.pNext = NULL;
  ms.flags = 0;
  ms.pSampleMask = NULL;
  ms.rasterizationSamples = NUM_SAMPLES;
  ms.sampleShadingEnable = VK_FALSE;
  ms.alphaToCoverageEnable = VK_FALSE;
  ms.alphaToOneEnable = VK_FALSE;
  ms.minSampleShading = 0.0;

  VkGraphicsPipelineCreateInfo pipeline;
  pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline.pNext = NULL;
  pipeline.layout = info.pipeline_layout;
  pipeline.basePipelineHandle = VK_NULL_HANDLE;
  pipeline.basePipelineIndex = 0;
  pipeline.flags = 0;
  pipeline.pVertexInputState = &vi;
  pipeline.pInputAssemblyState = &ia;
  pipeline.pRasterizationState = &rs;
  pipeline.pColorBlendState = &cb;
  pipeline.pTessellationState = NULL;
  pipeline.pMultisampleState = &ms;
  pipeline.pDynamicState = &dynamicState;
  pipeline.pViewportState = &vp;
  pipeline.pDepthStencilState = &ds;
  pipeline.pStages = info.shaderStages;
  pipeline.stageCount = 2;
  pipeline.renderPass = info.render_pass;
  pipeline.subpass = 0;

  res = vkCreateGraphicsPipelines(info.device, info.pipelineCache, 1, &pipeline, NULL, &info.pipeline);
  assert(res == VK_SUCCESS);
  return VkResult();
}

void VK_Viewports(DeviceInfo & info)
{
  info.viewport.height = (float)info.height;
  info.viewport.width = (float)info.width;
  info.viewport.minDepth = (float)0.0f;
  info.viewport.maxDepth = (float)1.0f;
  info.viewport.x = 0;
  info.viewport.y = 0;
  vkCmdSetViewport(info.cmd, 0, NUM_VIEWPORTS, &info.viewport);
}

void VK_Scissors(DeviceInfo & info)
{
  info.scissor.extent.width = info.width;
  info.scissor.extent.height = info.height;
  info.scissor.offset.x = 0;
  info.scissor.offset.y = 0;
  vkCmdSetScissor(info.cmd, 0, NUM_SCISSORS, &info.scissor);
}

VkResult VK_RenderCube(DeviceInfo & info)
{
  VkResult U_ASSERT_ONLY res;

  VkClearValue clear_values[2];
  clear_values[0].color.float32[0] = 0.2f;
  clear_values[0].color.float32[1] = 0.2f;
  clear_values[0].color.float32[2] = 0.2f;
  clear_values[0].color.float32[3] = 0.2f;
  clear_values[1].depthStencil.depth = 1.0f;
  clear_values[1].depthStencil.stencil = 0;

  VkSemaphore imageAcquiredSemaphore;
  VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
  imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  imageAcquiredSemaphoreCreateInfo.pNext = NULL;
  imageAcquiredSemaphoreCreateInfo.flags = 0;

  res = vkCreateSemaphore(info.device, &imageAcquiredSemaphoreCreateInfo, NULL, &imageAcquiredSemaphore);
  assert(res == VK_SUCCESS);

  // Get the index of the next available swapchain image:
  res = vkAcquireNextImageKHR(info.device, info.swap_chain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE,
    &info.current_buffer);
  // TODO: Deal with the VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR
  // return codes
  assert(res == VK_SUCCESS);

  VkRenderPassBeginInfo rp_begin;
  rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  rp_begin.pNext = NULL;
  rp_begin.renderPass = info.render_pass;
  rp_begin.framebuffer = info.framebuffers[info.current_buffer];
  rp_begin.renderArea.offset.x = 0;
  rp_begin.renderArea.offset.y = 0;
  rp_begin.renderArea.extent.width = info.width;
  rp_begin.renderArea.extent.height = info.height;
  rp_begin.clearValueCount = 2;
  rp_begin.pClearValues = clear_values;

  vkCmdBeginRenderPass(info.cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline);
  vkCmdBindDescriptorSets(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline_layout, 0, NUM_DESCRIPTOR_SETS,
    info.desc_set.data(), 0, NULL);

  const VkDeviceSize offsets[1] = { 0 };
  vkCmdBindVertexBuffers(info.cmd, 0, 1, &info.vertex_buffer.buf, offsets);

  VK_Viewports(info);
  VK_Scissors(info);

  vkCmdDraw(info.cmd, 12 * 3, 1, 0, 0);
  vkCmdEndRenderPass(info.cmd);
  res = vkEndCommandBuffer(info.cmd);
  const VkCommandBuffer cmd_bufs[] = { info.cmd };

  VkFenceCreateInfo fenceInfo;
  VkFence drawFence;
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.pNext = NULL;
  fenceInfo.flags = 0;
  vkCreateFence(info.device, &fenceInfo, NULL, &drawFence);

  VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submit_info[1] = {};
  submit_info[0].pNext = NULL;
  submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info[0].waitSemaphoreCount = 1;
  submit_info[0].pWaitSemaphores = &imageAcquiredSemaphore;
  submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
  submit_info[0].commandBufferCount = 1;
  submit_info[0].pCommandBuffers = cmd_bufs;
  submit_info[0].signalSemaphoreCount = 0;
  submit_info[0].pSignalSemaphores = NULL;

  /* Queue the command buffer for execution */
  res = vkQueueSubmit(info.graphics_queue, 1, submit_info, drawFence);
  assert(res == VK_SUCCESS);

  /* Now present the image in the window */

  VkPresentInfoKHR present;
  present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present.pNext = NULL;
  present.swapchainCount = 1;
  present.pSwapchains = &info.swap_chain;
  present.pImageIndices = &info.current_buffer;
  present.pWaitSemaphores = NULL;
  present.waitSemaphoreCount = 0;
  present.pResults = NULL;

  /* Make sure command buffer is finished before presenting */
  do {
    res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
  } while (res == VK_TIMEOUT);

  assert(res == VK_SUCCESS);
  res = vkQueuePresentKHR(info.present_queue, &present);
  assert(res == VK_SUCCESS);
  
  //res = vkResetCommandBuffer(info.cmd, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
  //res = vkResetCommandPool(info.device, info.cmd_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

  return res;
}



VkResult VK_Shutdown_Sequence(DeviceInfo &info)
{
  return VK_SUCCESS;
}
