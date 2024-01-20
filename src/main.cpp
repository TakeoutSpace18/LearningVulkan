#include <spdlog/spdlog.h>

#include "Application.h"

int main()
{
    Application app;
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
