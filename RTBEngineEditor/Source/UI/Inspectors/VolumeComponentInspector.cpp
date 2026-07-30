#include "VolumeComponentInspector.h"

#include <RTBEngine/Scene/VolumeComponent.h>
#include <imgui.h>
#include <functional>

namespace RTBEditor {

    namespace {

        constexpr float kLabelX = 154.0f;

        void DrawParamRow(const char* id, const char* label, const std::function<void()>& drawControl)
        {
            ImGui::PushID(id);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(label);
            ImGui::SameLine(kLabelX);
            ImGui::SetNextItemWidth(-1.0f);
            drawControl();
            ImGui::PopID();
        }

        // Checkbox next to section title (Unity Volume-style effect toggle).
        bool DrawEffectHeader(const char* id, const char* label, bool& enabled, bool& open, bool& changed)
        {
            ImGui::PushID(id);
            if (ImGui::Checkbox("##effectOn", &enabled)) {
                changed = true;
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Activa este efecto en el volumen.");
            }

            ImGui::SameLine();
            if (!enabled) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
            }
            open = ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
            if (!enabled) {
                ImGui::PopStyleColor();
            }
            ImGui::PopID();
            return open;
        }
    }

    bool DrawVolumeComponentHeader(RTBEngine::Scene::VolumeComponent* volume, bool& outOpen)
    {
        outOpen = false;
        if (!volume) {
            return false;
        }

        bool changed = false;
        bool enabled = volume->IsEnabled();

        if (ImGui::Checkbox("##volumeEnabled", &enabled)) {
            volume->SetEnabled(enabled);
            changed = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Activa o desactiva este volumen en el stack.");
        }

        ImGui::SameLine();
        if (!enabled) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
        }
        outOpen = ImGui::CollapsingHeader("Volume", ImGuiTreeNodeFlags_DefaultOpen);
        if (!enabled) {
            ImGui::PopStyleColor();
        }

        return changed;
    }

    bool DrawVolumeComponentInspector(RTBEngine::Scene::VolumeComponent* volume)
    {
        if (!volume) {
            return false;
        }

        bool changed = false;
        const bool volumeActive = volume->IsEnabled();

        ImGui::BeginDisabled(!volumeActive);

        if (ImGui::TreeNodeEx("Zona", ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawParamRow("global", "Global", [&]() {
                if (ImGui::Checkbox("##global", &volume->isGlobal)) {
                    changed = true;
                }
            });

            if (!volume->isGlobal) {
                DrawParamRow("size", "Tamaño", [&]() {
                    float size[3] = { volume->size.x, volume->size.y, volume->size.z };
                    if (ImGui::DragFloat3("##size", size, 0.1f, 0.1f, 500.0f)) {
                        volume->size = RTBEngine::Math::Vector3(size[0], size[1], size[2]);
                        changed = true;
                    }
                });

                DrawParamRow("blend", "Blend dist.", [&]() {
                    if (ImGui::DragFloat("##blend", &volume->blendDistance, 0.05f, 0.01f, 50.0f)) {
                        changed = true;
                    }
                });
            } else {
                ImGui::TextDisabled("Global: afecta toda la cámara.");
            }

            DrawParamRow("priority", "Prioridad", [&]() {
                if (ImGui::DragInt("##priority", &volume->priority, 1.0f, -100, 100)) {
                    changed = true;
                }
            });

            DrawParamRow("weight", "Peso", [&]() {
                if (ImGui::SliderFloat("##weight", &volume->weight, 0.0f, 1.0f)) {
                    changed = true;
                }
            });

            ImGui::TreePop();
        }

        ImGui::Spacing();

        bool distanceOpen = false;
        if (DrawEffectHeader("distanceFog", "Distance Fog", volume->overrideDistanceFog, distanceOpen, changed)) {
            if (volume->overrideDistanceFog) {
                volume->fogEnabled = true;
            }
            ImGui::BeginDisabled(!volume->overrideDistanceFog);

            DrawParamRow("fogCol", "Color", [&]() {
                float col[3] = { volume->fogColor.x, volume->fogColor.y, volume->fogColor.z };
                if (ImGui::ColorEdit3("##fogCol", col)) {
                    volume->fogColor = RTBEngine::Math::Vector3(col[0], col[1], col[2]);
                    changed = true;
                }
            });
            DrawParamRow("fogDen", "Densidad", [&]() {
                if (ImGui::DragFloat("##fogDen", &volume->fogDensity, 0.001f, 0.0f, 0.5f, "%.4f")) {
                    changed = true;
                }
            });
            DrawParamRow("fogH", "Altura", [&]() {
                if (ImGui::DragFloat("##fogH", &volume->fogHeight, 0.1f, -50.0f, 200.0f, "%.2f")) {
                    changed = true;
                }
            });
            DrawParamRow("fogHF", "Falloff altura", [&]() {
                if (ImGui::DragFloat("##fogHF", &volume->fogHeightFalloff, 0.01f, 0.0f, 2.0f, "%.3f")) {
                    changed = true;
                }
            });
            DrawParamRow("fogStart", "Inicio", [&]() {
                if (ImGui::DragFloat("##fogStart", &volume->fogStart, 0.5f, 0.0f, 500.0f, "%.1f")) {
                    changed = true;
                }
            });
            DrawParamRow("fogEnd", "Fin", [&]() {
                if (ImGui::DragFloat("##fogEnd", &volume->fogEnd, 1.0f, 1.0f, 1000.0f, "%.1f")) {
                    changed = true;
                }
            });

            ImGui::EndDisabled();
            ImGui::TreePop();
        }

        ImGui::Spacing();

        bool volumetricOpen = false;
        if (DrawEffectHeader("volumetricFog", "Volumetric Fog", volume->overrideVolumetricFog, volumetricOpen, changed)) {
            if (volume->overrideVolumetricFog) {
                volume->volumetricFogEnabled = true;
            }
            ImGui::BeginDisabled(!volume->overrideVolumetricFog);

            DrawParamRow("volInt", "Intensidad", [&]() {
                if (ImGui::DragFloat("##volInt", &volume->volumetricIntensity, 0.01f, 0.0f, 4.0f, "%.2f")) {
                    changed = true;
                }
            });
            DrawParamRow("volAniso", "Anisotropía", [&]() {
                if (ImGui::DragFloat("##volAniso", &volume->volumetricAnisotropy, 0.01f, -0.95f, 0.95f, "%.2f")) {
                    changed = true;
                }
            });
            DrawParamRow("volSamples", "Samples", [&]() {
                if (ImGui::SliderInt("##volSamples", &volume->volumetricSamples, 8, 64)) {
                    changed = true;
                }
            });
            DrawParamRow("volMaxL", "Max luminancia", [&]() {
                if (ImGui::DragFloat("##volMaxL", &volume->volumetricMaxLuminance, 0.01f, 0.05f, 8.0f, "%.2f")) {
                    changed = true;
                }
            });

            ImGui::EndDisabled();
            ImGui::TreePop();
        }

        ImGui::Spacing();

        bool bloomOpen = false;
        if (DrawEffectHeader("bloom", "Bloom", volume->overrideBloom, bloomOpen, changed)) {
            if (volume->overrideBloom) {
                volume->bloomEnabled = true;
            }
            ImGui::BeginDisabled(!volume->overrideBloom);

            DrawParamRow("bloomThresh", "Threshold", [&]() {
                if (ImGui::DragFloat("##bloomThresh", &volume->bloomThreshold, 0.01f, 0.0f, 8.0f, "%.2f")) {
                    changed = true;
                }
            });
            DrawParamRow("bloomInt", "Intensidad", [&]() {
                if (ImGui::DragFloat("##bloomInt", &volume->bloomIntensity, 0.01f, 0.0f, 4.0f, "%.2f")) {
                    changed = true;
                }
            });

            ImGui::EndDisabled();
            ImGui::TreePop();
        }

        ImGui::EndDisabled();

        return changed;
    }

}
