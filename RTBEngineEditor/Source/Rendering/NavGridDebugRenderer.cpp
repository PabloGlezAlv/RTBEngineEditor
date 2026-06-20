#include "NavGridDebugRenderer.h"

#include <RTBEngine/Core/ResourceManager.h>
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
                          const RTBEngine::ECS::NavGridComponent& navGridComponent,
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
                             const RTBEngine::ECS::NavAgentComponent& agent,
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

        RTBEngine::ECS::NavGridComponent* FindNavGridForActiveGrid(RTBEngine::ECS::Scene* scene)
        {
            const RTBEngine::Navigation::NavGrid* activeGrid =
                RTBEngine::Navigation::GetActiveNavGridForDebug();
            if (!scene || !activeGrid) {
                return nullptr;
            }

            RTBEngine::ECS::NavGridComponent* found = nullptr;
            std::function<void(RTBEngine::ECS::GameObject*)> visit =
                [&](RTBEngine::ECS::GameObject* gameObject) {
                    if (!gameObject || found) {
                        return;
                    }

                    if (auto* navGrid = gameObject->GetComponent<RTBEngine::ECS::NavGridComponent>()) {
                        if (&navGrid->GetGrid() == activeGrid) {
                            found = navGrid;
                            return;
                        }
                    }

                    for (RTBEngine::ECS::GameObject* child : gameObject->GetChildren()) {
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

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                              reinterpret_cast<void*>(offsetof(LineVertex, color)));

        glBindVertexArray(0);
    }

    NavGridDebugRenderer::~NavGridDebugRenderer()
    {
        if (vao) {
            glDeleteVertexArrays(1, &vao);
        }
        if (vbo) {
            glDeleteBuffers(1, &vbo);
        }
    }

    void NavGridDebugRenderer::Render(RTBEngine::Rendering::Camera* camera,
                                      RTBEngine::ECS::Scene* scene,
                                      RTBEngine::ECS::GameObject* selectedObject,
                                      const NavDebugSettings& settings)
    {
        if (!camera || !lineShader || !settings.enabled) {
            return;
        }

        RTBEngine::ECS::NavGridComponent* navGridComponent = FindNavGridForActiveGrid(scene);
        if (!navGridComponent && selectedObject) {
            if (auto* selectedNavGrid = selectedObject->GetComponent<RTBEngine::ECS::NavGridComponent>()) {
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
            for (RTBEngine::ECS::NavAgentComponent* agent : registeredAgents) {
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

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(vertices.size() * sizeof(LineVertex)),
                     vertices.data(),
                     GL_DYNAMIC_DRAW);

        glDisable(GL_DEPTH_TEST);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
        glEnable(GL_DEPTH_TEST);
        glBindVertexArray(0);
        lineShader->Unbind();
    }

}
