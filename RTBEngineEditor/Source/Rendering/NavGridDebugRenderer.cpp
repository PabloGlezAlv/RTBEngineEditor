#include "NavGridDebugRenderer.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <RTBEngine/Scene/NavAgentComponent.h>
#include <RTBEngine/Scene/NavGridComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Navigation/NavPathService.h>

#include <cstddef>
#include <functional>
#include <vector>

namespace RTBEditor {
    namespace {

        struct LineVertex {
            RTBEngine::Math::Vector3 position;
            RTBEngine::Math::Vector4 color;
        };

        void AppendLine(std::vector<LineVertex>& vertices,
                        const RTBEngine::Math::Vector3& a,
                        const RTBEngine::Math::Vector3& b,
                        const RTBEngine::Math::Vector4& color)
        {
            vertices.push_back({ a, color });
            vertices.push_back({ b, color });
        }

        void AppendBounds(std::vector<LineVertex>& vertices,
                          const RTBEngine::Scene::NavGridComponent& navGridComponent,
                          const RTBEngine::Math::Vector4& color,
                          float yOffset)
        {
            const RTBEngine::Math::Vector3 origin = navGridComponent.GetWorldOrigin();
            const RTBEngine::Math::Vector3 size = navGridComponent.GetWorldSize();
            const float y = origin.y + yOffset;

            const RTBEngine::Math::Vector3 corners[4] = {
                RTBEngine::Math::Vector3(origin.x, y, origin.z),
                RTBEngine::Math::Vector3(origin.x + size.x, y, origin.z),
                RTBEngine::Math::Vector3(origin.x + size.x, y, origin.z + size.z),
                RTBEngine::Math::Vector3(origin.x, y, origin.z + size.z),
            };

            for (int edge = 0; edge < 4; ++edge) {
                AppendLine(vertices, corners[edge], corners[(edge + 1) % 4], color);
            }
        }

        void AppendGridCells(std::vector<LineVertex>& vertices,
                             const RTBEngine::Navigation::NavGrid& grid,
                             const RTBEngine::Math::Vector4& walkableColor,
                             const RTBEngine::Math::Vector4& blockedColor,
                             float yOffset,
                             int step,
                             bool showWalkable,
                             bool showBlocked)
        {
            if (!showWalkable && !showBlocked) {
                return;
            }

            const float cellSize = grid.GetCellSize();
            const float half = cellSize * 0.45f;

            for (int z = 0; z < grid.GetHeight(); z += step) {
                for (int x = 0; x < grid.GetWidth(); x += step) {
                    const bool walkable = grid.IsWalkable(x, z);
                    if (walkable && !showWalkable) {
                        continue;
                    }
                    if (!walkable && !showBlocked) {
                        continue;
                    }

                    RTBEngine::Math::Vector3 center;
                    if (!grid.CellToWorld(x, z, center)) {
                        continue;
                    }

                    center.y += yOffset;
                    const RTBEngine::Math::Vector4& color =
                        walkable ? walkableColor : blockedColor;

                    const RTBEngine::Math::Vector3 corners[4] = {
                        RTBEngine::Math::Vector3(center.x - half, center.y, center.z - half),
                        RTBEngine::Math::Vector3(center.x + half, center.y, center.z - half),
                        RTBEngine::Math::Vector3(center.x + half, center.y, center.z + half),
                        RTBEngine::Math::Vector3(center.x - half, center.y, center.z + half),
                    };

                    for (int edge = 0; edge < 4; ++edge) {
                        AppendLine(vertices, corners[edge], corners[(edge + 1) % 4], color);
                    }
                }
            }
        }

