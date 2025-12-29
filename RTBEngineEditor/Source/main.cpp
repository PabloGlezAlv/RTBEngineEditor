// RTBEngineEditor - main.cpp
// Entry point for the RTBEngine Editor application

#include <RTBEngine.h>
#include <iostream>

int main(int argc, char* argv[]) {
    RTBEngine::Core::ApplicationConfig config;
    config.window.title = "RTBEngine Editor";
    config.window.width = 1920;
    config.window.height = 1080;
    config.initialScenePath = ""; 

    return RTBEngine::Run(config);
}
