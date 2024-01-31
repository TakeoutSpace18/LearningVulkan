# LearningVulkan

I will use this repo as a space to experiment with Vulkan API and CG in general. Who knows, how will it end...

Currently, I am following [Vulkan-Tutorial](https://vulkan-tutorial.com/)

## Useful links
 - [Vulkan chunked specification](https://vulkan.lunarg.com/doc/sdk/1.3.275.0/windows/chunked_spec/index.html)
 - [VulkanMemoryAllocator documentation](https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/index.html)
 - [Vulkan GPU reports database](https://vulkan.gpuinfo.org/listdevices.php)

## Building on Linux
**Step 1:** Install [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#linux) \
**Step 2:** Clone repository with submodules
```shell
git clone https://github.com/TakeoutSpace18/LearningVulkan.git --recurse-submodules
```
**Step 3:** Build
```shell
cd LearningVulkan; mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j
```