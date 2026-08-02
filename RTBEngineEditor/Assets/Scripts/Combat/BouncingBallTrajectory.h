#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>

#include <array>
#include <vector>

namespace RTBEngine {
    namespace Physics {
        class PhysicsWorld;
    }
    namespace Scene {
        class GameObject;
    }
}

namespace BouncingBallTrajectory {

    constexpr int kBounceCount = 3;
    constexpr int kSamplesPerArc = 12;

    struct Path {
        std::vector<RTBEngine::Math::Vector3> samples;
        std::vector<float> cumulativeLengths;
        std::array<RTBEngine::Math::Vector3, kBounceCount> bouncePoints{};
        std::array<float, kBounceCount> bounceRadii{2.4f, 1.7f, 1.1f};
        std::array<int, kBounceCount> bounceSampleIndices{-1, -1, -1};
        float totalLength = 0.0f;
        int bounceCount = kBounceCount;
    };

    struct BuildParams {
        RTBEngine::Math::Vector3 origin = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Vector3 planarDirection = RTBEngine::Math::Vector3::Forward();
        float totalRange = 9.0f;
        float arcHeight = 2.2f;
        float bounceRadius0 = 2.4f;
        float bounceRadius1 = 1.7f;
        float bounceRadius2 = 1.1f;
        float groundSnapLift = 0.05f;
        RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
        RTBEngine::Scene::GameObject* ignoreObject = nullptr;
    };

    void SnapToGround(
        RTBEngine::Math::Vector3& position,
        RTBEngine::Physics::PhysicsWorld* physicsWorld,
        RTBEngine::Scene::GameObject* ignoreObject,
        float groundSnapLift);

    bool Build(const BuildParams& params, Path& outPath);

    RTBEngine::Math::Vector3 EvaluateAtDistance(
        const Path& path,
        float distanceAlong,
        int* outSampleIndex = nullptr);

    // Returns bounce index 0..bounceCount-1 if sampleIndex is a bounce landing, else -1.
    int BounceIndexForSample(const Path& path, int sampleIndex);

}
