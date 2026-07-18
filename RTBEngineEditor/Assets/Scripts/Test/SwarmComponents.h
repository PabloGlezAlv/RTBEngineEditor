#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>

namespace RTBEngine {
    namespace ECS {

        // Perf benchmark: orbiting agent motion (dense POD, no hierarchy).
        struct SwarmMotion {
            Math::Vector3 center = Math::Vector3::Zero();
            float phase = 0.0f;
            float angularSpeed = 1.2f;
            float orbitRadius = 6.0f;
            float bobAmplitude = 0.75f;
            float bobSpeed = 2.4f;
            float height = 1.5f;
        };

        // Per-agent tint for instanced swarm draws.
        struct SwarmColor {
            Math::Vector4 rgba = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        };

    }
}
