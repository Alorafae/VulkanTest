# VulkanTest
Messing around with the new Vulkan API.

# Current State
Yes this repo does look a bit like a mess hence the Test part of the repo name.

This is a work in progress of creating a project/repo for anyone to be able to easily start a Vulkan project from. Almost all the files are from Khronos/Valve/Lunarg collab Vulkan SDK. The SDK tutorial can be found at https://vulkan.lunarg.com/

Eventually, hopefully soon, I will have the files and libraries trimmed down to only what is needed in a project that anyone can clone/fork and start working with Vulkan as quickly as possible.

This is also a lot of me learning Vulkan as well.

# My Files
My files are the ones that start with the AF prefix (plus Main.cpp) and are just me condensing/simplifying the Vulkan tutorial and getting everything up and running in a single project with all the libraries built & linked for VS2017. Other than these files everything else is from LunarG and the Vulkan/glslang repos.

# Issues
I have not properly linked the x64 libraries for an x64 build but that is one of my priorities.

Doesn't actually render anything yet, but does create everything and compile some simple shaders, so soon tm.

There are a lot of excess files that are not being used yet or are duplicates due to a very wonky file structure from getting everything included and linked.

One library that is part of the files from LunarG, SPIRV-Tools-opt.lib, is not in the repo due to some weird conflict. Not being used right now anyway.
