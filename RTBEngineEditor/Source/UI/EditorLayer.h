#include "Panels/EditorPanel.h"
#include "MainMenuBar.h"
#include <vector>
#include <memory>
#include <SDL.h>

namespace RTBEditor {

    class EditorLayer {
    public:
        EditorLayer();
        ~EditorLayer();

        void Initialize(SDL_Window* window);
        void Begin();
        void End();
        void OnUIRender();

        void AddPanel(std::unique_ptr<EditorPanel> panel);
        MainMenuBar* GetMenuBar() { return menuBar.get(); }

    private:
        void SetupDockspace();

    private:
        std::unique_ptr<MainMenuBar> menuBar;
        std::vector<std::unique_ptr<EditorPanel>> panels;
        bool isDockspaceOpen = true;
    };

}
