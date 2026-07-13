#pragma once

#include <functional>
#include <RTBEngine/Scene/PrefabInstanceResolver.h>
#include "../Panels/EditorPanel.h"

namespace RTBEngine {
    namespace ECS {
        class GameObject;
        class Component;
    }
    namespace Reflection {
        struct PropertyInfo;
    }
}

namespace RTBEditor {

    class PrefabOverrideInspector {
    public:
        static bool DrawInstanceHeader(
            const RTBEngine::ECS::PrefabInstanceContext& context,
            RTBEngine::ECS::GameObject* gameObject,
            EditorContext& editorContext,
            const std::function<void()>& markDirty);

        static bool DrawPropertyOverrideMenu(
            RTBEngine::ECS::GameObject* gameObject,
            RTBEngine::ECS::Component* component,
            const RTBEngine::Reflection::PropertyInfo& property,
            const std::function<void()>& markDirty);

        static bool DrawTransformOverrideMenu(
            RTBEngine::ECS::GameObject* gameObject,
            const char* popupId,
            const std::function<void()>& markDirty);

        static bool DrawComponentOverrideMenu(
            RTBEngine::ECS::GameObject* gameObject,
            RTBEngine::ECS::Component* component,
            bool isAddedComponent,
            const std::function<void()>& markDirty);

        static bool IsPropertyOverridden(
            RTBEngine::ECS::GameObject* gameObject,
            RTBEngine::ECS::Component* component,
            const RTBEngine::Reflection::PropertyInfo& property);
    };

}