        void AppendWaypointMarker(std::vector<LineVertex>& vertices,
                                  const RTBEngine::Math::Vector3& center,
                                  float halfSize,
                                  const RTBEngine::Math::Vector4& color)
        {
            const RTBEngine::Math::Vector3 north(center.x, center.y, center.z + halfSize);
            const RTBEngine::Math::Vector3 south(center.x, center.y, center.z - halfSize);
            const RTBEngine::Math::Vector3 east(center.x + halfSize, center.y, center.z);
            const RTBEngine::Math::Vector3 west(center.x - halfSize, center.y, center.z);

            AppendLine(vertices, north, south, color);
            AppendLine(vertices, east, west, color);
        }

        void AppendAgentPath(std::vector<LineVertex>& vertices,
                             const RTBEngine::Scene::NavAgentComponent& agent,
                             const RTBEngine::Math::Vector4& pathColor,
                             const RTBEngine::Math::Vector4& waypointColor,
                             const RTBEngine::Math::Vector4& destinationColor,
                             float yOffset)
        {
            const std::vector<RTBEngine::Math::Vector3>& waypoints = agent.GetWaypoints();
            if (waypoints.empty()) {
                return;
            }

            for (size_t index = 1; index < waypoints.size(); ++index) {
                RTBEngine::Math::Vector3 start = waypoints[index - 1];
                RTBEngine::Math::Vector3 end = waypoints[index];
                start.y += yOffset;
                end.y += yOffset;
                AppendLine(vertices, start, end, pathColor);
            }

            constexpr float kMarkerHalfSize = 0.22f;
            for (size_t index = 0; index < waypoints.size(); ++index) {
                RTBEngine::Math::Vector3 marker = waypoints[index];
                marker.y += yOffset;
                const RTBEngine::Math::Vector4& color =
                    (index + 1 == waypoints.size()) ? destinationColor : waypointColor;
                AppendWaypointMarker(vertices, marker, kMarkerHalfSize, color);
            }

            if (agent.GetOwner() && agent.HasDestination()) {
                RTBEngine::Math::Vector3 ownerPosition = agent.GetOwner()->GetWorldPosition();
                ownerPosition.y += yOffset;
                RTBEngine::Math::Vector3 destination = agent.GetDestination();
                destination.y += yOffset;
                AppendLine(vertices, ownerPosition, destination, destinationColor);
            }
        }

        RTBEngine::Scene::NavGridComponent* FindNavGridForActiveGrid(RTBEngine::Scene::Scene* scene)
        {
            const RTBEngine::Navigation::NavGrid* activeGrid =
                RTBEngine::Navigation::GetActiveNavGridForDebug();
            if (!scene || !activeGrid) {
                return nullptr;
            }

            RTBEngine::Scene::NavGridComponent* found = nullptr;
            std::function<void(RTBEngine::Scene::GameObject*)> visit =
                [&](RTBEngine::Scene::GameObject* gameObject) {
                    if (!gameObject || found) {
                        return;
                    }

                    if (auto* navGrid = gameObject->GetComponent<RTBEngine::Scene::NavGridComponent>()) {
                        if (&navGrid->GetGrid() == activeGrid) {
                            found = navGrid;
                            return;
                        }
                    }

                    for (RTBEngine::Scene::GameObject* child : gameObject->GetChildren()) {
                        visit(child);
                    }
                };

            for (const auto& gameObject : scene->GetGameObjects()) {
                if (gameObject) {
                    visit(gameObject.get());
                }
            }

            return found;
        }

    }

    NavGridDebugRenderer::NavGridDebugRenderer()
    {
        lineShader = RTBEngine::Core::ResourceManager::GetInstance().LoadShader(
            "EditorLineShader",
            "Default/Shaders/EditorLine.vert",
            "Default/Shaders/EditorLine.frag");

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        vao = device.CreateVertexArray();
        vbo = device.CreateBuffer();

        device.BindVertexArray(vao);
        device.SetArrayBufferData(vbo, nullptr, 0, RTBEngine::Rendering::RHI::BufferUsage::Dynamic);
        device.EnableVertexAttribFloat(0, 3, static_cast<int>(sizeof(LineVertex)), 0);
        device.EnableVertexAttribFloat(
            1, 4, static_cast<int>(sizeof(LineVertex)), offsetof(LineVertex, color));
        device.UnbindVertexArray();
    }

