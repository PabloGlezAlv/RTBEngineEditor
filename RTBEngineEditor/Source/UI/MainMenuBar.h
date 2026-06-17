#pragma once

#include "Panels/EditorPanel.h"
#include <functional>

namespace RTBEditor {

    class MainMenuBar {
    public:
        using BuildCallback = std::function<void()>;
        using ExitCallback = std::function<void()>;
        using SaveSceneCallback = std::function<void()>;
        using SaveSceneAsCallback = std::function<void()>;
        using EditCommandCallback = std::function<void()>;
        using EditCommandPredicate = std::function<bool()>;

        MainMenuBar();
        ~MainMenuBar();

        void OnUIRender(EditorContext& context);
        void SetExitCallback(ExitCallback callback) { exitCallback = callback; }
        void SetBuildCallback(BuildCallback callback) { buildCallback = callback; }

        void SetSaveSceneCallback(SaveSceneCallback callback) { saveSceneCallback = callback; }
        void SetSaveSceneAsCallback(SaveSceneAsCallback callback) { saveSceneAsCallback = callback; }
        void SetSceneDirty(bool dirty) { sceneDirty = dirty; }

        void SetCopyCallback(EditCommandCallback callback) { copyCallback = callback; }
        void SetPasteCallback(EditCommandCallback callback) { pasteCallback = callback; }
        void SetDuplicateCallback(EditCommandCallback callback) { duplicateCallback = callback; }
        void SetCanCopyProvider(EditCommandPredicate provider) { canCopyProvider = provider; }
        void SetCanPasteProvider(EditCommandPredicate provider) { canPasteProvider = provider; }
        void SetPersistWindowPrefsCallback(std::function<void()> callback) {
            persistWindowPrefsCallback = callback;
        }

    private:
        ExitCallback exitCallback;
        BuildCallback buildCallback;

        SaveSceneCallback saveSceneCallback;
        SaveSceneAsCallback saveSceneAsCallback;
        EditCommandCallback copyCallback;
        EditCommandCallback pasteCallback;
        EditCommandCallback duplicateCallback;
        EditCommandPredicate canCopyProvider;
        EditCommandPredicate canPasteProvider;
        std::function<void()> persistWindowPrefsCallback;
        bool sceneDirty = false;
    };

}
