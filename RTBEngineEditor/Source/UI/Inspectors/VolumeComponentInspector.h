#pragma once

namespace RTBEngine {
    namespace Scene {
        class VolumeComponent;
    }
}

namespace RTBEditor {

    // Checkbox + collapsing header (Unity-style). Returns true if enabled toggled.
    bool DrawVolumeComponentHeader(RTBEngine::Scene::VolumeComponent* volume, bool& outOpen);

    // Zona + Distance Fog / Volumetric Fog con checkbox por efecto. Returns true if changed.
    bool DrawVolumeComponentInspector(RTBEngine::Scene::VolumeComponent* volume);

}
