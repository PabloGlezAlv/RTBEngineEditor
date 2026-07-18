#pragma once

#include <functional>
#include <RTBEngine/Scene/PrefabInstanceResolver.h>
#include "../Panels/EditorPanel.h"

namespace RTBEngine {
    namespace Scene {
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
            const RTBEngine::Scene::PrefabInstanceContext& context,
            RTBEngine::Scene::GameObject* gameObject,
            EditorContext& editorContext,
            const std::function<void()>& markDirty);

        static bool DrawPropertyOverrideMenu(
            RTBEngine::Scene::GameObject* gameObject,
            RTBEngine::Scene::Component* component,
            const RTBEngine::Reflection::PropertyInfo& property,
            const std::function<void()>& markDirty);

        static bool DrawTransformOverrideMenu(
            RTBEngine::Scene::GameObject* gameObject,
            const char* popupId,
            const std::function<void()>& markDirty);

        static bool DrawComponentOverrideMenu(
            RTBEngine::Scene::GameObject* gameObject,
            RTBEngine::Scene::Component* component,
            bool isAddedComponent,
            const std::function<void()>& markDirty);

        static bool IsPropertyOverridden(
            RTBEngine::Scene::GameObject* gameObject,
            RTBEngine::Scene::Component* component,
            const RTBEngine::Reflection::PropertyInfo& property);
    };

}
