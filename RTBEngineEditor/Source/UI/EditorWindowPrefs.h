#pragma once

#include "Panels/EditorPanel.h"
#include <filesystem>

namespace RTBEditor {

    class EditorWindowPrefs {
    public:
        static std::filesystem::path GetPrefsPath();
        static void LoadInto(EditorContext& context);
        static void SaveFrom(const EditorContext& context);
    };

}
