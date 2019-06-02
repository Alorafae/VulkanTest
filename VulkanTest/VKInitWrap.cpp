
#include "VKInitWrap.h"

// temporary
static DeviceInfo g_VK_Engine = {};


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

// this should take all the info from the user for the app
// this is going to be a general quick setup for vulcan to get to rendering quicker
bool VK_Vulkan_Init(void)
{
  VK_Set_Extensions(g_VK_Engine);

  VK_Init_Instance(g_VK_Engine, "blah");

  VK_Window(g_VK_Engine);

  VK_Create_Queue(g_VK_Engine);

  VK_Init_Swapchain(g_VK_Engine);

  // q gets called in swapchain?
  //VK_Create_Queue(g_VK_Engine);

  //VK_Window(g_VK_Engine);

  return true;
}

bool VK_Set_Extensions(DeviceInfo info) // take extensions maybe
{
  g_VK_Engine.instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  g_VK_Engine.instance_extension_names.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
  g_VK_Engine.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  return true;
}

bool VK_Init_Instance(DeviceInfo info, char* title)
{
  VkApplicationInfo appInfo = {
    VK_STRUCTURE_TYPE_APPLICATION_INFO,
    nullptr,
    "Vulkan",                             // app name
    VK_MAKE_VERSION(1, 0, 0),
    "Alora's Vulkan",                     // engine name
    VK_MAKE_VERSION(1, 0, 0),
    VK_MAKE_VERSION(1, 0, 0)
  };

  VkInstanceCreateInfo instance_create_info = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,         // VkStructureType            sType
    nullptr,                                        // const void*                pNext
    0,                                              // VkInstanceCreateFlags      flags
    &appInfo,                              // const VkApplicationInfo   *pApplicationInfo
    0,                                              // uint32_t                   enabledLayerCount
    nullptr,                                        // const char * const        *ppEnabledLayerNames
    g_VK_Engine.instance_extension_names.size(),    // uint32_t                   enabledExtensionCount
    g_VK_Engine.instance_extension_names.data()      // const char * const        *ppEnabledExtensionNames
  };



  // move below stuff maybs


  if (vkCreateInstance(&instance_create_info, nullptr, &g_VK_Engine.inst) != VK_SUCCESS)
  {
    printf("Could not create Vulkan instance!");
    return false;
  }
  // Instance is Created

  // get our physical GPUS that we can use
  uint32_t num_gpu = 0;
  VkResult res = vkEnumeratePhysicalDevices(g_VK_Engine.inst, &num_gpu, NULL);

  if (num_gpu < 1)
  {
    //return -1;
    return false; // no gpu, exit
  }

  g_VK_Engine.gpus.resize(num_gpu);
  res = vkEnumeratePhysicalDevices(g_VK_Engine.inst, &num_gpu, g_VK_Engine.gpus.data());
  // now we have our devices (gpus) stored

  return false;
}

