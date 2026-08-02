#include "BouncingBallTrajectory.h"

#include "CharacterCombatUtils.h"

#include <RTBEngine/Physics/PhysicsWorld.h>

#include <algorithm>
#include <cmath>

namespace BouncingBallTrajectory {
    namespace {

        RTBEngine::Math::Vector3 SampleArcPoint(
            const RTBEngine::Math::Vector3& start,
            const RTBEngine::Math::Vector3& end,
            float t,
            float arcHeight)
        {
            t = std::clamp(t, 0.0f, 1.0f);
            RTBEngine::Math::Vector3 point = start + (end - start) * t;
            point.y += arcHeight * 4.0f * t * (1.0f - t);
            return point;
        }

        void AppendArcSamples(
            Path& path,
            const RTBEngine::Math::Vector3& start,
            const RTBEngine::Math::Vector3& end,
            float arcHeight,
            bool includeStart)
        {
            const int startIndex = includeStart ? 0 : 1;
            for (int i = startIndex; i <= kSamplesPerArc; ++i) {
                const float t = static_cast<float>(i) / static_cast<float>(kSamplesPerArc);
                const RTBEngine::Math::Vector3 point = SampleArcPoint(start, end, t, arcHeight);

                if (!path.samples.empty()) {
                    const RTBEngine::Math::Vector3 delta = point - path.samples.back();
                    const float segmentLength = delta.Length();
                    path.totalLength += segmentLength;
                } else {
                    path.totalLength = 0.0f;
                }

                path.samples.push_back(point);
                path.cumulativeLengths.push_back(path.totalLength);
            }
        }

    }

    void SnapToGround(
        RTBEngine::Math::Vector3& position,
        RTBEngine::Physics::PhysicsWorld* physicsWorld,
        RTBEngine::Scene::GameObject* ignoreObject,
        float groundSnapLift)
    {
        if (!physicsWorld) {
            return;
        }

        constexpr float kProbeUp = 4.0f;
        constexpr float kProbeDown = 30.0f;

        RTBEngine::Physics::PhysicsQueryOptions options;
        options.ignoredObject = ignoreObject;
        options.ignoreIgnoredObjectHierarchy = true;
        options.ignoreTriggers = true;
        options.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Default");

        const RTBEngine::Math::Vector3 castStart(position.x, position.y + kProbeUp, position.z);
        const RTBEngine::Math::Vector3 castEnd(position.x, position.y - kProbeDown, position.z);

        RTBEngine::Physics::PhysicsQueryHit hit;
        if (!physicsWorld->RaycastClosest(castStart, castEnd, hit, options)) {
            return;
        }

        if (hit.normal.y <= 0.65f) {
            return;
        }

        position.y = hit.point.y + groundSnapLift;
    }

    bool Build(const BuildParams& params, Path& outPath)
    {
        outPath = Path{};

        if (!CharacterCombatUtils::HasPlanarDirection(params.planarDirection) ||
            params.totalRange <= 0.05f) {
            return false;
        }

        const RTBEngine::Math::Vector3 direction =
            CharacterCombatUtils::NormalizePlanarDirection(params.planarDirection);
        const float spacing = params.totalRange / static_cast<float>(kBounceCount);

        outPath.bounceCount = kBounceCount;
        outPath.bounceRadii[0] = std::max(0.25f, params.bounceRadius0);
        outPath.bounceRadii[1] = std::max(0.25f, params.bounceRadius1);
        outPath.bounceRadii[2] = std::max(0.25f, params.bounceRadius2);

        outPath.samples.reserve(static_cast<std::size_t>(kBounceCount * (kSamplesPerArc + 1)));
        outPath.cumulativeLengths.reserve(outPath.samples.capacity());

        RTBEngine::Math::Vector3 previous = params.origin;
        for (int bounceIndex = 0; bounceIndex < kBounceCount; ++bounceIndex) {
            RTBEngine::Math::Vector3 bouncePoint =
                params.origin + direction * (spacing * static_cast<float>(bounceIndex + 1));
            bouncePoint.y = params.origin.y;
            SnapToGround(
                bouncePoint,
                params.physicsWorld,
                params.ignoreObject,
                params.groundSnapLift);

            outPath.bouncePoints[static_cast<std::size_t>(bounceIndex)] = bouncePoint;

            const float heightScale = 1.0f - 0.18f * static_cast<float>(bounceIndex);
            const float arcHeight = std::max(0.35f, params.arcHeight * heightScale);

            AppendArcSamples(
                outPath,
                previous,
                bouncePoint,
                arcHeight,
                bounceIndex == 0);

            outPath.bounceSampleIndices[static_cast<std::size_t>(bounceIndex)] =
                static_cast<int>(outPath.samples.size()) - 1;
            previous = bouncePoint;
        }

        return outPath.samples.size() >= 2 && outPath.totalLength > 0.01f;
    }

    RTBEngine::Math::Vector3 EvaluateAtDistance(
        const Path& path,
        float distanceAlong,
        int* outSampleIndex)
    {
        if (path.samples.empty()) {
            if (outSampleIndex) {
                *outSampleIndex = -1;
            }
            return RTBEngine::Math::Vector3::Zero();
        }

        if (path.samples.size() == 1 || path.totalLength <= 0.0f || distanceAlong <= 0.0f) {
            if (outSampleIndex) {
                *outSampleIndex = 0;
            }
            return path.samples.front();
        }

        if (distanceAlong >= path.totalLength) {
            if (outSampleIndex) {
                *outSampleIndex = static_cast<int>(path.samples.size()) - 1;
            }
            return path.samples.back();
        }

        auto it = std::lower_bound(
            path.cumulativeLengths.begin(),
            path.cumulativeLengths.end(),
            distanceAlong);
        int index = static_cast<int>(it - path.cumulativeLengths.begin());
        index = std::clamp(index, 1, static_cast<int>(path.samples.size()) - 1);

        const float lengthA = path.cumulativeLengths[static_cast<std::size_t>(index - 1)];
        const float lengthB = path.cumulativeLengths[static_cast<std::size_t>(index)];
        const float segment = std::max(0.0001f, lengthB - lengthA);
        const float t = (distanceAlong - lengthA) / segment;

        if (outSampleIndex) {
            *outSampleIndex = index;
        }

        const RTBEngine::Math::Vector3& a = path.samples[static_cast<std::size_t>(index - 1)];
        const RTBEngine::Math::Vector3& b = path.samples[static_cast<std::size_t>(index)];
        return a + (b - a) * t;
    }

    int BounceIndexForSample(const Path& path, int sampleIndex)
    {
        if (sampleIndex < 0) {
            return -1;
        }

        for (int i = 0; i < path.bounceCount; ++i) {
            if (path.bounceSampleIndices[static_cast<std::size_t>(i)] == sampleIndex) {
                return i;
            }
        }
        return -1;
    }

}
