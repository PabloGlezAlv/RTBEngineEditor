#include "PrefabOverrideInspector.h"

#include <RTBEngine/Scene/PrefabOverrideDiff.h>
#include <RTBEngine/Scene/PrefabOverrideOps.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <imgui.h>
#include <functional>

namespace RTBEditor {

    bool PrefabOverrideInspector::IsPropertyOverridden(
        RTBEngine::ECS::GameObject* gameObject,
        RTBEngine::ECS::Component* component,
        const RTBEngine::Reflection::PropertyInfo& property)
    {
        return RTBEngine::ECS::PrefabOverrideOps::IsPropertyOverridden(
            gameObject,
            component,
            &property);
    }

    bool PrefabOverrideInspector::DrawInstanceHeader(
        const RTBEngine::ECS::PrefabInstanceContext& context,
        RTBEngine::ECS::GameObject* gameObject,
        EditorContext& editorContext,
        const std::function<void()>& markDirty)
    {
        if (!context.IsValid() || !gameObject) {
            return false;
        }

        const bool editingDisabled = editorContext.state != EditorState::Edit;
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.4f, 0.6f, 1.0f, 1.0f), "Prefab: %s", context.assetName.c_str());

        if (context.target != context.instanceRoot) {
            ImGui::SameLine();
            ImGui::TextDisabled("(child override)");
        }

        ImGui::SameLine();
        if (ImGui::Button("Select Root")) {
            editorContext.selectedGameObject = context.instanceRoot;
            editorContext.selectedGameObjects = { context.instanceRoot };
        }

        ImGui::SameLine();
        if (editingDisabled) {
            ImGui::BeginDisabled();
        }

        bool acted = false;
        if (ImGui::Button("Revert All")) {
            RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
            RTBEngine::ECS::GameObject* replacement = nullptr;
            if (RTBEngine::ECS::PrefabOverrideOps::RevertAll(gameObject, scene, &replacement)) {
                if (replacement) {
                    editorContext.selectedGameObject = replacement;
                    editorContext.selectedGameObjects = { replacement };
                }
                markDirty();
                acted = true;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Unlink")) {
            std::function<void(RTBEngine::ECS::GameObject*)> clearPrefabNames =
                [&](RTBEngine::ECS::GameObject* node) {
                    if (!node) {
                        return;
                    }
                    node->SetPrefabName("");
                    for (RTBEngine::ECS::GameObject* child : node->GetChildren()) {
                        clearPrefabNames(child);
                    }
                };
            clearPrefabNames(context.instanceRoot);
            markDirty();
            acted = true;
        }

        if (editingDisabled) {
            ImGui::EndDisabled();
        }

        ImGui::Spacing();
        return acted;
    }

    bool PrefabOverrideInspector::DrawPropertyOverrideMenu(
        RTBEngine::ECS::GameObject* gameObject,
        RTBEngine::ECS::Component* component,
        const RTBEngine::Reflection::PropertyInfo& property,
        const std::function<void()>& markDirty)
    {
        if (!ImGui::BeginPopupContextItem("PrefabPropertyOverrideMenu")) {
            return false;
        }

        const RTBEngine::ECS::PrefabInstanceContext context =
            RTBEngine::ECS::PrefabInstanceResolver::Resolve(gameObject);
        if (!context.IsValid()) {
            ImGui::EndPopup();
            return false;
        }

        bool acted = false;
        const bool overridden = IsPropertyOverridden(gameObject, component, property);

        if (overridden && ImGui::MenuItem("Revert")) {
            if (RTBEngine::ECS::PrefabOverrideOps::RevertProperty(
                    gameObject,
                    component,
                    &property)) {
                component->OnValidate();
                markDirty();
                acted = true;
            }
        }

        if (overridden && ImGui::MenuItem("Apply to Prefab")) {
            if (RTBEngine::ECS::PrefabOverrideOps::ApplyProperty(
                    gameObject,
                    component,
                    &property)) {
                markDirty();
                acted = true;
            }
        }

        ImGui::EndPopup();
        return acted;
    }

    bool PrefabOverrideInspector::DrawTransformOverrideMenu(
        RTBEngine::ECS::GameObject* gameObject,
        const char* popupId,
        const std::function<void()>& markDirty)
    {
        if (!ImGui::BeginPopupContextItem(popupId)) {
            return false;
        }

        const RTBEngine::ECS::PrefabInstanceContext context =
            RTBEngine::ECS::PrefabInstanceResolver::Resolve(gameObject);
        if (!context.IsValid() || !context.baselineNode) {
            ImGui::EndPopup();
            return false;
        }

        bool acted = false;
        const bool overridden = RTBEngine::ECS::PrefabOverrideDiff::IsTransformOverridden(
            gameObject,
            context.baselineNode);

        if (overridden && ImGui::MenuItem("Revert")) {
            if (RTBEngine::ECS::PrefabOverrideOps::RevertTransform(gameObject)) {
                markDirty();
                acted = true;
            }
        }

        if (overridden && ImGui::MenuItem("Apply to Prefab")) {
            if (RTBEngine::ECS::PrefabOverrideOps::ApplyTransform(gameObject)) {
                markDirty();
                acted = true;
            }
        }

        ImGui::EndPopup();
        return acted;
    }

    bool PrefabOverrideInspector::DrawComponentOverrideMenu(
        RTBEngine::ECS::GameObject* gameObject,
        RTBEngine::ECS::Component* component,
        bool isAddedComponent,
        const std::function<void()>& markDirty)
    {
        if (!ImGui::BeginPopupContextItem("PrefabComponentOverrideMenu")) {
            return false;
        }

        bool acted = false;
        if (isAddedComponent) {
            if (ImGui::MenuItem("Revert (Remove)")) {
                if (RTBEngine::ECS::PrefabOverrideOps::RevertAddedComponent(gameObject, component)) {
                    markDirty();
                    acted = true;
                }
            }
            if (ImGui::MenuItem("Apply to Prefab")) {
                if (RTBEngine::ECS::PrefabOverrideOps::ApplyAddedComponent(gameObject, component)) {
                    markDirty();
                    acted = true;
                }
            }
        } else {
            if (ImGui::MenuItem("Revert Component")) {
                if (RTBEngine::ECS::PrefabOverrideOps::RevertComponent(
                        gameObject,
                        component->GetTypeName())) {
                    markDirty();
                    acted = true;
                }
            }
        }

        ImGui::EndPopup();
        return acted;
    }

}