bool VK_Create_Queue(DeviceInfo info)
{
  
  vkGetPhysicalDeviceQueueFamilyProperties(g_VK_Engine.gpus[0], &g_VK_Engine.queue_family_count, NULL);

  if (g_VK_Engine.queue_family_count < 1)
  { 
    return -1;
  }  

  g_VK_Engine.queue_props.resize(g_VK_Engine.queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(g_VK_Engine.gpus[0], &g_VK_Engine.queue_family_count, g_VK_Engine.queue_props.data());

  bool foundQG = false;

  for (unsigned i = 0; i < g_VK_Engine.queue_family_count; ++i)
  {
    // we want a queue flagged for graphics, since we want to draw (for now)
    if (g_VK_Engine.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      g_VK_Engine.graphics_queue_family_index= i;
      foundQG = true;
      break;
    }
  }

  if (!foundQG)
    return -1;
  if (g_VK_Engine.queue_family_count < 1)
    return -1;

  

  return false;
}

bool VK_Window(DeviceInfo info)
{
  WNDCLASSEX win_class;
  //assert(info.width > 0);
  //assert(info.height > 0);

  g_VK_Engine.connection = GetModuleHandle(NULL);
  //sprintf(info.name, "Sample");

  // Initialize the window class structure:
  win_class.cbSize = sizeof(WNDCLASSEX);
  win_class.style = CS_HREDRAW | CS_VREDRAW;
  win_class.lpfnWndProc = WndProc;
  win_class.cbClsExtra = 0;
  win_class.cbWndExtra = 0;
  win_class.hInstance = g_VK_Engine.connection;  // hInstance
  win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  win_class.lpszMenuName = NULL;
  win_class.lpszClassName = L"Vulkan Test";
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
  RECT wr = { 0, 0, 1024, 720 };
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

  g_VK_Engine.window = CreateWindowEx(0,
    L"Vulkan Test",             // class name
    L"Vulkan Test",             // app name
    WS_OVERLAPPEDWINDOW |  // window style
    WS_VISIBLE | WS_SYSMENU,
    100, 100,            // x/y coords
    wr.right - wr.left,  // width
    wr.bottom - wr.top,  // height
    NULL,                // handle to parent
    NULL,                // handle to menu
    g_VK_Engine.connection,     // hInstance
    NULL);               // no extra parameters

  if (!g_VK_Engine.window)
  {
    // It didn't work, so try to give a useful error:
    printf("Cannot create a window in which to draw!\n");
    fflush(stdout);
    exit(1);
  }
  //SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)&info);

  return false;
}

bool VK_Init_Swapchain(DeviceInfo& info)
{
  VkWin32SurfaceCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  createInfo.pNext = NULL;
  createInfo.hinstance = info.connection;
  createInfo.hwnd = info.window;
  VkResult res = vkCreateWin32SurfaceKHR(info.inst, &createInfo, NULL, &info.surface);

  if (res != VK_SUCCESS)
    return false;


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
    {
      if (pSupportsPresent[i] == VK_TRUE)
      {
        info.present_queue_family_index = i;
        break;
      }
    }
  }
  free(pSupportsPresent);

  // Generate error if could not find queues that support graphics
  // and present
  if (info.graphics_queue_family_index == UINT32_MAX || info.present_queue_family_index == UINT32_MAX)
  {
    printf("Could not find a queues for graphics and "      "present\n");
    exit(-1);
  }





  VK_Init_Device(g_VK_Engine);
  //init_device(info);

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
  if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
    info.format = VK_FORMAT_B8G8R8A8_UNORM;
  }
  else {
    assert(formatCount >= 1);
    info.format = surfFormats[0].format;
  }
  free(surfFormats);

  VkSurfaceCapabilitiesKHR surfCapabilities;

  res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info.gpus[0], info.surface, &surfCapabilities);
  assert(res == VK_SUCCESS);

  uint32_t presentModeCount;
  res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0], info.surface, &presentModeCount, NULL);
  assert(res == VK_SUCCESS);
  VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));

  res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0], info.surface, &presentModeCount, presentModes);
  assert(res == VK_SUCCESS);

  VkExtent2D swapchainExtent;
  // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
  if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
    // If the surface size is undefined, the size is set to
    // the size of the images requested.
    swapchainExtent.width = info.width;
    swapchainExtent.height = info.height;
    if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
      swapchainExtent.width = surfCapabilities.minImageExtent.width;
    }
    else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) {
      swapchainExtent.width = surfCapabilities.maxImageExtent.width;
    }

    if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
      swapchainExtent.height = surfCapabilities.minImageExtent.height;
    }
    else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) {
      swapchainExtent.height = surfCapabilities.maxImageExtent.height;
    }
  }
  else {
    // If the surface size is defined, the swap chain size must match
    swapchainExtent = surfCapabilities.currentExtent;
  }

  // The FIFO present mode is guaranteed by the spec to be supported
  VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

  // Determine the number of VkImage's to use in the swap chain.
  // We need to acquire only 1 presentable image at at time.
  // Asking for minImageCount images ensures that we can acquire
  // 1 presentable image as long as we present it before attempting
  // to acquire another.
  uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount;

  VkSurfaceTransformFlagBitsKHR preTransform;
  if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  }
  else {
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
  for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
    if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
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
  swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_ci.queueFamilyIndexCount = 0;
  swapchain_ci.pQueueFamilyIndices = NULL;
  uint32_t queueFamilyIndices[2] = { (uint32_t)info.graphics_queue_family_index, (uint32_t)info.present_queue_family_index };
  if (info.graphics_queue_family_index != info.present_queue_family_index) {
    // If the graphics and present queues are from different queue families,
    // we either have to explicitly transfer ownership of images between
    // the queues, or we have to create the swapchain with imageSharingMode
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

  info.buffers.resize(info.swapchainImageCount);
  for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
    info.buffers[i].image = swapchainImages[i];
  }
  free(swapchainImages);

  for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
    VkImageViewCreateInfo color_image_view = {};
    color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    color_image_view.pNext = NULL;
    color_image_view.flags = 0;
    color_image_view.image = info.buffers[i].image;
    color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
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

    res = vkCreateImageView(info.device, &color_image_view, NULL, &info.buffers[i].view);
    assert(res == VK_SUCCESS);
  }

  /* VULKAN_KEY_END */

  /* Clean Up */
  for (uint32_t i = 0; i < info.swapchainImageCount; i++)
  {
    vkDestroyImageView(info.device, info.buffers[i].view, NULL);
  }

  return false;
}

