#include "EditorDockingUtils.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <cstring>

namespace RTBEditor {
    namespace {

        ImGuiID TryGetWindowDockId(const char* windowName)
        {
            ImGuiWindow* window = ImGui::FindWindowByName(windowName);
            if (!window || !window->DockIsActive || window->DockId == 0) {
                return 0;
            }

            return window->DockId;
        }

        ImGuiID FindDockIdForOptionalWindow(const char* openingWindowName)
        {
            static const char* kOptionalWindowNames[] = {
                "Online",
                "Physics Layers",
                "Navigation Debug",
            };

            static const char* kFallbackAnchorNames[] = {
                "Inspector",
                "Hierarchy",
                "Console",
                "Content Browser",
                "Scene",
                "Game",
            };

            for (const char* name : kOptionalWindowNames) {
                if (openingWindowName && std::strcmp(name, openingWindowName) == 0) {
                    continue;
                }

                if (const ImGuiID dockId = TryGetWindowDockId(name)) {
                    return dockId;
                }
            }

            for (const char* name : kFallbackAnchorNames) {
                if (const ImGuiID dockId = TryGetWindowDockId(name)) {
                    return dockId;
                }
            }

            return 0;
        }

    }

    void PrepareOptionalWindowDocking(const char* windowName)
    {
        if (ImGui::FindWindowByName(windowName) != nullptr) {
            return;
        }

        const ImGuiID dockId = FindDockIdForOptionalWindow(windowName);
        if (dockId != 0) {
            ImGui::SetNextWindowDockID(dockId, ImGuiCond_Always);
        }
    }

}