    NavGridDebugRenderer::~NavGridDebugRenderer()
    {
        if (!RTBEngine::Rendering::RHI::RenderDevice::HasDevice()) {
            vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
            vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
            return;
        }

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        if (vao != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyVertexArray(vao);
            vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        }
        if (vbo != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyBuffer(vbo);
            vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
        }
    }

    void NavGridDebugRenderer::Render(RTBEngine::Rendering::Camera* camera,
                                      RTBEngine::Scene::Scene* scene,
                                      RTBEngine::Scene::GameObject* selectedObject,
                                      const NavDebugSettings& settings)
    {
        if (!camera || !lineShader || !settings.enabled) {
            return;
        }

        RTBEngine::Scene::NavGridComponent* navGridComponent = FindNavGridForActiveGrid(scene);
        if (!navGridComponent && selectedObject) {
            if (auto* selectedNavGrid = selectedObject->GetComponent<RTBEngine::Scene::NavGridComponent>()) {
                navGridComponent = selectedNavGrid;
            }
        }

        std::vector<LineVertex> vertices;
        const RTBEngine::Math::Vector4 boundsColor(1.0f, 0.85f, 0.2f, 0.95f);
        const RTBEngine::Math::Vector4 walkableColor(0.2f, 0.85f, 0.35f, 0.55f);
        const RTBEngine::Math::Vector4 blockedColor(0.9f, 0.2f, 0.2f, 0.25f);
        const RTBEngine::Math::Vector4 pathColor(0.2f, 0.7f, 1.0f, 1.0f);
        const RTBEngine::Math::Vector4 waypointColor(1.0f, 0.95f, 0.2f, 1.0f);
        const RTBEngine::Math::Vector4 destinationColor(1.0f, 0.35f, 0.2f, 1.0f);
        const float yOffset = settings.yOffset;

        if (navGridComponent) {
            if (settings.showBounds) {
                AppendBounds(vertices, *navGridComponent, boundsColor, yOffset);
            }

            if (navGridComponent->IsBaked() &&
                (settings.showWalkableCells || settings.showBlockedCells)) {
                int step = settings.gridCellStep;
                if (step <= 0) {
                    step = navGridComponent->GetGrid().GetWidth() > 48 ? 2 : 1;
                }
                AppendGridCells(vertices,
                                navGridComponent->GetGrid(),
                                walkableColor,
                                blockedColor,
                                yOffset,
                                step,
                                settings.showWalkableCells,
                                settings.showBlockedCells);
            }
        }

        if (settings.showAgentPaths) {
            const auto& registeredAgents =
                RTBEngine::Navigation::NavPathService::GetInstance().GetRegisteredAgents();
            for (RTBEngine::Scene::NavAgentComponent* agent : registeredAgents) {
                if (!agent || !agent->GetOwner() || !agent->GetOwner()->IsActiveInHierarchy()) {
                    continue;
                }

                if (!agent->GetWaypoints().empty()) {
                    AppendAgentPath(vertices, *agent, pathColor, waypointColor, destinationColor, yOffset);
                }
            }
        }

        if (vertices.empty()) {
            return;
        }

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        device.SetArrayBufferData(
            vbo,
            vertices.data(),
            vertices.size() * sizeof(LineVertex),
            RTBEngine::Rendering::RHI::BufferUsage::Dynamic);

        device.SetDepthTest(false);

        device.BindVertexArray(vao);
        device.DrawArrays(
            RTBEngine::Rendering::RHI::PrimitiveTopology::Lines,
            0,
            static_cast<int>(vertices.size()));
        device.UnbindVertexArray();

        device.SetDepthTest(true);
        lineShader->Unbind();
    }

}
