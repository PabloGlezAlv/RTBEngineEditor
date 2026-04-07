function CreateScene()
    return {
        name = "MainMenu",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "198966E8-23E3-4EC4-9F05-BEB0810D1A43",
                position = Vector3(0.30, 1.00, -5.00),
                components = {
                    {
                        type = "CameraComponent",
                        fov = 60.00,
                        nearClip = 0.10,
                        farClip = 1000.00,
                        projectionType = "Perspective",
                        orthographicSize = 10.00,
                        syncWithTransform = true,
                        isMainCamera = false
                    },
                    {
                        type = "FreeLookCamera",
                        moveSpeed = 5.00,
                        lookSpeed = 0.10,
                        rotationSpeed = 90.00
                    },
                }
            },
            {
                name = "Directional Light",
                uuid = "48103FD7-E1F1-4AAB-8127-C0C2ED5B4111",
                rotation = Quaternion.FromEulerAngles(-37.27, -39.69, 27.31),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        color = Color(1.00, 1.00, 1.00, 1.00),
                        intensity = 1.00,
                        range = 10.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        syncPosition = true,
                        syncDirection = true
                    },
                }
            },
            {
                name = "Canvas",
                uuid = "073830E4-5E61-4968-B33A-433B8A60D77C",
                components = {
                    {
                        type = "Canvas"
                    },
                },
                children = {
                    {
                        name = "Button",
                        uuid = "2E0ED540-A92F-40A9-B2E7-4906659B3E6E",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(1.00, 1.00, 1.00, 0.50),
                                borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                borderThickness = 1.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 0.00),
                                sizeDelta = Vector2(160.00, 40.00)
                            },
                            {
                                type = "UIButton",
                                normalColor = Color(1.00, 1.00, 1.00, 0.23),
                                hoveredColor = Color(0.90, 0.90, 0.90, 0.44),
                                pressedColor = Color(0.70, 0.70, 0.70, 1.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.50),
                                interactable = true,
                                enableDefaultHoverVisuals = false
                            },
                            {
                                type = "ButtonStyle",
                                backgroundPanel = "2E0ED540-A92F-40A9-B2E7-4906659B3E6E/UIPanel",
                                label = "6C243A1E-F469-47BE-9CB9-7DAC1471F31F/UIText",
                                normalPanelColor = Color(0.20, 0.20, 0.20, 1.00),
                                normalTextColor = Color(1.00, 1.00, 1.00, 1.00),
                                hoverPanelColor = Color(0.35, 0.35, 0.65, 1.00),
                                hoverTextColor = Color(1.00, 1.00, 0.60, 1.00),
                                hoverScaleBoost = 1.06,
                                hoverRotationDeg = 0.00,
                                clickPanelColor = Color(0.15, 0.15, 0.45, 1.00),
                                clickTextColor = Color(0.86, 0.86, 0.92, 1.00),
                                clickScaleBoost = 0.96,
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12
                            },
                        },
                        children = {
                            {
                                name = "Text",
                                uuid = "6C243A1E-F469-47BE-9CB9-7DAC1471F31F",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Play",
                                        color = Color(1.00, 1.00, 1.00, 1.00),
                                        fontSize = 16.00,
                                        alignment = "Center",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.00, 0.00),
                                        anchorMax = Vector2(1.00, 1.00),
                                        anchoredPosition = Vector2(0.00, 0.00),
                                        sizeDelta = Vector2(0.00, 0.00)
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "Button (1)",
                        uuid = "6F093F7A-8C5E-47AE-B7DA-84C929A60009",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(1.00, 1.00, 1.00, 0.50),
                                borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                borderThickness = 1.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, -78.00),
                                sizeDelta = Vector2(160.00, 40.00)
                            },
                            {
                                type = "UIButton",
                                normalColor = Color(1.00, 1.00, 1.00, 0.23),
                                hoveredColor = Color(0.90, 0.90, 0.90, 0.44),
                                pressedColor = Color(0.70, 0.70, 0.70, 1.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.50),
                                interactable = true,
                                enableDefaultHoverVisuals = true
                            },
                        },
                        children = {
                            {
                                name = "Text",
                                uuid = "6C4B352B-BAFB-4A6C-806C-2E24DB7CB30B",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Exit",
                                        color = Color(1.00, 1.00, 1.00, 1.00),
                                        fontSize = 16.00,
                                        alignment = "Center",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.00, 0.00),
                                        anchorMax = Vector2(1.00, 1.00),
                                        anchoredPosition = Vector2(0.00, 0.00),
                                        sizeDelta = Vector2(0.00, 0.00)
                                    },
                                }
                            },
                        }
                    },
                }
            },
        }
    }
end
