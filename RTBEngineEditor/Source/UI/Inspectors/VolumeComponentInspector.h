#pragma once

namespace RTBEngine {
    namespace Scene {
        class VolumeComponent;
    }
}

namespace RTBEditor {

    // Checkbox + collapsing header (Unity-style). Returns true if enabled toggled.
    bool DrawVolumeComponentHeader(RTBEngine::Scene::VolumeComponent* volume, bool& outOpen);

    // Bounds plus Distance Fog, Volumetric Fog, and Bloom, with a checkbox per effect. Returns true if changed.
    bool DrawVolumeComponentInspector(RTBEngine::Scene::VolumeComponent* volume);

}
