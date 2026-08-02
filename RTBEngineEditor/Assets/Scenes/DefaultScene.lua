function CreateScene()
    return {
        name = "Test Scene",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "MainLight",
                uuid = "63347DB4-BEBE-4CCF-90C0-A270A9F7BF1E",
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        lightMode = "Realtime",
                        color = Color(1.00, 1.00, 1.00, 1.00),
                        intensity = 4.10,
                        range = 10.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        castShadows = true,
                        shadowBias = 0.00,
                        syncPosition = true,
                        syncDirection = true
                    },
                }
            },
            {
                name = "Fog Volume",
                uuid = "A1B2C3D4-E5F6-7890-ABCD-EF1234567890",
                components = {
                    {
                        type = "VolumeComponent"
                    },
                }
            },
            {
                name = "UICanvas",
                uuid = "62E6AE07-7DC2-42B3-8F0E-AA7CA40264F0",
                components = {
                    {
                        type = "Canvas",
                        renderMode = "ScreenSpaceOverlay",
                        canvasSize = Vector2(1920.00, 1080.00),
                        pixelsPerUnit = 100.00,
                        sortOrder = 0,
                        faceCamera = false,
                        faceCameraLockY = false
                    },
                    {
                        type = "AudioSourceComponent",
                        volume = 0.85,
                        pitch = 1.00,
                        loop = false,
                        playOnStart = false,
                        audioClip = "Assets/Audio/UI/button_click.mp3"
                    },
                    {
                        type = "RoundUIHandler",
                        roundText = "D7D85F5A-5A9A-4E6A-8AA2-9032EC0F0B11/UIText",
                        countdownText = "A5C9F5F4-2D0B-4D2C-9FE5-443A683E1A22/UIText",
                        roundPrefix = "Round ",
                        countdownPrefix = "Start in ",
                        endGameCountdownPrefix = "Game over in ",
                        respawnCountdownPrefix = "Revive in "
                    },
                    {
                        type = "PauseMenuController",
                        menuRoot = "F0B5D1DA-3CF0-4563-88C2-4731F9E91A10",
                        resumeButton = "D51F4600-5E83-4B08-8402-D0F6189B936A/UIButton",
                        exitButton = "9E2C0BE0-4742-4279-A2DD-0C7009566B7B/UIButton",
                        notificationText = "MATCH-NOTIF-0001-4000-8000-000000000001/UIText",
                        pauseSimulation = false,
                        useRelativeMouseWhenClosed = false,
                        mainMenuScenePath = "Assets/Scenes/MainMenu.lua"
                    },
                },
                children = {
                    {
                        name = "RoundText",
                        uuid = "D7D85F5A-5A9A-4E6A-8AA2-9032EC0F0B11",
                        components = {
                            {
                                type = "UIText",
                                text = "",
                                color = Color(0.96, 0.94, 0.88, 1.00),
                                fontSize = 36.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 1.00),
                                anchorMax = Vector2(0.50, 1.00),
                                pivot = Vector2(0.50, 1.00),
                                anchoredPosition = Vector2(0.00, -50.00),
                                sizeDelta = Vector2(420.00, 48.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "MatchNotificationText",
                        uuid = "MATCH-NOTIF-0001-4000-8000-000000000001",
                        components = {
                            {
                                type = "UIText",
                                text = "",
                                color = Color(1.00, 0.82, 0.47, 1.00),
                                fontSize = 22.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = false,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.00),
                                anchorMax = Vector2(0.50, 0.00),
                                pivot = Vector2(0.50, 0.00),
                                anchoredPosition = Vector2(0.00, 120.00),
                                sizeDelta = Vector2(720.00, 40.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "CountdownText",
                        uuid = "A5C9F5F4-2D0B-4D2C-9FE5-443A683E1A22",
                        components = {
                            {
                                type = "UIText",
                                text = "",
                                color = Color(1.00, 0.82, 0.47, 1.00),
                                fontSize = 24.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 1.00),
                                anchorMax = Vector2(0.50, 1.00),
                                pivot = Vector2(0.50, 1.00),
                                anchoredPosition = Vector2(0.00, -159.00),
                                sizeDelta = Vector2(420.00, 36.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "AttackJoystick",
                        uuid = "B4C7E92D-5F22-43A9-8E3B-2A6D7D1A8C10",
                        components = {
                            {
                                type = "UIImage",
                                texture = "Default/UI/joystick bg.png",
                                tintColor = Color(1.00, 1.00, 1.00, 1.00),
                                preserveAspect = true,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(1.00, 0.00),
                                anchorMax = Vector2(1.00, 0.00),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(-140.00, 140.00),
                                sizeDelta = Vector2(180.00, 180.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIJoystick",
                                handleImage = "8D9E47A2-7C1F-4B6E-8F52-3DCAF6E98521/UIImage",
                                deadZone = 0.20,
                                maxDistance = 48.00,
                                interactable = true
                            },
                        },
                        children = {
                            {
                                name = "AttackJoystickHandle",
                                uuid = "8D9E47A2-7C1F-4B6E-8F52-3DCAF6E98521",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Default/UI/joystick.png",
                                        tintColor = Color(1.00, 1.00, 1.00, 1.00),
                                        preserveAspect = true,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 0.50),
                                        anchorMax = Vector2(0.50, 0.50),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(0.00, 0.00),
                                        sizeDelta = Vector2(84.00, 84.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "SpecialAttackJoystick",
                        uuid = "B5D8E93E-6F33-44BA-9F4C-3B7E8E2B9D11",
                        components = {
                            {
                                type = "UIImage",
                                texture = "Default/UI/joystick bg.png",
                                tintColor = Color(0.84, 1.00, 0.00, 1.00),
                                preserveAspect = true,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(1.00, 0.00),
                                anchorMax = Vector2(1.00, 0.00),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(-170.00, 340.00),
                                sizeDelta = Vector2(150.00, 150.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIJoystick",
                                handleImage = "9E0F58B3-8D2A-4C7F-9063-4EDBF7FA9632/UIImage",
                                deadZone = 0.20,
                                maxDistance = 40.00,
                                interactable = false
                            },
                        },
                        children = {
                            {
                                name = "SpecialAttackJoystickHandle",
                                uuid = "9E0F58B3-8D2A-4C7F-9063-4EDBF7FA9632",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Default/UI/joystick.png",
                                        tintColor = Color(0.89, 0.97, 0.34, 1.00),
                                        preserveAspect = true,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 0.50),
                                        anchorMax = Vector2(0.50, 0.50),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(0.00, 0.00),
                                        sizeDelta = Vector2(70.00, 70.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                },
                                children = {
                                    {
                                        name = "SpecialAttackIcon",
                                        uuid = "C2A14D70-1B9E-4F83-A551-6D8E9C0F1122",
                                        components = {
                                            {
                                                type = "UIImage",
                                                texture = "Assets/UI/special_attack_charged_icon.png",
                                                tintColor = Color(1.00, 1.00, 1.00, 1.00),
                                                preserveAspect = true,
                                                isVisible = false,
                                                raycastTarget = false,
                                                anchorMin = Vector2(0.50, 0.50),
                                                anchorMax = Vector2(0.50, 0.50),
                                                pivot = Vector2(0.50, 0.50),
                                                anchoredPosition = Vector2(0.00, 0.00),
                                                sizeDelta = Vector2(32.00, 32.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                        }
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "PauseMenu",
                        uuid = "F0B5D1DA-3CF0-4563-88C2-4731F9E91A10",
                        active = false,
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.00, 0.00, 0.00, 0.62),
                                borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                borderThickness = 1.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.00, 0.00),
                                anchorMax = Vector2(1.00, 1.00),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 0.00),
                                sizeDelta = Vector2(0.00, 0.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        },
                        children = {
                            {
                                name = "PausePanel",
                                uuid = "0D9B1028-C0DA-4736-A523-C1D53594E4E2",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.00, 0.00, 0.00, 0.00),
                                        borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                        borderThickness = 0.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
                                        anchorMin = Vector2(0.50, 0.50),
                                        anchorMax = Vector2(0.50, 0.50),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(0.00, 0.00),
                                        sizeDelta = Vector2(420.00, 300.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                },
                                children = {
                                    {
                                        name = "PanelFrame",
                                        uuid = "A9B8C7D6-E5F4-4321-8765-543210FEDCBA",
                                        components = {
                                            {
                                                type = "UIImage",
                                                texture = "Assets/UI/GuildSlate/panel_large.png",
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
                                        name = "PauseTitle",
                                        uuid = "B7E6A2A0-D3E6-450C-A7E2-9F43E52D8B74",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Game Menu",
                                                color = Color(1.00, 0.82, 0.47, 1.00),
                                                fontSize = 42.00,
                                                alignment = "Center",
                                                font = nil,
                                                isVisible = true,
                                                raycastTarget = false,
                                                anchorMin = Vector2(0.50, 1.00),
                                                anchorMax = Vector2(0.50, 1.00),
                                                pivot = Vector2(0.50, 1.00),
                                                anchoredPosition = Vector2(0.00, -75.00),
                                                sizeDelta = Vector2(360.00, 60.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                        }
                                    },
                                    {
                                        name = "ContinueButton",
                                        uuid = "D51F4600-5E83-4B08-8402-D0F6189B936A",
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
                                                anchoredPosition = Vector2(0.00, 18.00),
                                                sizeDelta = Vector2(260.00, 54.00),
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
                                                backgroundImage = "F1A2B3C4-D5E6-4789-A012-3456789AB201/UIImage",
                                                label = "5DE3585B-3426-421D-8F36-DBAF43123F50/UIText",
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
                                                pressOutTimeSec = 0.12,
                                                playClickSound = true,
                                                clickAudio = "62E6AE07-7DC2-42B3-8F0E-AA7CA40264F0/AudioSourceComponent"
                                            },
                                            {
                                                type = "ResumeGameButton",
                                                controller = "62E6AE07-7DC2-42B3-8F0E-AA7CA40264F0/PauseMenuController"
                                            },
                                        },
                                        children = {
                                            {
                                                name = "Background",
                                                uuid = "F1A2B3C4-D5E6-4789-A012-3456789AB201",
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
                                                uuid = "5DE3585B-3426-421D-8F36-DBAF43123F50",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Resume",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 20.00,
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
                                        name = "ExitButton",
                                        uuid = "9E2C0BE0-4742-4279-A2DD-0C7009566B7B",
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
                                                anchoredPosition = Vector2(0.00, -58.00),
                                                sizeDelta = Vector2(260.00, 54.00),
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
                                                backgroundImage = "F1A2B3C4-D5E6-4789-A012-3456789AB202/UIImage",
                                                label = "2F7C83F3-A334-41DA-BF4C-A4FDDE59BA6C/UIText",
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
                                                disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                                disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
                                                hoverInTimeSec = 0.12,
                                                hoverOutTimeSec = 0.18,
                                                pressInTimeSec = 0.08,
                                                pressOutTimeSec = 0.12,
                                                playClickSound = true,
                                                clickAudio = "62E6AE07-7DC2-42B3-8F0E-AA7CA40264F0/AudioSourceComponent"
                                            },
                                            {
                                                type = "SceneChangeButton",
                                                scenePath = "Assets/Scenes/MainMenu.lua"
                                            },
                                        },
                                        children = {
                                            {
                                                name = "Background",
                                                uuid = "F1A2B3C4-D5E6-4789-A012-3456789AB202",
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
                                                uuid = "2F7C83F3-A334-41DA-BF4C-A4FDDE59BA6C",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Exit",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 20.00,
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
                    },
                }
            },
            {
                name = "RoundManager",
                uuid = "4A7C26F9-7F62-4A3A-B8F0-91A4B36C1601",
                components = {
                    {
                        type = "RoundManager",
                        playerObject = "F86013BD-FF24-4672-91B3-9411CDF272AE",
                        onlinePlayerManager = "B1A2C3D4-E5F6-7890-ABCD-EF1234567891/OnlinePlayerManager",
                        uiHandler = "62E6AE07-7DC2-42B3-8F0E-AA7CA40264F0/RoundUIHandler",
                        roundCountdownDuration = 5.00,
                        baseEnemiesPerRound = 2,
                        additionalEnemiesPerRound = 1,
                        winningRound = 5,
                        playerRespawnDelay = 30.00,
                        teamWipeSceneDelay = 5.00,
                        finalScenePath = "Assets/Scenes/FinalScene.lua",
                        enemyPrefabRef = "Assets/Prefabs/Enemies/Enemy Melee.prefab",
                        spawnPoints = {
                            "74B6BC8E-5892-4C38-A4B6-8AAAF7B1F001",
                            "74B6BC8E-5892-4C38-A4B6-8AAAF7B1F002",
                            "74B6BC8E-5892-4C38-A4B6-8AAAF7B1F003"                        }
                    },
                },
                children = {
                    {
                        name = "EnemySpawnPointLeft",
                        uuid = "74B6BC8E-5892-4C38-A4B6-8AAAF7B1F001",
                        position = Vector3(-11.80, 0.00, 8.60),
                        rotation = Quaternion.FromEulerAngles(0.00, 135.00, 0.00),
                    },
                    {
                        name = "EnemySpawnPointRight",
                        uuid = "74B6BC8E-5892-4C38-A4B6-8AAAF7B1F002",
                        position = Vector3(11.90, 0.00, 9.60),
                        rotation = Quaternion.FromEulerAngles(0.00, -135.00, 0.00),
                    },
                    {
                        name = "EnemySpawnPointRear",
                        uuid = "74B6BC8E-5892-4C38-A4B6-8AAAF7B1F003",
                        position = Vector3(0.00, 0.00, -11.80),
                    },
                }
            },
            {
                name = "Navigation",
                uuid = "A1B2C3D4-E5F6-4789-A012-3456789ABCDE",
                position = Vector3(0.00, 0.38, 0.00),
                components = {
                    {
                        type = "NavGridComponent",
                        origin = Vector3(-16.00, 0.00, -16.00),
                        size = Vector3(32.00, 0.00, 32.00),
                        cellSize = 0.50,
                        agentRadius = 0.40,
                        groundProbeHeight = 4.00
                    },
                }
            },
            {
                name = "Barbarian",
                uuid = "753770D2-B00E-4859-B7D3-5ABF1249B014",
                active = false,
                collisionLayer = "Characters",
                prefab = "Enemy Melee",
                position = Vector3(-3.00, 0.00, 0.00),
                overrides = {
                    components = {
                        { type = "EnemyMeleeAI",
                            health = "753770D2-B00E-4859-B7D3-5ABF1249B014/HealthComponent",
                            targetTracker = "753770D2-B00E-4859-B7D3-5ABF1249B014/EnemyTargetTracker",
                            animationDriver = "753770D2-B00E-4859-B7D3-5ABF1249B014/EnemyAnimationDriver",
                            locomotion = "753770D2-B00E-4859-B7D3-5ABF1249B014/EnemyLocomotionController",
                            navAgent = "753770D2-B00E-4859-B7D3-5ABF1249B014/NavAgentComponent",
                            meleeAttack = "753770D2-B00E-4859-B7D3-5ABF1249B014/MeleeSphereAttackAbility",
                        },
                    },
                },
            },
            {
                name = "OnlinePlayers",
                uuid = "B1A2C3D4-E5F6-7890-ABCD-EF1234567891",
                components = {
                    {
                        type = "OnlinePlayerManager",
                        localPlayerObject = "F86013BD-FF24-4672-91B3-9411CDF272AE",
                        roundManager = "4A7C26F9-7F62-4A3A-B8F0-91A4B36C1601/RoundManager",
                        remoteSpawnOffsetX = 0.50
                    },
                }
            },
            {
                name = "PlayerSpawnPoint",
                uuid = "PLSP-AWN0-4000-8000-000000000001",
                active = false,
                collisionLayer = "Characters",
                position = Vector3(0.90, 0.10, 0.00),
                components = {
                    {
                        type = "PlayerPawnSpawner",
                        onlinePlayerManager = "B1A2C3D4-E5F6-7890-ABCD-EF1234567891/OnlinePlayerManager",
                        roundManager = "4A7C26F9-7F62-4A3A-B8F0-91A4B36C1601/RoundManager",
                        attackJoystick = "B4C7E92D-5F22-43A9-8E3B-2A6D7D1A8C10/UIJoystick",
                        specialAttackJoystick = "B5D8E93E-6F33-44BA-9F4C-3B7E8E2B9D11/UIJoystick",
                        specialAttackReadyIcon = "C2A14D70-1B9E-4F83-A551-6D8E9C0F1122/UIImage"
                    },
                }
            },
            {
                name = "Dungeon Environment",
                uuid = "E0F367FE-C84E-4D38-922E-C3056F32D517",
                children = {
                    {
                        name = "Floor",
                        uuid = "89068B52-7B3B-41ED-A1BA-6CB77C18253F",
                        children = {
                            {
                                name = "floor_0_0",
                                uuid = "BAEA53F1-EE9B-426F-BF3A-2B8E46DEA75F",
                                staticFlags = 15,
                                position = Vector3(-12.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                }
                            },
                            {
                                name = "floor_0_1",
                                uuid = "373F6DAA-77A0-4DBE-8AAE-106E5F637054",
                                staticFlags = 15,
                                position = Vector3(-12.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_0_2",
                                uuid = "0C8C1E97-3C29-4596-AD51-0B49675E5918",
                                staticFlags = 15,
                                position = Vector3(-12.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_0_3",
                                uuid = "9013FCC1-1CAA-42E7-B972-F0EA6B70D08C",
                                staticFlags = 15,
                                position = Vector3(-12.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_0_4",
                                uuid = "BAC4304E-5BCC-45A2-86A1-056B650B8D59",
                                staticFlags = 15,
                                position = Vector3(-12.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_0_5",
                                uuid = "81C5D6B2-E45A-45EE-B407-EBDCAA27F3FA",
                                staticFlags = 15,
                                position = Vector3(-12.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_0_6",
                                uuid = "E8F93FCC-B04E-48E5-BBFF-4318114042C1",
                                staticFlags = 15,
                                position = Vector3(-12.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_1_0",
                                uuid = "4440970E-BF57-4C27-8C34-675DB7DA3A8D",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_1_1",
                                uuid = "38D8EE45-74AD-465F-954D-9ACEB8757879",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_1_2",
                                uuid = "7776A29A-1F51-47F2-B4DE-3DB9C6D306E7",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_1_3",
                                uuid = "C247868E-ACCF-436C-A635-1D4256F6F88A",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_1_4",
                                uuid = "B3C73C07-4FDE-4F57-B4C1-848CAE823208",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_1_5",
                                uuid = "41BD2009-7E07-466A-8AF3-EFCD7CEDF420",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_1_6",
                                uuid = "8C13092C-317F-4CE0-91D4-A648C806BCC3",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_2_0",
                                uuid = "EE4411B6-346F-44E8-AFEB-B095E172524B",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_2_1",
                                uuid = "044D17FC-5E26-4EFE-9E7D-CA59A7757B4E",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_2_2",
                                uuid = "0CA1FC12-AB95-427D-895D-7003F630B802",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_2_3",
                                uuid = "5245392A-F659-4FAF-86AD-37A7E97D23B9",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_2_4",
                                uuid = "5F5C7685-9DB9-4C29-9090-3EFC3792E227",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_2_5",
                                uuid = "122924A8-C5F7-4ABD-BCF9-F4B01E8447B3",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_2_6",
                                uuid = "37038BF3-A260-4484-8237-C5E631E6572B",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_3_0",
                                uuid = "130EB721-F2AC-4B5A-BC47-DA1E6E27FC64",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_3_1",
                                uuid = "6477674A-88D6-42D9-B024-7D258E664EE6",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_3_2",
                                uuid = "7A025432-BC9C-4320-ACA0-1C6E067A89E6",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_3_3",
                                uuid = "27F6173C-B853-4E18-9778-8B8D3C969627",
                                staticFlags = 15,
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_3_4",
                                uuid = "CE0473C3-8186-45DE-B66E-652F4DBD42AF",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_3_5",
                                uuid = "7162474A-D06C-4B1E-86E0-E4528CD383A4",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_3_6",
                                uuid = "0568C99E-CC97-4E21-81BE-6C9D44EBC630",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_4_0",
                                uuid = "5AAD091C-8678-42FA-824F-DADA482E8DDB",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_4_1",
                                uuid = "1DEEEE2C-9DC5-4E92-B65C-6CC54772F93E",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_4_2",
                                uuid = "C9F06DA0-4B25-4B68-A8D2-012A4831C0B5",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_4_3",
                                uuid = "06E4B05E-8F5D-4692-BD0F-EB46E667201C",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_4_4",
                                uuid = "819F8EB7-0F7E-410B-9E8C-55C384DD6625",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_4_5",
                                uuid = "DFE0B4EB-93BC-48D9-9EBF-99041802F517",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_4_6",
                                uuid = "D2DB5265-9439-4FCF-B1A0-BD47AF8678AC",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_5_0",
                                uuid = "269B51EF-2CB3-4C75-B1A3-7E5E0B8F9DCE",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_5_1",
                                uuid = "5EE94DF0-8626-4E6F-90F9-8D5BB179268F",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_5_2",
                                uuid = "703B6EF6-3ED1-4FDA-AB03-F67921A3C2A2",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_5_3",
                                uuid = "36EFED1C-D206-4AAB-9F6A-F08CC3BFBDFB",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_5_4",
                                uuid = "112C5D07-AA5A-4CF1-A7DF-8C7128DD42E8",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_5_5",
                                uuid = "F71F7937-0F1C-45F9-B1F8-46EE9C1F4758",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_5_6",
                                uuid = "5C4E2379-BDFF-4D74-B31F-DA947AA52AD6",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_6_0",
                                uuid = "7D0185BA-22D3-4D66-8B80-49D2B2E1D478",
                                staticFlags = 15,
                                position = Vector3(12.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_6_1",
                                uuid = "CE2B4EB7-1EA1-48AB-8F4F-B1AEF01DE90A",
                                staticFlags = 15,
                                position = Vector3(12.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_6_2",
                                uuid = "ED70A603-6AA2-422D-A143-DA8BFBB921B8",
                                staticFlags = 15,
                                position = Vector3(12.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_6_3",
                                uuid = "7ACD5A2D-C54C-4CE6-B643-BA08EBC45D42",
                                staticFlags = 15,
                                position = Vector3(12.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_6_4",
                                uuid = "27458B34-3C2A-4FBD-8427-6943AE40E77F",
                                staticFlags = 15,
                                position = Vector3(12.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_6_5",
                                uuid = "5A4E9546-C36B-49F2-AFFD-5BE272C004E8",
                                staticFlags = 15,
                                position = Vector3(12.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "floor_6_6",
                                uuid = "D6D922B1-A313-4A1A-9DA7-6C705F7BAE31",
                                staticFlags = 15,
                                position = Vector3(12.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "Walls",
                        uuid = "EA5242DA-466B-40E8-B0BA-B73784275D6B",
                        components = {
                            {
                                type = "OcclusionFadeController",
                                occludedAlpha = 0.35,
                                fadeSpeed = 12.00,
                                boundsPadding = 0.15,
                                controllerEnabled = true
                            },
                        },
                        children = {
                            {
                                name = "wall_corner_sw",
                                uuid = "51456D84-CFC9-4403-ADFD-991F162D8A55",
                                staticFlags = 15,
                                position = Vector3(-14.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_corner_se",
                                uuid = "6A25C275-5AF3-47F1-BE30-F5C9AC12FF51",
                                staticFlags = 15,
                                position = Vector3(14.00, 0.00, -14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_corner_nw",
                                uuid = "AC312C13-90E5-4EB6-B8AA-F84BDA8EAAA4",
                                staticFlags = 15,
                                position = Vector3(-14.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_corner_ne",
                                uuid = "2511373C-D1A9-4183-AE10-3CC6C9A44409",
                                staticFlags = 15,
                                position = Vector3(14.00, -0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_s_-12",
                                uuid = "EAC67787-ED33-4A94-BBD6-7A4E4D17B751",
                                staticFlags = 15,
                                position = Vector3(-12.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_s_-8",
                                uuid = "B148B637-C7CE-4600-B272-CAE4B69B2828",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_s_-4",
                                uuid = "50B285E6-7731-41E7-A827-EB6EBD93B360",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_s_0",
                                uuid = "1B342F65-A7DB-48CC-A901-737C6F707183",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_s_4",
                                uuid = "380A506B-66C1-4111-9597-F7F1DF53C36D",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_s_8",
                                uuid = "5A358223-286C-494D-96BE-C920CE050101",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_s_12",
                                uuid = "4948EF58-E846-46DB-8D6B-FFAA717C7A7D",
                                staticFlags = 15,
                                position = Vector3(12.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_n_-12",
                                uuid = "56BF061A-D1D6-441B-B6E6-26165AEED521",
                                staticFlags = 15,
                                position = Vector3(-12.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_n_-8",
                                uuid = "0475162D-E451-45F6-90B0-551038025002",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_n_-4",
                                uuid = "8987FEDF-75C9-4809-90E3-782CB9278901",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_n_0",
                                uuid = "013C2A45-B1B5-4645-A8AC-EE5824F94914",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_n_4",
                                uuid = "06B97D68-350C-4301-8DE6-C714191DBF3D",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_n_8",
                                uuid = "03EA9125-FCEF-4F65-A817-7C2697593C83",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_n_12",
                                uuid = "677F63A1-66E1-4D54-9A76-C7F8BC485349",
                                staticFlags = 15,
                                position = Vector3(12.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_w_-12",
                                uuid = "FFCE8380-314F-4B7A-8F0A-F7CA4A0EB764",
                                staticFlags = 15,
                                position = Vector3(-14.00, 0.00, -12.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_w_-8",
                                uuid = "FDC05E6A-B37C-4CC2-AD95-2C62A02D0208",
                                staticFlags = 15,
                                position = Vector3(-14.00, 0.00, -8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                }
                            },
                            {
                                name = "wall_w_-4",
                                uuid = "30F31E32-8336-4C8E-9FDC-138556427F9F",
                                staticFlags = 15,
                                position = Vector3(-14.00, 0.00, -4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_w_0",
                                uuid = "F6D118EA-AA4D-4EF0-84DA-8E2D4D0CE02C",
                                staticFlags = 15,
                                position = Vector3(-14.00, 0.00, 0.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_w_4",
                                uuid = "9C3DA1C3-ED09-4D0A-8859-91D07A4A6800",
                                staticFlags = 15,
                                position = Vector3(-14.00, 0.00, 4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_w_8",
                                uuid = "A1DE8989-C92C-454B-A00E-D266863ED1A1",
                                staticFlags = 15,
                                position = Vector3(-14.00, 0.00, 8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_w_12",
                                uuid = "EA8854E2-8B7E-4E90-B254-AAA1637964D1",
                                staticFlags = 15,
                                position = Vector3(-14.00, 0.00, 12.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_e_-12",
                                uuid = "98203F54-7FCD-4F50-825E-EBD3622B54AB",
                                staticFlags = 15,
                                position = Vector3(14.00, 0.00, -12.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_e_-8",
                                uuid = "9414038E-CB0E-4697-A2DE-634F713DA637",
                                staticFlags = 15,
                                position = Vector3(14.00, 0.00, -8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_e_-4",
                                uuid = "EF5A0C26-0C42-4DD6-9DB8-8652C739023B",
                                staticFlags = 15,
                                position = Vector3(14.00, 0.00, -4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_e_0",
                                uuid = "0F8E522C-6505-4685-994B-EEB538713BA4",
                                staticFlags = 15,
                                position = Vector3(14.00, 0.00, 0.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_e_4",
                                uuid = "BCDF12BF-1C14-408A-993C-8DDCFAA0C40D",
                                staticFlags = 15,
                                position = Vector3(14.00, 0.00, 4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_e_8",
                                uuid = "43353BFE-E38D-4B94-86E0-80CFA2E3B2BF",
                                staticFlags = 15,
                                position = Vector3(14.00, 0.00, 8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_e_12",
                                uuid = "6295F5FE-B512-45C5-BF73-26AA384D99CF",
                                staticFlags = 15,
                                position = Vector3(14.00, 0.00, 12.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_cross_x_-8",
                                uuid = "4CA2B8F1-6241-4078-8808-79AC0EE84EFF",
                                staticFlags = 15,
                                position = Vector3(-8.00, -0.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_cross_x_-4",
                                uuid = "AAB39B68-4D11-4A1E-BBF3-6100489EB069",
                                staticFlags = 15,
                                position = Vector3(-4.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_cross_x_4",
                                uuid = "A179C4AD-B4DC-4494-AA77-56A1C08079D2",
                                staticFlags = 15,
                                position = Vector3(4.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_cross_x_8",
                                uuid = "8B6B521C-B5D9-4CF5-BC95-07ED649B1A5B",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 8.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_cross_z_-8",
                                uuid = "1AA5EE48-DF80-4B63-BB29-01125831EC20",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, -8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_cross_z_-4",
                                uuid = "08793F79-4C23-4591-B729-9DF49D54A677",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, -4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_cross_z_4",
                                uuid = "A19F059D-BFBF-499E-B1E1-884D871B7589",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, 4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "wall_cross_z_8",
                                uuid = "A8196364-765A-4E93-BD0F-DFDA9F0C9045",
                                staticFlags = 15,
                                position = Vector3(0.00, 0.00, 8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "Props",
                        uuid = "ABECE040-0377-4657-8D7F-1CC25F71DA0B",
                        children = {
                            {
                                name = "pillar_ne",
                                uuid = "35955A5B-69B9-47D1-A206-6E5939707A70",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/pillar_decorated.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(1.70, 4.00, 1.70),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "pillar_nw",
                                uuid = "D9A58998-3BE6-4521-9F0D-1B5C12B560C0",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/pillar_decorated.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(1.70, 4.00, 1.70),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "pillar_se",
                                uuid = "85321929-ECE5-42CF-A91D-A8FECEA692DD",
                                staticFlags = 15,
                                position = Vector3(8.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/pillar_decorated.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(1.70, 4.00, 1.70),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "pillar_sw",
                                uuid = "8B3D94E4-C295-46FF-B9C2-19FD7C1594C5",
                                staticFlags = 15,
                                position = Vector3(-8.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "Occludable",
                                        occluderEnabled = true,
                                        boundsPadding = 0.00
                                    },
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/pillar_decorated.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(1.70, 4.00, 1.70),
                                        isTrigger = false
                                    },
                                    {
                                        type = "RigidBodyComponent",
                                        mass = 1.00,
                                        friction = 0.50,
                                        restitution = 0.00,
                                        freezeRotationX = false,
                                        freezeRotationY = false,
                                        freezeRotationZ = false,
                                        bodyType = "Static"
                                    },
                                }
                            },
                            {
                                name = "torch_s_1",
                                uuid = "4B55DAB7-3451-4E41-A5A0-853CF9DFDD42",
                                position = Vector3(-6.00, 0.00, -13.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/torch_lit.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                },
                                children = {
                                    {
                                        name = "FlameFX",
                                        uuid = "TORCH-FL1-0001-4000-8000-000000000001",
                                        position = Vector3(0.00, 0.42, 0.00),
                                        components = {
                                            {
                                                type = "LightComponent",
                                                lightType = "Point",
                                                lightMode = "Realtime",
                                                color = Color(1.00, 0.55, 0.20, 1.00),
                                                intensity = 2.50,
                                                range = 6.00,
                                                spotAngle = 45.00,
                                                spotInnerAngle = 30.00,
                                                castShadows = true,
                                                shadowBias = 0.00,
                                                syncPosition = true,
                                                syncDirection = true
                                            },
                                            {
                                                type = "LightFlicker",
                                                baseIntensity = 2.50,
                                                amplitude = 0.22,
                                                speed = 5.00
                                            },
                                        },
                                        children = {
                                            {
                                                name = "FlameBody",
                                                uuid = "TORCH-FL1-BODY-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "AnimatedBillboard",
                                                        textureRef = "Assets/VFX/fuegomultitextura.png",
                                                        color = Color(1.00, 0.72, 0.28, 0.95),
                                                        size = Vector2(0.48, 0.82),
                                                        verticalOffset = 0.18,
                                                        textureSheetColumns = 4,
                                                        textureSheetRows = 2,
                                                        textureSheetFrameCount = 8,
                                                        textureSheetFramesPerSecond = 14.00,
                                                        animationOffset = 0.00,
                                                        blendMode = "Additive",
                                                        visible = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        randomStartFrame = true
                                                    },
                                                }
                                            },
                                            {
                                                name = "FlameCore",
                                                uuid = "TORCH-FL1-CORE-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "AnimatedBillboard",
                                                        textureRef = "Assets/VFX/flame_soft.png",
                                                        color = Color(1.00, 0.92, 0.55, 0.85),
                                                        size = Vector2(0.20, 0.34),
                                                        verticalOffset = 0.08,
                                                        textureSheetColumns = 1,
                                                        textureSheetRows = 1,
                                                        textureSheetFrameCount = 1,
                                                        textureSheetFramesPerSecond = 0.00,
                                                        animationOffset = 0.00,
                                                        blendMode = "Additive",
                                                        visible = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        randomStartFrame = false
                                                    },
                                                }
                                            },
                                            {
                                                name = "Embers",
                                                uuid = "TORCH-FL1-EMBR-4000-8000-000000000001",
                                                position = Vector3(0.00, 0.10, 0.00),
                                                components = {
                                                    {
                                                        type = "ParticleSystem",
                                                        maxParticles = 16,
                                                        emissionRate = 8.00,
                                                        emitterShape = "Cone",
                                                        shapeRadius = 0.04,
                                                        coneAngle = 28.00,
                                                        boxSize = Vector3(1.00, 1.00, 1.00),
                                                        startLifetime = 0.70,
                                                        startSpeed = 1.40,
                                                        startSize = 0.08,
                                                        endSize = 0.02,
                                                        startColor = Color(1.00, 0.75, 0.25, 0.90),
                                                        endColor = Color(1.00, 0.20, 0.00, 0.00),
                                                        gravity = Vector3(0.00, -0.80, 0.00),
                                                        worldSimulation = false,
                                                        textureRef = "Assets/VFX/spark_soft.png",
                                                        visible = true,
                                                        loop = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        destroyOwnerWhenFinished = false,
                                                        burstCount = 1,
                                                        textureSheetColumns = 1,
                                                        textureSheetRows = 1,
                                                        textureSheetFrameCount = 1,
                                                        textureSheetFramesPerSecond = 0.00,
                                                        blendMode = "Additive"
                                                    },
                                                }
                                            },
                                            {
                                                name = "Smoke",
                                                uuid = "TORCH-FL1-SMOK-4000-8000-000000000001",
                                                position = Vector3(0.00, 0.35, 0.00),
                                                components = {
                                                    {
                                                        type = "ParticleSystem",
                                                        maxParticles = 12,
                                                        emissionRate = 3.50,
                                                        emitterShape = "Cone",
                                                        shapeRadius = 0.06,
                                                        coneAngle = 40.00,
                                                        boxSize = Vector3(1.00, 1.00, 1.00),
                                                        startLifetime = 1.80,
                                                        startSpeed = 0.55,
                                                        startSize = 0.35,
                                                        endSize = 0.85,
                                                        startColor = Color(0.55, 0.55, 0.55, 0.35),
                                                        endColor = Color(0.35, 0.35, 0.35, 0.00),
                                                        gravity = Vector3(0.00, 0.15, 0.00),
                                                        worldSimulation = false,
                                                        textureRef = "Assets/VFX/humomultitextura.png",
                                                        visible = true,
                                                        loop = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        destroyOwnerWhenFinished = false,
                                                        burstCount = 1,
                                                        textureSheetColumns = 4,
                                                        textureSheetRows = 2,
                                                        textureSheetFrameCount = 8,
                                                        textureSheetFramesPerSecond = 8.00,
                                                        blendMode = "Alpha"
                                                    },
                                                }
                                            },
                                        }
                                    },
                                }
                            },
                            {
                                name = "torch_s_2",
                                uuid = "9330E264-DC54-4AFD-9E3B-ED71228B208B",
                                position = Vector3(6.00, 0.00, -13.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/torch_lit.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                },
                                children = {
                                    {
                                        name = "FlameFX",
                                        uuid = "TORCH-FL2-0001-4000-8000-000000000001",
                                        position = Vector3(0.00, 0.42, 0.00),
                                        components = {
                                            {
                                                type = "LightComponent",
                                                lightType = "Point",
                                                lightMode = "Realtime",
                                                color = Color(1.00, 0.55, 0.20, 1.00),
                                                intensity = 2.50,
                                                range = 6.00,
                                                spotAngle = 45.00,
                                                spotInnerAngle = 30.00,
                                                castShadows = true,
                                                shadowBias = 0.00,
                                                syncPosition = true,
                                                syncDirection = true
                                            },
                                            {
                                                type = "LightFlicker",
                                                baseIntensity = 2.50,
                                                amplitude = 0.22,
                                                speed = 5.00
                                            },
                                        },
                                        children = {
                                            {
                                                name = "FlameBody",
                                                uuid = "TORCH-FL2-BODY-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "AnimatedBillboard",
                                                        textureRef = "Assets/VFX/fuegomultitextura.png",
                                                        color = Color(1.00, 0.72, 0.28, 0.95),
                                                        size = Vector2(0.48, 0.82),
                                                        verticalOffset = 0.18,
                                                        textureSheetColumns = 4,
                                                        textureSheetRows = 2,
                                                        textureSheetFrameCount = 8,
                                                        textureSheetFramesPerSecond = 14.00,
                                                        animationOffset = 0.00,
                                                        blendMode = "Additive",
                                                        visible = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        randomStartFrame = true
                                                    },
                                                }
                                            },
                                            {
                                                name = "FlameCore",
                                                uuid = "TORCH-FL2-CORE-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "AnimatedBillboard",
                                                        textureRef = "Assets/VFX/flame_soft.png",
                                                        color = Color(1.00, 0.92, 0.55, 0.85),
                                                        size = Vector2(0.20, 0.34),
                                                        verticalOffset = 0.08,
                                                        textureSheetColumns = 1,
                                                        textureSheetRows = 1,
                                                        textureSheetFrameCount = 1,
                                                        textureSheetFramesPerSecond = 0.00,
                                                        animationOffset = 0.00,
                                                        blendMode = "Additive",
                                                        visible = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        randomStartFrame = false
                                                    },
                                                }
                                            },
                                            {
                                                name = "Embers",
                                                uuid = "TORCH-FL2-EMBR-4000-8000-000000000001",
                                                position = Vector3(0.00, 0.10, 0.00),
                                                components = {
                                                    {
                                                        type = "ParticleSystem",
                                                        maxParticles = 16,
                                                        emissionRate = 8.00,
                                                        emitterShape = "Cone",
                                                        shapeRadius = 0.04,
                                                        coneAngle = 28.00,
                                                        boxSize = Vector3(1.00, 1.00, 1.00),
                                                        startLifetime = 0.70,
                                                        startSpeed = 1.40,
                                                        startSize = 0.08,
                                                        endSize = 0.02,
                                                        startColor = Color(1.00, 0.75, 0.25, 0.90),
                                                        endColor = Color(1.00, 0.20, 0.00, 0.00),
                                                        gravity = Vector3(0.00, -0.80, 0.00),
                                                        worldSimulation = false,
                                                        textureRef = "Assets/VFX/spark_soft.png",
                                                        visible = true,
                                                        loop = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        destroyOwnerWhenFinished = false,
                                                        burstCount = 1,
                                                        textureSheetColumns = 1,
                                                        textureSheetRows = 1,
                                                        textureSheetFrameCount = 1,
                                                        textureSheetFramesPerSecond = 0.00,
                                                        blendMode = "Additive"
                                                    },
                                                }
                                            },
                                            {
                                                name = "Smoke",
                                                uuid = "TORCH-FL2-SMOK-4000-8000-000000000001",
                                                position = Vector3(0.00, 0.35, 0.00),
                                                components = {
                                                    {
                                                        type = "ParticleSystem",
                                                        maxParticles = 12,
                                                        emissionRate = 3.50,
                                                        emitterShape = "Cone",
                                                        shapeRadius = 0.06,
                                                        coneAngle = 40.00,
                                                        boxSize = Vector3(1.00, 1.00, 1.00),
                                                        startLifetime = 1.80,
                                                        startSpeed = 0.55,
                                                        startSize = 0.35,
                                                        endSize = 0.85,
                                                        startColor = Color(0.55, 0.55, 0.55, 0.35),
                                                        endColor = Color(0.35, 0.35, 0.35, 0.00),
                                                        gravity = Vector3(0.00, 0.15, 0.00),
                                                        worldSimulation = false,
                                                        textureRef = "Assets/VFX/humomultitextura.png",
                                                        visible = true,
                                                        loop = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        destroyOwnerWhenFinished = false,
                                                        burstCount = 1,
                                                        textureSheetColumns = 4,
                                                        textureSheetRows = 2,
                                                        textureSheetFrameCount = 8,
                                                        textureSheetFramesPerSecond = 8.00,
                                                        blendMode = "Alpha"
                                                    },
                                                }
                                            },
                                        }
                                    },
                                }
                            },
                            {
                                name = "torch_n",
                                uuid = "31C7F376-69C7-4C51-B45C-57D2EBB9EA73",
                                position = Vector3(0.00, 0.00, 13.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/torch_lit.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                },
                                children = {
                                    {
                                        name = "FlameFX",
                                        uuid = "TORCH-FL3-0001-4000-8000-000000000001",
                                        position = Vector3(0.00, 0.42, 0.00),
                                        components = {
                                            {
                                                type = "LightComponent",
                                                lightType = "Point",
                                                lightMode = "Realtime",
                                                color = Color(1.00, 0.55, 0.20, 1.00),
                                                intensity = 2.50,
                                                range = 6.00,
                                                spotAngle = 45.00,
                                                spotInnerAngle = 30.00,
                                                castShadows = true,
                                                shadowBias = 0.00,
                                                syncPosition = true,
                                                syncDirection = true
                                            },
                                            {
                                                type = "LightFlicker",
                                                baseIntensity = 2.50,
                                                amplitude = 0.22,
                                                speed = 5.00
                                            },
                                        },
                                        children = {
                                            {
                                                name = "FlameBody",
                                                uuid = "TORCH-FL3-BODY-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "AnimatedBillboard",
                                                        textureRef = "Assets/VFX/fuegomultitextura.png",
                                                        color = Color(1.00, 0.72, 0.28, 0.95),
                                                        size = Vector2(0.48, 0.82),
                                                        verticalOffset = 0.18,
                                                        textureSheetColumns = 4,
                                                        textureSheetRows = 2,
                                                        textureSheetFrameCount = 8,
                                                        textureSheetFramesPerSecond = 14.00,
                                                        animationOffset = 0.00,
                                                        blendMode = "Additive",
                                                        visible = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        randomStartFrame = true
                                                    },
                                                }
                                            },
                                            {
                                                name = "FlameCore",
                                                uuid = "TORCH-FL3-CORE-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "AnimatedBillboard",
                                                        textureRef = "Assets/VFX/flame_soft.png",
                                                        color = Color(1.00, 0.92, 0.55, 0.85),
                                                        size = Vector2(0.20, 0.34),
                                                        verticalOffset = 0.08,
                                                        textureSheetColumns = 1,
                                                        textureSheetRows = 1,
                                                        textureSheetFrameCount = 1,
                                                        textureSheetFramesPerSecond = 0.00,
                                                        animationOffset = 0.00,
                                                        blendMode = "Additive",
                                                        visible = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        randomStartFrame = false
                                                    },
                                                }
                                            },
                                            {
                                                name = "Embers",
                                                uuid = "TORCH-FL3-EMBR-4000-8000-000000000001",
                                                position = Vector3(0.00, 0.10, 0.00),
                                                components = {
                                                    {
                                                        type = "ParticleSystem",
                                                        maxParticles = 16,
                                                        emissionRate = 8.00,
                                                        emitterShape = "Cone",
                                                        shapeRadius = 0.04,
                                                        coneAngle = 28.00,
                                                        boxSize = Vector3(1.00, 1.00, 1.00),
                                                        startLifetime = 0.70,
                                                        startSpeed = 1.40,
                                                        startSize = 0.08,
                                                        endSize = 0.02,
                                                        startColor = Color(1.00, 0.75, 0.25, 0.90),
                                                        endColor = Color(1.00, 0.20, 0.00, 0.00),
                                                        gravity = Vector3(0.00, -0.80, 0.00),
                                                        worldSimulation = false,
                                                        textureRef = "Assets/VFX/spark_soft.png",
                                                        visible = true,
                                                        loop = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        destroyOwnerWhenFinished = false,
                                                        burstCount = 1,
                                                        textureSheetColumns = 1,
                                                        textureSheetRows = 1,
                                                        textureSheetFrameCount = 1,
                                                        textureSheetFramesPerSecond = 0.00,
                                                        blendMode = "Additive"
                                                    },
                                                }
                                            },
                                            {
                                                name = "Smoke",
                                                uuid = "TORCH-FL3-SMOK-4000-8000-000000000001",
                                                position = Vector3(0.00, 0.35, 0.00),
                                                components = {
                                                    {
                                                        type = "ParticleSystem",
                                                        maxParticles = 12,
                                                        emissionRate = 3.50,
                                                        emitterShape = "Cone",
                                                        shapeRadius = 0.06,
                                                        coneAngle = 40.00,
                                                        boxSize = Vector3(1.00, 1.00, 1.00),
                                                        startLifetime = 1.80,
                                                        startSpeed = 0.55,
                                                        startSize = 0.35,
                                                        endSize = 0.85,
                                                        startColor = Color(0.55, 0.55, 0.55, 0.35),
                                                        endColor = Color(0.35, 0.35, 0.35, 0.00),
                                                        gravity = Vector3(0.00, 0.15, 0.00),
                                                        worldSimulation = false,
                                                        textureRef = "Assets/VFX/humomultitextura.png",
                                                        visible = true,
                                                        loop = true,
                                                        playOnAwake = true,
                                                        simulateInEditMode = true,
                                                        destroyOwnerWhenFinished = false,
                                                        burstCount = 1,
                                                        textureSheetColumns = 4,
                                                        textureSheetRows = 2,
                                                        textureSheetFrameCount = 8,
                                                        textureSheetFramesPerSecond = 8.00,
                                                        blendMode = "Alpha"
                                                    },
                                                }
                                            },
                                        }
                                    },
                                }
                            },
                            {
                                name = "barrel_1",
                                uuid = "126BA0D2-E28A-4EB0-81FB-9A7CADBDB81A",
                                position = Vector3(-10.00, 0.00, -10.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/barrel_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                }
                            },
                            {
                                name = "barrel_2",
                                uuid = "F63FA6C8-45F0-42A3-BFBB-1E0418A04769",
                                position = Vector3(10.00, 0.00, -10.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/barrel_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                }
                            },
                            {
                                name = "chest_1",
                                uuid = "21C6C00A-58A6-408B-A6B8-E53E9BE04BEC",
                                position = Vector3(-10.00, 0.00, 10.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/chest.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                }
                            },
                            {
                                name = "banner_1",
                                uuid = "60FDC273-65AE-491F-A7EC-4B6E0DB54C3F",
                                position = Vector3(0.00, 0.00, 13.50),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/banner_red.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        shaderRef = "basic",
                                        shaderPropertyOverrides = "",
                                        meshIndex = 0,
                                        multiMesh = false,
                                        castShadows = true,
                                        receiveShadows = true
                                    },
                                }
                            },
                        }
                    },
                }
            },
            {
                name = "Player",
                uuid = "F86013BD-FF24-4672-91B3-9411CDF272AE",
                collisionLayer = "Characters",
                prefab = "Player Ranger",
                position = Vector3(0.90, 0.10, 0.00),
                overrides = {
                    components = {
                        { type = "CharacterStatsApplier",
                            usePlayerSelection = false,
                            characterDefinitionRef = "Assets/Data/Characters/Ranger.rtbasset",
                        },
                        { type = "ThirdPersonCharacterController",
                            cameraObject = "875ACC83-E59F-4B26-BAE7-B3C4BF3A1637",
                            health = "F86013BD-FF24-4672-91B3-9411CDF272AE/HealthComponent",
                            animator = "B07543DC-D2C4-4C9A-BAAE-829055141C7A/Animator",
                            attackAbility = "F86013BD-FF24-4672-91B3-9411CDF272AE/ProjectileAttackAbility",
                            attackAimTrail = "7C57C988-6629-4FE8-AFF1-8D4D39C84D1B/TrailRenderer",
                            aimArrowVisual = "",
                        },
                        { type = "PlayerDamageFeedback",
                            health = "F86013BD-FF24-4672-91B3-9411CDF272AE/HealthComponent",
                        },
                        { type = "PlayerAmmoSystem",
                            ammoSlider = "A1447084-CE03-491F-A070-85BF4BD352D6/UISlider",
                            ammoFillPanel = "A1000001-0011-4000-8000-000000000011/UIPanel",
                        },
                        { type = "ProjectileAttackAbility",
                            hitAudio = "C0MBAT-H1T0-4001-8000-000000000001/AudioSourceComponent",
                        },
                    },
                },
                children = {
                    {
                        name = "PlayerNameplate",
                        uuid = "1BBB9837-C7AC-4612-AAA7-5F19B0A7CB43",
                        position = Vector3(0.00, 2.20, 0.00),
                        rotation = Quaternion.FromEulerAngles(50.00, -180.00, 0.00),
                        children = {
                            {
                                name = "NameplateCanvas",
                                uuid = "51AAC541-14A9-4369-A3AF-32FDFED053CA",
                                components = {
                                    {
                                        type = "Canvas",
                                        renderMode = "WorldSpace",
                                        canvasSize = Vector2(200.00, 24.00),
                                        pixelsPerUnit = 100.00,
                                        sortOrder = 25,
                                        faceCamera = false,
                                        faceCameraLockY = false
                                    },
                                    {
                                        type = "PlayerNameplateUI",
                                        displayNameText = "A1000001-0004-4000-8000-000000000004/UIText",
                                        healthBarUI = "40968BFF-6484-4ED6-AF95-45D207A01236/HealthBarUI"
                                    },
                                },
                                children = {
                                    {
                                        name = "NameplateRoot",
                                        uuid = "DBD277BA-8CD7-4043-840D-199101F16746",
                                        children = {
                                            {
                                                name = "NameplateHealthTrack",
                                                uuid = "40968BFF-6484-4ED6-AF95-45D207A01236",
                                                components = {
                                                    {
                                                        type = "UIPanel",
                                                        backgroundColor = Color(0.00, 0.00, 0.00, 0.00),
                                                        borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                                        borderThickness = 0.00,
                                                        hasBorder = false,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.50, 0.24),
                                                        anchorMax = Vector2(0.50, 0.24),
                                                        pivot = Vector2(0.50, 0.50),
                                                        anchoredPosition = Vector2(0.00, -2.00),
                                                        sizeDelta = Vector2(168.00, 5.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                    {
                                                        type = "UISlider",
                                                        minValue = 0.00,
                                                        maxValue = 1.00,
                                                        value = 1.00,
                                                        interactable = false,
                                                        fillPanel = "78ED8B41-2F67-415E-946B-1808C2F9C311/UIPanel",
                                                        handlePanel = "A1000001-0007-4000-8000-000000000007/UIPanel"
                                                    },
                                                    {
                                                        type = "HealthBarUI",
                                                        health = "F86013BD-FF24-4672-91B3-9411CDF272AE/HealthComponent",
                                                        fillPanel = "78ED8B41-2F67-415E-946B-1808C2F9C311/UIPanel",
                                                        highHealthColor = Color(0.12, 0.78, 0.24, 1.00),
                                                        midHealthColor = Color(0.95, 0.74, 0.16, 1.00),
                                                        lowHealthColor = Color(0.82, 0.18, 0.18, 1.00)
                                                    },
                                                },
                                                children = {
                                                    {
                                                        name = "NameplateFill",
                                                        uuid = "78ED8B41-2F67-415E-946B-1808C2F9C311",
                                                        components = {
                                                            {
                                                                type = "UIPanel",
                                                                backgroundColor = Color(0.12, 0.78, 0.24, 1.00),
                                                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                                                borderThickness = 0.00,
                                                                hasBorder = false,
                                                                isVisible = true,
                                                                raycastTarget = false,
                                                                anchorMin = Vector2(0.00, 0.50),
                                                                anchorMax = Vector2(0.00, 0.50),
                                                                pivot = Vector2(0.00, 0.50),
                                                                anchoredPosition = Vector2(1.00, 0.00),
                                                                sizeDelta = Vector2(168.00, 12.00),
                                                                rotation = 0.00,
                                                                scale = Vector2(1.00, 1.00)
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "NameplateAmmoTrack",
                                                uuid = "A1447084-CE03-491F-A070-85BF4BD352D6",
                                                components = {
                                                    {
                                                        type = "UIPanel",
                                                        backgroundColor = Color(0.00, 0.00, 0.00, 0.00),
                                                        borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                                        borderThickness = 0.00,
                                                        hasBorder = false,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.50, 0.10),
                                                        anchorMax = Vector2(0.50, 0.10),
                                                        pivot = Vector2(0.50, 0.50),
                                                        anchoredPosition = Vector2(0.00, -10.00),
                                                        sizeDelta = Vector2(168.00, 5.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                    {
                                                        type = "UISlider",
                                                        minValue = 0.00,
                                                        maxValue = 1.00,
                                                        value = 1.00,
                                                        interactable = false,
                                                        fillPanel = "A1000001-0011-4000-8000-000000000011/UIPanel",
                                                        handlePanel = "A1000001-0012-4000-8000-000000000012/UIPanel"
                                                    },
                                                },
                                            },
                                        }
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "Avatar",
                        uuid = "B07543DC-D2C4-4C9A-BAAE-829055141C7A",
                        scale = Vector3(0.85, 0.85, 0.85),
                        components = {
                            {
                                type = "Animator",
                                modelRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                currentClipName = "Idle",
                                defaultClip = "Idle",
                                speed = 1.00,
                                playing = true,
                                looping = true,
                                additionalModels = {
                                    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
                                    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
                                    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx"                                },
                                keyClips = {
                                        {
                                            key = "Idle",
                                            clipFbxRef = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx|Idle_A",
                                            loop = true
                                        },
                                        {
                                            key = "Walk",
                                            clipFbxRef = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx|Walking_A",
                                            loop = true
                                        },
                                        {
                                            key = "Run",
                                            clipFbxRef = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx|Running_A",
                                            loop = true
                                        },
                                        {
                                            key = "AimDraw",
                                            clipFbxRef = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx|Ranged_Bow_Draw",
                                            loop = false
                                        },
                                        {
                                            key = "AimLoop",
                                            clipFbxRef = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx|Ranged_Bow_Aiming_Idle",
                                            loop = true
                                        },
                                        {
                                            key = "Attack",
                                            clipFbxRef = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx|Ranged_Bow_Release",
                                            loop = false
                                        },
                                        {
                                            key = "Death",
                                            clipFbxRef = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx|Death_A",
                                            loop = false
                                        }                                }
                            },
                        },
                        children = {
                        }
                    },
                    {
                        name = "MainCamera",
                        uuid = "875ACC83-E59F-4B26-BAE7-B3C4BF3A1637",
                        position = Vector3(0.00, 9.48, -7.07),
                        rotation = Quaternion.FromEulerAngles(50.00, 0.00, 0.00),
                        components = {
                            {
                                type = "CameraComponent",
                                fov = 45.00,
                                nearClip = 0.10,
                                farClip = 100.00,
                                projectionType = "Perspective",
                                orthographicSize = 3.50,
                                syncWithTransform = true,
                                isMainCamera = true
                            },
                        }
                    },
                }
            },
        }
    }
end
