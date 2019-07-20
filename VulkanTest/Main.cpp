
#include "AF_VKWrapper.h"

// make sure to link vulkan-1.lib to the project


int main(void)
{
  DeviceInfo VK_Engine = {};

  VK_Start_Sequence(VK_Engine);
  // actual engine loop here

  UpdateWindow(VK_Engine.window);

  MSG msg = {};
  while(GetMessage(&msg, NULL, 0, 0)) //maybe pass the game itself the vulkan engine
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  //
  VK_Shutdown_Sequence(VK_Engine);

  return 0;
}
