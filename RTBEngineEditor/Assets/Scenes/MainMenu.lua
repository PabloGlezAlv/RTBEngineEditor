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
                rotation = Quaternion.FromEulerAngles(42.36, -34.02, -16.87),
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
                        sortOrder = 0,
                        faceCamera = false
                    },
                    {
                        type = "AudioSourceComponent",
                        clip = "Assets/Audio/UI/button_click.mp3",
                        volume = 0.85,
                        playOnStart = false
                    },
                    {
                        type = "CursorUnlocker"
                    },
                    {
                        type = "MainMenuController",
                        playerNameInput = "8A6DDA41-9A67-4B95-A2B6-E2159C420101/UIInputField",
                        playButton = "2E0ED540-A92F-40A9-B2E7-4906659B3E6E/UIButton",
                        multiplayerButton = "A1B2C3D4-E5F6-4789-A012-3456789ABCDE/UIButton",
                        statusMessageText = "MM-STATUS-0001-4000-8000-000000000001/UIText",
                        gameScenePath = "Assets/Scenes/DefaultScene.lua",
                        multiplayerMenuScenePath = "Assets/Scenes/MultiplayerMenu.lua"
                    },
                },
                children = {
                    {
                        name = "Background",
                        uuid = "9E8BE53F-0896-4AEF-B348-7CF8F3C8F881",
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
                        name = "StatusMessage",
                        uuid = "MM-STATUS-0001-4000-8000-000000000001",
                        components = {
                            {
                                type = "UIText",
                                text = "",
                                color = Color(1.00, 0.55, 0.45, 1.00),
                                fontSize = 18.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = false,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 168.00),
                                sizeDelta = Vector2(520.00, 48.00),
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
                                color = Color(0.96, 0.94, 0.88, 1.00),
                                fontSize = 18.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 166.00),
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
                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                borderColor = Color(0.91, 0.66, 0.29, 0.65),
                                borderThickness = 1.00,
                                hasBorder = true,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 118.00),
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
                                textColor = Color(0.96, 0.94, 0.88, 1.00),
                                placeholderColor = Color(0.62, 0.60, 0.56, 1.00),
                                normalColor = Color(0.12, 0.14, 0.18, 0.92),
                                focusedColor = Color(0.18, 0.22, 0.28, 1.00),
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
                                        color = Color(0.62, 0.60, 0.56, 1.00),
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
                                clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent",
                                backgroundImage = "E8F1A2B3-C4D5-4E6F-9012-ABCDEF010101/UIImage",
                                label = "6C243A1E-F469-47BE-9CB9-7DAC1471F31F/UIText",
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
                                disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12
                            },
                        },
                        children = {
                            {
                                name = "Background",
                                uuid = "E8F1A2B3-C4D5-4E6F-9012-ABCDEF010101",
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
                                uuid = "6C243A1E-F469-47BE-9CB9-7DAC1471F31F",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Play",
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
                        name = "MultiplayerButton",
                        uuid = "A1B2C3D4-E5F6-4789-A012-3456789ABCDE",
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
                                clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent",
                                backgroundImage = "MPBG01-2345-6789-ABCD-EF0123456701/UIImage",
                                label = "MPLBL01-2345-6789-ABCD-EF0123456701/UIText",
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
                                disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12
                            },
                        },
                        children = {
                            {
                                name = "Background",
                                uuid = "MPBG01-2345-6789-ABCD-EF0123456701",
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
                                uuid = "MPLBL01-2345-6789-ABCD-EF0123456701",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Multiplayer",
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
                        name = "Button (1)",
                        uuid = "6F093F7A-8C5E-47AE-B7DA-84C929A60009",
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
                                clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent",
                                backgroundImage = "E8F1A2B3-C4D5-4E6F-9012-ABCDEF010102/UIImage",
                                label = "6C4B352B-BAFB-4A6C-806C-2E24DB7CB30B/UIText",
                                normalTexture = "Assets/UI/GuildSlate/btn_danger_normal.png",
                                hoverTexture = "Assets/UI/GuildSlate/btn_danger_hover.png",
                                pressedTexture = "Assets/UI/GuildSlate/btn_danger_pressed.png",
                                normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverScaleBoost = 1.05,
                                hoverRotationDeg = 0.00,
                                clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                clickScaleBoost = 0.97,
                                disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
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
                                name = "Background",
                                uuid = "E8F1A2B3-C4D5-4E6F-9012-ABCDEF010102",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Assets/UI/GuildSlate/btn_danger_normal.png",
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
                                uuid = "6C4B352B-BAFB-4A6C-806C-2E24DB7CB30B",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Exit",
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