bool VK_Init_Device(DeviceInfo info)
{
  VkDeviceQueueCreateInfo qInfo = {};
  
  float qPrio[1] = { 0.0 };
  qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  qInfo.pNext = NULL;
  qInfo.queueCount = 1;
  qInfo.pQueuePriorities = qPrio;

  VkDeviceCreateInfo vkDevInfo = {};
  vkDevInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  vkDevInfo.pNext = NULL;
  vkDevInfo.queueCreateInfoCount = 1;
  vkDevInfo.pQueueCreateInfos = &qInfo;
  vkDevInfo.enabledExtensionCount = 0;
  vkDevInfo.ppEnabledExtensionNames = NULL;
  vkDevInfo.enabledLayerCount = 0;
  vkDevInfo.ppEnabledLayerNames = NULL;
  vkDevInfo.pEnabledFeatures = NULL;

  // Device Created
  VkResult res = vkCreateDevice(g_VK_Engine.gpus[0], &vkDevInfo, NULL, &g_VK_Engine.device);
  if (res != VK_SUCCESS)
    return -1;

  //-----------

  VkCommandPoolCreateInfo cmdPoolInfo = {};
  cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmdPoolInfo.pNext = NULL;
  cmdPoolInfo.queueFamilyIndex = qInfo.queueFamilyIndex;
  cmdPoolInfo.flags = 0;

  res = vkCreateCommandPool(g_VK_Engine.device, &cmdPoolInfo, NULL, &g_VK_Engine.cmd_pool);
  if (res != VK_SUCCESS)
    return -1;

  VkCommandBufferAllocateInfo cmdAllocInfo = {};
  cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdAllocInfo.pNext = NULL;
  cmdAllocInfo.commandPool = g_VK_Engine.cmd_pool;
  cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdAllocInfo.commandBufferCount = 1;

  res = vkAllocateCommandBuffers(g_VK_Engine.device, &cmdAllocInfo, &g_VK_Engine.cmd);
  if (res != VK_SUCCESS)
    return -1;

  return false;
}

bool VK_Vulkan_Shutdown(void)
{
  VK_Destroy_Instance(g_VK_Engine);
  VK_Destroy_Device(g_VK_Engine);

  return false;
}

bool VK_Destroy_Device(DeviceInfo info)
{
  vkDestroyDevice(g_VK_Engine.device, NULL);

  return false;
}

bool VK_Destroy_Window(DeviceInfo info)
{
  VkCommandBuffer cmd_bufs[1] = { g_VK_Engine.cmd };

  vkFreeCommandBuffers(g_VK_Engine.device, g_VK_Engine.cmd_pool, 1, cmd_bufs);
  vkDestroyCommandPool(g_VK_Engine.device, g_VK_Engine.cmd_pool, NULL);

  return false;
}

bool VK_Destroy_Instance(DeviceInfo info)
{
  vkDestroyInstance(g_VK_Engine.inst, NULL);

  return false;
}


