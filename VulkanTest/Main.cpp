
#include "AF_VKWrapper.h"

// make sure to link vulkan-1.lib to the project


int main(void)
{
  DeviceInfo VK_Engine = {};

  VK_Start_Sequence(VK_Engine);
  // actual engine loop here
  while(1) //maybe pass the game itself the vulkan engine
  {
    //VK_RenderCube(VK_Engine);
    ShowWindow(VK_Engine.window, 1);
    UpdateWindow(VK_Engine.window);
  }
  //
  VK_Shutdown_Sequence(VK_Engine);

  return 0;
}
