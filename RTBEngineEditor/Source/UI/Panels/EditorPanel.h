#pragma once

namespace RTBEditor {

    class EditorPanel {
    public:
        virtual ~EditorPanel() = default;
        virtual void OnUIRender() = 0;
    };

}
