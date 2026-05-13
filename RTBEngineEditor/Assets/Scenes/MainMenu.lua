function CreateScene()
    return {
        name = "MainMenu",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "198966E8-23E3-4EC4-9F05-BEB0810D1A43",
                position = Vector3(1.10, 0.99, -12.80),
                rotation = Quaternion.FromEulerAngles(0.00, -121.41, -0.00),
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
                rotation = Quaternion.FromEulerAngles(29.95, -45.03, 35.31),
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
                        type = "Canvas",
                        renderMode = "ScreenSpaceOverlay",
                        canvasSize = Vector2(1920.00, 1080.00),
                        pixelsPerUnit = 100.00,
                        sortOrder = 0
                    },
                    {
                        type = "CursorUnlocker"
                    },
                },
                children = {
                    {
                        name = "Background",
                        uuid = "9E8BE53F-0896-4AEF-B348-7CF8F3C8F881",
                        components = {
                            {
                                type = "UIImage",
                                texture = "Assets/UI/MainMenuBG.png",
                                tintColor = Color(1.00, 1.00, 1.00, 1.00),
                                preserveAspect = false,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.00, 0.00),
                                anchorMax = Vector2(1.00, 1.00),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 0.00),
                                sizeDelta = Vector2(0.00, 0.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "PlayerNameLabel",
                        uuid = "A80E2F9B-2D3A-4A38-BE1D-7D4A2C780101",
                        components = {
                            {
                                type = "UIText",
                                text = "Player Name",
                                color = Color(1.00, 1.00, 1.00, 1.00),
                                fontSize = 18.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 124.00),
                                sizeDelta = Vector2(260.00, 28.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "PlayerNameInputField",
                        uuid = "8A6DDA41-9A67-4B95-A2B6-E2159C420101",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.08, 0.08, 0.10, 0.92),
                                borderColor = Color(1.00, 1.00, 1.00, 0.55),
                                borderThickness = 1.00,
                                hasBorder = true,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 82.00),
                                sizeDelta = Vector2(260.00, 42.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIInputField",
                                text = "",
                                placeholder = "Enter name",
                                maxLength = 24,
                                interactable = true,
                                textColor = Color(1.00, 1.00, 1.00, 1.00),
                                placeholderColor = Color(0.72, 0.72, 0.78, 1.00),
                                normalColor = Color(0.08, 0.08, 0.10, 0.92),
                                focusedColor = Color(0.15, 0.15, 0.45, 1.00),
                                disabledColor = Color(0.20, 0.20, 0.20, 0.50),
                                textComponent = "D68D1E33-2706-41F1-9F13-923EBD130101/UIText",
                                backgroundPanel = "8A6DDA41-9A67-4B95-A2B6-E2159C420101/UIPanel"
                            },
                        },
                        children = {
                            {
                                name = "Text",
                                uuid = "D68D1E33-2706-41F1-9F13-923EBD130101",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Enter name",
                                        color = Color(0.72, 0.72, 0.78, 1.00),
                                        fontSize = 16.00,
                                        alignment = "Left",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.00, 0.00),
                                        anchorMax = Vector2(1.00, 1.00),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(12.00, 0.00),
                                        sizeDelta = Vector2(-24.00, 0.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "Button",
                        uuid = "2E0ED540-A92F-40A9-B2E7-4906659B3E6E",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.20, 0.20, 0.20, 1.00),
                                borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                borderThickness = 1.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 0.00),
                                sizeDelta = Vector2(160.00, 40.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
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
                            {
                                type = "SceneChangeButton",
                                scenePath = "Assets/Scenes/LobbyScene.lua"
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
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(0.00, 0.00),
                                        sizeDelta = Vector2(0.00, 0.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
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
                                backgroundColor = Color(0.20, 0.20, 0.20, 1.00),
                                borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                borderThickness = 1.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, -78.00),
                                sizeDelta = Vector2(160.00, 40.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
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
                                backgroundPanel = "6F093F7A-8C5E-47AE-B7DA-84C929A60009/UIPanel",
                                label = "6C4B352B-BAFB-4A6C-806C-2E24DB7CB30B/UIText",
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
                            {
                                type = "ApplicationQuitButton"
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
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(0.00, 0.00),
                                        sizeDelta = Vector2(0.00, 0.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
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
