function CreateScene()
    return {
        name = "MultiplayerMenu",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "MP-CAM0-0001-4000-8000-000000000001",
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
                        isMainCamera = true
                    },
                }
            },
            {
                name = "Directional Light",
                uuid = "MP-LGT0-0001-4000-8000-000000000001",
                rotation = Quaternion.FromEulerAngles(44.17, -31.37, 9.65),
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
                uuid = "MP-CVS0-0001-4000-8000-000000000001",
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
                    {
                        type = "MultiplayerMenuController",
                        lanLobbyButton = "MP-BTN0-0001-4000-8000-000000000001/UIButton",
                        onlineLobbyButton = "MP-BTN0-0002-4000-8000-000000000002/UIButton",
                        backButton = "MP-BTN0-0003-4000-8000-000000000003/UIButton",
                        lobbyScenePath = "Assets/Scenes/LobbyScene.lua",
                        mainMenuScenePath = "Assets/Scenes/MainMenu.lua"
                    },
                },
                children = {
                    {
                        name = "Background",
                        uuid = "MP-BKG0-0001-4000-8000-000000000001",
                        components = {
                            {
                                type = "UIImage",
                                texture = "Assets/UI/GuildSlate/bg_main_menu.png",
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
                        name = "Title",
                        uuid = "MP-TTL0-0001-4000-8000-000000000001",
                        components = {
                            {
                                type = "UIText",
                                text = "Multiplayer",
                                color = Color(0.91, 0.66, 0.29, 1.00),
                                fontSize = 42.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 160.00),
                                sizeDelta = Vector2(560.00, 58.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "LanLobbyButton",
                        uuid = "MP-BTN0-0001-4000-8000-000000000001",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.20, 0.20, 0.20, 0.00),
                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                borderThickness = 0.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 44.00),
                                sizeDelta = Vector2(320.00, 68.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIButton",
                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                hoveredColor = Color(1.00, 1.00, 1.00, 0.00),
                                pressedColor = Color(1.00, 1.00, 1.00, 0.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                interactable = true,
                                enableDefaultHoverVisuals = false
                            },
                            {
                                type = "ButtonStyle",
                                backgroundImage = "MP-LAN0-0001-4000-8000-000000000001/UIImage",
                                label = "MP-LAN0-0002-4000-8000-000000000001/UIText",
                                normalTexture = "Assets/UI/GuildSlate/btn_accent_normal.png",
                                hoverTexture = "Assets/UI/GuildSlate/btn_accent_hover.png",
                                pressedTexture = "Assets/UI/GuildSlate/btn_accent_pressed.png",
                                normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverScaleBoost = 1.05,
                                hoverRotationDeg = 0.00,
                                clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                clickScaleBoost = 0.97,
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12
                            },
                        },
                        children = {
                            {
                                name = "Background",
                                uuid = "MP-LAN0-0001-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Assets/UI/GuildSlate/btn_accent_normal.png",
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
                                name = "Text",
                                uuid = "MP-LAN0-0002-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "LAN Lobby",
                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                        fontSize = 22.00,
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
                        name = "OnlineLobbyButton",
                        uuid = "MP-BTN0-0002-4000-8000-000000000002",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.20, 0.20, 0.20, 0.00),
                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                borderThickness = 0.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, -32.00),
                                sizeDelta = Vector2(320.00, 68.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIButton",
                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                hoveredColor = Color(1.00, 1.00, 1.00, 0.00),
                                pressedColor = Color(1.00, 1.00, 1.00, 0.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                interactable = true,
                                enableDefaultHoverVisuals = false
                            },
                            {
                                type = "ButtonStyle",
                                backgroundImage = "MP-ONL0-0001-4000-8000-000000000001/UIImage",
                                label = "MP-ONL0-0002-4000-8000-000000000001/UIText",
                                normalTexture = "Assets/UI/GuildSlate/btn_accent_normal.png",
                                hoverTexture = "Assets/UI/GuildSlate/btn_accent_hover.png",
                                pressedTexture = "Assets/UI/GuildSlate/btn_accent_pressed.png",
                                normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverScaleBoost = 1.05,
                                hoverRotationDeg = 0.00,
                                clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                clickScaleBoost = 0.97,
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12
                            },
                        },
                        children = {
                            {
                                name = "Background",
                                uuid = "MP-ONL0-0001-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Assets/UI/GuildSlate/btn_accent_normal.png",
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
                                name = "Text",
                                uuid = "MP-ONL0-0002-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Online Lobby",
                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                        fontSize = 22.00,
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
                        name = "BackButton",
                        uuid = "MP-BTN0-0003-4000-8000-000000000003",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.20, 0.20, 0.20, 0.00),
                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                borderThickness = 0.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, -108.00),
                                sizeDelta = Vector2(320.00, 68.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIButton",
                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                hoveredColor = Color(1.00, 1.00, 1.00, 0.00),
                                pressedColor = Color(1.00, 1.00, 1.00, 0.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                interactable = true,
                                enableDefaultHoverVisuals = false
                            },
                            {
                                type = "ButtonStyle",
                                backgroundImage = "MP-BAK0-0001-4000-8000-000000000001/UIImage",
                                label = "MP-BAK0-0002-4000-8000-000000000001/UIText",
                                normalTexture = "Assets/UI/GuildSlate/btn_secondary_normal.png",
                                hoverTexture = "Assets/UI/GuildSlate/btn_secondary_hover.png",
                                pressedTexture = "Assets/UI/GuildSlate/btn_secondary_pressed.png",
                                normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverScaleBoost = 1.05,
                                hoverRotationDeg = 0.00,
                                clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                clickScaleBoost = 0.97,
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12
                            },
                        },
                        children = {
                            {
                                name = "Background",
                                uuid = "MP-BAK0-0001-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Assets/UI/GuildSlate/btn_secondary_normal.png",
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
                                name = "Text",
                                uuid = "MP-BAK0-0002-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Back",
                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                        fontSize = 22.00,
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
