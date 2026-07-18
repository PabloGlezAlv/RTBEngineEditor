function CreateScene()
    return {
        name = "Swarm Perf ECS",
        skyboxEnabled = true,
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "TPERF-ECS-CAM1-4000-8000-000000000001",
                position = Vector3(0.00, 18.00, -28.00),
                rotation = Quaternion.FromEulerAngles(28.00, 0.00, 0.00),
                components = {
                    {
                        type = "CameraComponent",
                        fov = 60.00,
                        nearClip = 0.10,
                        farClip = 200.00,
                        projectionType = "Perspective",
                        orthographicSize = 5.00,
                        syncWithTransform = true,
                        isMainCamera = true
                    },
                }
            },
            {
                name = "Directional Light",
                uuid = "TPERF-ECS-LIT1-4000-8000-000000000002",
                rotation = Quaternion.FromEulerAngles(-0.79, -51.13, -34.99),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        color = Color(1.00, 0.96, 0.90, 1.00),
                        intensity = 1.15,
                        range = 10.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        syncPosition = true,
                        syncDirection = true
                    },
                }
            },
            {
                name = "Ground",
                uuid = "TPERF-ECS-GRD1-4000-8000-000000000003",
                rotation = Quaternion.FromEulerAngles(0.00, -180.00, -180.00),
                scale = Vector3(40.00, 1.00, 40.00),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Default/Models/plane.obj",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                        shaderRef = "basic",
                        shaderPropertyOverrides = "",
                        meshIndex = 0,
                        multiMesh = false
                    },
                }
            },
            {
                name = "PerfBenchmark",
                uuid = "TPERF-ECS-BNCH-4000-8000-000000000004",
                components = {
                    {
                        type = "PerfSwarmBenchmark",
                        useEcs = true,
                        agentCount = 1500,
                        spawnRadiusMin = 2.00,
                        spawnRadiusMax = 14.00,
                        agentScale = 0.12,
                        statusRefreshSeconds = 0.35,
                        statusTextObjectName = "PerfStatusText"
                    },
                }
            },
            {
                name = "Canvas",
                uuid = "TPERF-ECS-CVS1-4000-8000-000000000005",
                components = {
                    {
                        type = "Canvas",
                        renderMode = "ScreenSpaceOverlay",
                        canvasSize = Vector2(1920.00, 1080.00),
                        pixelsPerUnit = 100.00,
                        sortOrder = 10,
                        faceCamera = false,
                        faceCameraLockY = false
                    },
                },
                children = {
                    {
                        name = "PerfStatusText",
                        uuid = "TPERF-ECS-TXT1-4000-8000-000000000006",
                        components = {
                            {
                                type = "UIText",
                                text = "MODE: ECS\nSpawning...",
                                color = Color(0.85, 0.95, 1.00, 1.00),
                                fontSize = 22.00,
                                alignment = "Left",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.00, 1.00),
                                anchorMax = Vector2(0.00, 1.00),
                                pivot = Vector2(0.00, 1.00),
                                anchoredPosition = Vector2(24.00, -231.00),
                                sizeDelta = Vector2(520.00, 220.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "TitleText",
                        uuid = "TPERF-ECS-TTL1-4000-8000-000000000007",
                        components = {
                            {
                                type = "UIText",
                                text = "Swarm Perf — ECS (dense World systems)",
                                color = Color(0.45, 0.85, 1.00, 1.00),
                                fontSize = 26.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 1.00),
                                anchorMax = Vector2(0.50, 1.00),
                                pivot = Vector2(0.50, 1.00),
                                anchoredPosition = Vector2(0.00, -18.00),
                                sizeDelta = Vector2(900.00, 40.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                }
            },
        }
    }
end
