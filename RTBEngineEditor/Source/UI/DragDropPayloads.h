#pragma once
#include <string>

namespace RTBEditor {
    // Payload type identifiers
    constexpr const char* PAYLOAD_TEXTURE = "PAYLOAD_TEXTURE";
    constexpr const char* PAYLOAD_MESH = "PAYLOAD_MESH";
    constexpr const char* PAYLOAD_AUDIOCLIP = "PAYLOAD_AUDIOCLIP";
    constexpr const char* PAYLOAD_FONT = "PAYLOAD_FONT";
    constexpr const char* PAYLOAD_CUBEMAP = "PAYLOAD_CUBEMAP";
    constexpr const char* PAYLOAD_GAMEOBJECT = "PAYLOAD_GAMEOBJECT";
    constexpr const char* PAYLOAD_PREFAB = "PAYLOAD_PREFAB";

    // Payload structures
    struct TexturePayload {
        char path[256];
    };

    struct MeshPayload {
        char path[256];
    };

    struct AudioClipPayload {
        char path[256];
    };

    struct FontPayload {
        char path[256];
    };

    struct CubemapPayload {
        char path[256];
    };

    struct GameObjectPayload {
        uint64_t gameObjectId;
    };

    struct PrefabPayload {
        char path[256];
    };

}
