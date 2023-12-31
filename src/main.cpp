#include <spdlog/spdlog.h>

#include "VulkanApp.h"

int main()
{
    VulkanApp app;
    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        spdlog::error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
