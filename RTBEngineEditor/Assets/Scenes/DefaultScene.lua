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
                        color = Color(1.00, 1.00, 1.00, 1.00),
                        intensity = 0.50,
                        range = 10.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        syncPosition = true,
                        syncDirection = true
                    },
                }
            },
            {
                name = "PointLight",
                uuid = "148BC308-B383-468A-B7FF-CE0203AAAB79",
                position = Vector3(3.00, 4.00, 0.00),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        color = Color(0.20, 0.50, 1.00, 1.00),
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
                name = "SpotLight",
                uuid = "7858929D-02D1-4003-BBBE-FA27270B24FB",
                position = Vector3(-3.00, 6.00, 0.00),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        color = Color(1.00, 0.00, 0.00, 50.00),
                        intensity = 50.00,
                        range = 10.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        syncPosition = true,
                        syncDirection = true
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
                        faceCamera = false
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
                        useRelativeMouseWhenClosed = true,
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
                                text = "Round 1",
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
                                text = "Start in 5",
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
                                                type = "ResumeGameButton",
                                                controller = nil
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
                                                pressOutTimeSec = 0.12
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
                                                type = "ExitToMenuButton",
                                                controller = nil
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
                                                pressOutTimeSec = 0.12
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
                        enemyTemplate = "753770D2-B00E-4859-B7D3-5ABF1249B014",
                        playerObject = "E8682E33-50ED-45D8-BC76-B31113639F9E",
                        uiHandler = "62E6AE07-7DC2-42B3-8F0E-AA7CA40264F0/RoundUIHandler",
                        roundCountdownDuration = 5.00,
                        baseEnemiesPerRound = 2,
                        additionalEnemiesPerRound = 1,
                        winningRound = 5,
                        playerRespawnDelay = 30.00,
                        teamWipeSceneDelay = 5.00,
                        finalScenePath = "Assets/Scenes/FinalScene.lua",
                        enemyPrefabName = "Enemy Melee"
                    },
                }
            },
            {
                name = "Floor",
                uuid = "737688CD-BE46-4776-B38D-CA30B01FB6D1",
                position = Vector3(-0.46, -0.05, 0.00),
                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                scale = Vector3(31.87, 0.10, 28.33),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Default/Models/cube.obj",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                        meshIndex = 0,
                        multiMesh = false
                    },
                    {
                        type = "BoxColliderComponent",
                        size = Vector3(1.00, 1.00, 1.00),
                        isTrigger = false
                    },
                    {
                        type = "RigidBodyComponent",
                        mass = 1.00,
                        friction = 0.50,
                        restitution = 0.00,
                        bodyType = "Static"
                    },
                }
            },
            {
                name = "EnemySpawnPointLeft",
                uuid = "74B6BC8E-5892-4C38-A4B6-8AAAF7B1F001",
                position = Vector3(-11.80, 0.00, 8.60),
                rotation = Quaternion.FromEulerAngles(0.00, 135.00, 0.00),
                components = {
                    {
                        type = "EnemySpawnPoint"
                    },
                }
            },
            {
                name = "EnemySpawnPointRight",
                uuid = "74B6BC8E-5892-4C38-A4B6-8AAAF7B1F002",
                position = Vector3(11.90, 0.00, 9.60),
                rotation = Quaternion.FromEulerAngles(0.00, -135.00, 0.00),
                components = {
                    {
                        type = "EnemySpawnPoint"
                    },
                }
            },
            {
                name = "EnemySpawnPointRear",
                uuid = "74B6BC8E-5892-4C38-A4B6-8AAAF7B1F003",
                position = Vector3(0.00, 0.00, -11.80),
                components = {
                    {
                        type = "EnemySpawnPoint"
                    },
                }
            },
            {
                name = "Barbarian",
                uuid = "753770D2-B00E-4859-B7D3-5ABF1249B014",
                collisionLayer = "Characters",
                prefab = "Enemy Melee",
                position = Vector3(-3.00, 0.00, 0.00),
                overrides = {
                    components = {
                        { type = "EnemyTargetTracker",
                            targetObject = "E8682E33-50ED-45D8-BC76-B31113639F9E",
                        },
                        { type = "EnemyAnimationDriver",
                            animator = "E1F5A9B7-1030-4F9B-9A18-77AD11B06E20/Animator",
                        },
                        { type = "EnemyMeleeAI",
                            health = "753770D2-B00E-4859-B7D3-5ABF1249B014/HealthComponent",
                            targetTracker = "753770D2-B00E-4859-B7D3-5ABF1249B014/EnemyTargetTracker",
                            animationDriver = "753770D2-B00E-4859-B7D3-5ABF1249B014/EnemyAnimationDriver",
                            locomotion = "753770D2-B00E-4859-B7D3-5ABF1249B014/EnemyLocomotionController",
                            meleeAttack = "753770D2-B00E-4859-B7D3-5ABF1249B014/MeleeSphereAttackAbility",
                        },
                    },
                },
                children = {
                    {
                        name = "EnemyAttackOrigin",
                        uuid = "189E3F37-41B7-4DE9-A35D-206162B433E8",
                        prefab = "EnemyAttackOrigin",
                        position = Vector3(0.00, 0.85, 0.75),
                        overrides = {
                        },
                    },
                    {
                        name = "Enemy Model",
                        uuid = "E1F5A9B7-1030-4F9B-9A18-77AD11B06E20",
                        prefab = "Enemy Model",
                        scale = Vector3(0.85, 0.85, 0.85),
                        overrides = {
                        },
                        children = {
                            {
                                name = "Barbarian_Body",
                                uuid = "B8E5803A-E8BB-4776-9BD9-C6190F95702E",
                                prefab = "Barbarian_Body",
                                overrides = {
                                },
                            },
                            {
                                name = "Barbarian_Head",
                                uuid = "2982F2FE-F453-45A3-9148-3AD13E458FFA",
                                prefab = "Barbarian_Head",
                                position = Vector3(0.00, 1.22, 0.00),
                                overrides = {
                                },
                            },
                            {
                                name = "Barbarian_BearHat",
                                uuid = "C7ACF81B-53DB-4CC3-988F-9AC652210592",
                                prefab = "Barbarian_BearHat",
                                position = Vector3(0.00, 1.22, 0.00),
                                overrides = {
                                },
                            },
                            {
                                name = "Barbarian_LegLeft",
                                uuid = "E1BD6A74-F41F-4182-9BC0-4E9D6F84B8D1",
                                prefab = "Barbarian_LegLeft",
                                overrides = {
                                },
                            },
                            {
                                name = "Barbarian_LegRight",
                                uuid = "921612F1-49BF-4AE2-A7F4-37E3F478D7C5",
                                prefab = "Barbarian_LegRight",
                                overrides = {
                                },
                            },
                            {
                                name = "Barbarian_ArmRight",
                                uuid = "2B624E10-DAB9-46BE-94B3-23F07785827F",
                                prefab = "Barbarian_ArmRight",
                                overrides = {
                                },
                            },
                            {
                                name = "Barbarian_ArmLeft",
                                uuid = "5DD634B6-F3BA-4C16-8023-EA55C5DF953A",
                                prefab = "Barbarian_ArmLeft",
                                overrides = {
                                },
                            },
                            {
                                name = "root",
                                uuid = "46CA0DC1-50F6-4561-A6F7-DACE89D0EF09",
                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, 0.00),
                                children = {
                                    {
                                        name = "hips",
                                        uuid = "3BCB9B6D-D246-4AF2-9A6E-CD82D65367CB",
                                        position = Vector3(0.00, 0.41, -0.00),
                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.00),
                                        children = {
                                            {
                                                name = "spine",
                                                uuid = "F42CCCDB-8B82-4879-AA72-97B1FC470F97",
                                                position = Vector3(-0.00, 0.19, -0.00),
                                                children = {
                                                    {
                                                        name = "chest",
                                                        uuid = "F9DF6F7D-5134-4FAA-AC83-990AC00FE938",
                                                        position = Vector3(-0.00, 0.37, 0.00),
                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                        children = {
                                                            {
                                                                name = "upperarm.l",
                                                                uuid = "024E2FCE-8B23-40A3-88E0-8E05E13B4C23",
                                                                position = Vector3(0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-3.28, 90.00, -90.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.l",
                                                                        uuid = "FAFF56EB-E618-4FBD-B00B-4610234DF400",
                                                                        position = Vector3(0.00, 0.24, 0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, -6.34),
                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.l",
                                                                                uuid = "BC3C8B41-4189-4CA9-A00B-99CB0939F792",
                                                                                position = Vector3(-0.00, 0.26, -0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.l",
                                                                                        uuid = "951CA939-C4C1-4CFE-95DA-DB66333D0411",
                                                                                        position = Vector3(-0.00, 0.07, -0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.l",
                                                                                                uuid = "B4031D73-35A2-4C11-BB5E-9E4CF71E489C",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, -90.00),
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
                                                                name = "upperarm.r",
                                                                uuid = "F929D2A6-5C78-484F-8411-EC0F1D68B7A6",
                                                                position = Vector3(-0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-3.28, -90.00, 90.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.r",
                                                                        uuid = "4C27EA79-551A-4B5D-8861-24E0E8B4F5BB",
                                                                        position = Vector3(-0.00, 0.24, 0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, 6.34),
                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.r",
                                                                                uuid = "CEFEEC76-C168-4AC6-BF13-87C6B5138338",
                                                                                position = Vector3(0.00, 0.26, -0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, -3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.r",
                                                                                        uuid = "F78265C4-34A8-428F-B038-11382CBFB6F9",
                                                                                        position = Vector3(0.00, 0.07, -0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.r",
                                                                                                uuid = "6FCF1FB3-1C5E-421E-B2CD-A8D0E4F92833",
                                                                                                prefab = "handslot.r",
                                                                                                position = Vector3(0.00, -0.79, -1.11),
                                                                                                rotation = Quaternion.FromEulerAngles(90.00, 53.13, 0.00),
                                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                                overrides = {
                                                                                                },
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Barbarian_Axe",
                                                                                                        uuid = "13B7C927-2535-4223-B5AE-181A5DD88B7D",
                                                                                                        prefab = "Barbarian_Axe",
                                                                                                        position = Vector3(0.00, 0.10, -0.06),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 90.00),
                                                                                                        overrides = {
                                                                                                        },
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
                                                                name = "head",
                                                                uuid = "AE9734A1-472B-4FAE-9323-3E718E4C3E67",
                                                                position = Vector3(-0.00, 0.27, 0.00),
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.l",
                                                uuid = "CFBD0A0D-9C4A-4157-B7E1-4A8A203D4751",
                                                position = Vector3(0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(1.99, -180.00, 180.00),
                                                scale = Vector3(1.00, 1.00, 1.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.l",
                                                        uuid = "752ACA49-7342-4078-877A-F46596A1D73D",
                                                        position = Vector3(0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(12.20, -0.00, -0.00),
                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                        children = {
                                                            {
                                                                name = "foot.l",
                                                                uuid = "CB012F73-4215-4C33-B03B-EB518F8EBA23",
                                                                position = Vector3(-0.00, 0.15, -0.00),
                                                                rotation = Quaternion.FromEulerAngles(-54.16, 0.00, 0.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.l",
                                                                        uuid = "B0122851-EA55-42C0-AD4D-13E0B228EAA5",
                                                                        position = Vector3(0.00, 0.17, 0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.04, -180.00, 0.00),
                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                    },
                                                                }
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.r",
                                                uuid = "91A21399-677D-45C0-9EB1-10683F5FEC8A",
                                                position = Vector3(-0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(1.99, 180.00, -180.00),
                                                scale = Vector3(1.00, 1.00, 1.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.r",
                                                        uuid = "60C817F6-6C8D-4574-9ED6-B52B6B6CAEDC",
                                                        position = Vector3(-0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(12.20, 0.00, 0.00),
                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                        children = {
                                                            {
                                                                name = "foot.r",
                                                                uuid = "EDEBD2D3-E280-4FFD-BAA3-449CABCF4A64",
                                                                position = Vector3(0.00, 0.15, -0.00),
                                                                rotation = Quaternion.FromEulerAngles(-54.16, 0.00, 0.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.r",
                                                                        uuid = "3A08E12A-9E94-489F-892F-456FBB07F6F0",
                                                                        position = Vector3(-0.00, 0.17, 0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.04, 180.00, 0.00),
                                                                        scale = Vector3(1.00, 1.00, 1.00),
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
                        }
                    },
                }
            },
            {
                name = "OnlinePlayers",
                uuid = "B1A2C3D4-E5F6-7890-ABCD-EF1234567891",
                components = {
                    {
                        type = "OnlinePlayerManager",
                        localPlayerObject = "E8682E33-50ED-45D8-BC76-B31113639F9E",
                        remoteSpawnOffsetX = 2.50
                    },
                }
            },
            {
                name = "Player GO",
                uuid = "E8682E33-50ED-45D8-BC76-B31113639F9E",
                collisionLayer = "Characters",
                components = {
                    {
                        type = "NetworkIdentity",
                        networkOwnerUserId = "",
                        networkPlayerSlot = -1
                    },
                    {
                        type = "NetworkTransform",
                        sendRate = 20.00,
                        interpolationSpeed = 14.00,
                        replicatePosition = true,
                        replicateRotation = true
                    },
                    {
                        type = "ThirdPersonCharacterController",
                        cameraObject = "0E7CE370-7C0E-45A8-89CA-AD7A3E70C216",
                        health = "E8682E33-50ED-45D8-BC76-B31113639F9E/HealthComponent",
                        team = 1,
                        moveSpeed = 4.00,
                        sprintMultiplier = 1.75,
                        turnSpeed = 720.00,
                        cameraDistance = 11.00,
                        cameraFocusOffset = Vector3(0.00, 0.05, 0.00),
                        animator = "5200AC9B-039A-416E-82AE-FD5FE1754C95/Animator",
                        projectileAttack = "E8682E33-50ED-45D8-BC76-B31113639F9E/ProjectileAttackAbility",
                        attackJoystick = "B4C7E92D-5F22-43A9-8E3B-2A6D7D1A8C10/UIJoystick",
                        attackAimTrail = "7C57C988-6629-4FE8-AFF1-8D4D39C84D1B/TrailRenderer",
                        aimArrowVisual = "R4NG3R00-ARRO-4000-8000-000000000001",
                        idleAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx|Idle_A",
                        walkAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx|Walking_A",
                        runAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx|Running_A",
                        aimDrawAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx|Ranged_Bow_Draw",
                        aimLoopAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx|Ranged_Bow_Aiming_Idle",
                        attackAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx|Ranged_Bow_Release",
                        deathAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx|Death_A"
                    },
                    {
                        type = "ProjectileAttackAbility",
                        attackOriginOffset = Vector3(0.20, 1.00, 0.40),
                        launchOriginObject = "109DAEF5-4736-4BB3-B63F-DD6B5B44855E",
                        launchForwardOffset = 0.35,
                        projectileModel = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/arrow_bow.obj",
                        projectileTexture = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/ranger_texture.png",
                        cooldown = 0.80,
                        damage = 25.00,
                        hitDelay = 0.28,
                        recoveryDuration = 0.45,
                        projectileSpeed = 14.00,
                        projectileLifetime = 0.85,
                        projectileRadius = 0.12,
                        destroyOnHit = true,
                        maxHits = 1,
                        ignoreSameTeam = true
                    },
                    {
                        type = "HealthComponent",
                        maxHealth = 100.00,
                        currentHealth = 100.00
                    },
                    {
                        type = "RigidBodyComponent",
                        mass = 1.00,
                        friction = 0.50,
                        restitution = 0.00,
                        bodyType = "Dynamic"
                    },
                    {
                        type = "CapsuleColliderComponent",
                        radius = 0.30,
                        height = 1.55,
                        centerOffset = Vector3(0.00, 0.78, 0.00),
                        isTrigger = false
                    },
                },
                children = {
                    {
                        name = "PlayerNameplate",
                        uuid = "A1000001-0001-4000-8000-000000000001",
                        position = Vector3(0.00, 2.20, 0.00),
                        children = {
                            {
                                name = "NameplateCanvas",
                                uuid = "A1000001-0002-4000-8000-000000000002",
                                components = {
                                    {
                                        type = "Canvas",
                                        renderMode = "WorldSpace",
                                        canvasSize = Vector2(200.00, 24.00),
                                        pixelsPerUnit = 100.00,
                                        sortOrder = 25,
                                        faceCamera = true
                                    },
                                    {
                                        type = "PlayerNameplateUI",
                                        displayNameText = "A1000001-0004-4000-8000-000000000004/UIText",
                                        healthBarUI = "A1000001-0005-4000-8000-000000000005/HealthBarUI"
                                    },
                                },
                                children = {
                                    {
                                        name = "NameplateRoot",
                                        uuid = "A1000001-0003-4000-8000-000000000003",
                                        children = {
                                            {
                                                name = "FrameImage",
                                                uuid = "A1000001-0008-4000-8000-000000000008",
                                                components = {
                                                    {
                                                        type = "UIImage",
                                                        texture = "Assets/UI/GuildSlate/health_nameplate_frame.texture",
                                                        tintColor = Color(1.00, 1.00, 1.00, 1.00),
                                                        preserveAspect = false,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.50, 0.50),
                                                        anchorMax = Vector2(0.50, 0.50),
                                                        pivot = Vector2(0.50, 0.50),
                                                        anchoredPosition = Vector2(0.00, 0.00),
                                                        sizeDelta = Vector2(200.00, 20.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(0.92, 1.89)
                                                    },
                                                }
                                            },
                                            {
                                                name = "DisplayName",
                                                uuid = "A1000001-0004-4000-8000-000000000004",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Ranger",
                                                        color = Color(1.00, 0.82, 0.47, 1.00),
                                                        fontSize = 27.10,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.50, 0.76),
                                                        anchorMax = Vector2(0.50, 0.76),
                                                        pivot = Vector2(0.50, 0.50),
                                                        anchoredPosition = Vector2(0.00, 24.00),
                                                        sizeDelta = Vector2(184.00, 12.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "NameplateHealthTrack",
                                                uuid = "A1000001-0005-4000-8000-000000000005",
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
                                                        fillPanel = "A1000001-0006-4000-8000-000000000006/UIPanel",
                                                        handlePanel = "A1000001-0007-4000-8000-000000000007/UIPanel"
                                                    },
                                                    {
                                                        type = "HealthBarUI",
                                                        health = "E8682E33-50ED-45D8-BC76-B31113639F9E/HealthComponent",
                                                        fillPanel = "A1000001-0006-4000-8000-000000000006/UIPanel",
                                                        highHealthColor = Color(0.24, 0.75, 0.63, 1.00),
                                                        midHealthColor = Color(0.91, 0.66, 0.29, 1.00),
                                                        lowHealthColor = Color(0.77, 0.29, 0.29, 1.00)
                                                    },
                                                },
                                                children = {
                                                    {
                                                        name = "TrackImage",
                                                        uuid = "A1000001-0009-4000-8000-000000000009",
                                                        components = {
                                                            {
                                                                type = "UIImage",
                                                                texture = "Assets/UI/GuildSlate/health_bar_track.texture",
                                                                tintColor = Color(1.00, 1.00, 1.00, 1.00),
                                                                preserveAspect = false,
                                                                isVisible = true,
                                                                raycastTarget = false,
                                                                anchorMin = Vector2(0.50, 0.50),
                                                                anchorMax = Vector2(0.50, 0.50),
                                                                pivot = Vector2(0.50, 0.50),
                                                                anchoredPosition = Vector2(0.00, 0.00),
                                                                sizeDelta = Vector2(168.00, 13.00),
                                                                rotation = 0.00,
                                                                scale = Vector2(1.00, 1.00)
                                                            },
                                                        }
                                                    },
                                                    {
                                                        name = "NameplateFill",
                                                        uuid = "A1000001-0006-4000-8000-000000000006",
                                                        components = {
                                                            {
                                                                type = "UIPanel",
                                                                backgroundColor = Color(0.24, 0.75, 0.63, 1.00),
                                                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                                                borderThickness = 0.00,
                                                                hasBorder = false,
                                                                isVisible = true,
                                                                raycastTarget = false,
                                                                anchorMin = Vector2(0.00, 0.50),
                                                                anchorMax = Vector2(0.00, 0.50),
                                                                pivot = Vector2(0.00, 0.50),
                                                                anchoredPosition = Vector2(1.00, 0.00),
                                                                sizeDelta = Vector2(165.00, 12.00),
                                                                rotation = 0.00,
                                                                scale = Vector2(1.00, 1.00)
                                                            },
                                                        }
                                                    },
                                                    {
                                                        name = "NameplateHandle",
                                                        uuid = "A1000001-0007-4000-8000-000000000007",
                                                        components = {
                                                            {
                                                                type = "UIPanel",
                                                                backgroundColor = Color(0.00, 0.00, 0.00, 0.00),
                                                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                                                borderThickness = 0.00,
                                                                hasBorder = false,
                                                                isVisible = false,
                                                                raycastTarget = false,
                                                                anchorMin = Vector2(0.00, 0.50),
                                                                anchorMax = Vector2(0.00, 0.50),
                                                                pivot = Vector2(0.50, 0.50),
                                                                anchoredPosition = Vector2(168.00, 0.00),
                                                                sizeDelta = Vector2(1.00, 1.00),
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
                        name = "Attack Aim Trail",
                        uuid = "7C57C988-6629-4FE8-AFF1-8D4D39C84D1B",
                        position = Vector3(0.00, 0.05, 0.00),
                        components = {
                            {
                                type = "TrailRenderer",
                                width = 0.55,
                                color = Color(1.00, 1.00, 1.00, 1.00),
                                visible = false
                            },
                        }
                    },
                    {
                        name = "Player",
                        uuid = "5200AC9B-039A-416E-82AE-FD5FE1754C95",
                        scale = Vector3(0.85, 0.85, 0.85),
                        components = {
                            {
                                type = "Animator",
                                modelRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                currentClipName = "Idle_A",
                                defaultClip = "Idle_A",
                                speed = 1.00,
                                playing = false,
                                looping = true,
                                additionalModels = {
                                    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
                                    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
                                    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx",
                                }
                            },
                        },
                        children = {
                            {
                                name = "Ranger_Body",
                                uuid = "F02F4F7D-E02E-4BFE-BC2C-B95B4789F6B3",
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "Ranger_ArmLeft",
                                uuid = "D96B4FA5-8242-4AAB-AD20-636EC682F83B",
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 1,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "Ranger_LegLeft",
                                uuid = "0014770D-C798-471E-89E4-961965808A32",
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 2,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "Ranger_LegRight",
                                uuid = "50EFDE79-BA4D-4340-961E-70E7378D52F6",
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 3,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "Ranger_ArmRight",
                                uuid = "C670D418-3446-4B72-AF56-E7E400F25F60",
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 4,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "Ranger_Head",
                                uuid = "141353F4-CEB5-4FF6-93FB-27EAD6F7823B",
                                position = Vector3(0.00, 1.24, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 5,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "Ranger_Quiver",
                                uuid = "0ACCC6B3-6BA7-4353-A7D3-9656D95A57CD",
                                position = Vector3(0.01, 0.91, -0.39),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 6,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "Ranger_Cape",
                                uuid = "DDED2BA7-E9C6-4FE0-91B6-184A548DF391",
                                position = Vector3(0.00, 1.24, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 7,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "root",
                                uuid = "C5AD6486-1C69-44C8-8FA5-1631DCC3890A",
                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, 0.00),
                                children = {
                                    {
                                        name = "hips",
                                        uuid = "69657A7D-CE8E-42BB-AE35-5AC07910643C",
                                        position = Vector3(-0.00, 0.39, 0.00),
                                        rotation = Quaternion.FromEulerAngles(0.01, -4.95, 0.00),
                                        scale = Vector3(1.00, 1.00, 1.00),
                                        children = {
                                            {
                                                name = "spine",
                                                uuid = "5C4C8BA9-6DC6-4D34-B001-841D4E3DCA8B",
                                                position = Vector3(0.00, 0.19, 0.00),
                                                rotation = Quaternion.FromEulerAngles(0.04, 0.00, 0.00),
                                                scale = Vector3(1.00, 1.00, 1.00),
                                                children = {
                                                    {
                                                        name = "chest",
                                                        uuid = "C02222AD-8A4A-44A9-9568-713639DFE8FE",
                                                        position = Vector3(0.00, 0.37, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(0.54, 0.00, 0.05),
                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                        children = {
                                                            {
                                                                name = "upperarm.l",
                                                                uuid = "EC6C2857-949D-4DA2-BDF4-ED2450B6C5E5",
                                                                position = Vector3(0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-9.77, 65.84, -140.57),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.l",
                                                                        uuid = "4DB13D0A-4290-419D-A32D-54B0C4E831A5",
                                                                        position = Vector3(0.00, 0.24, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, -42.10),
                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.l",
                                                                                uuid = "2F5ABDB7-48FE-4ADC-B568-EEC24DC4B0A1",
                                                                                position = Vector3(-0.00, 0.26, -0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.l",
                                                                                        uuid = "B2E987EF-3736-4B47-93F3-D0C2A9364C95",
                                                                                        position = Vector3(-0.00, 0.07, 0.00),
                                                                                        rotation = Quaternion.FromEulerAngles(-31.59, 38.95, 11.91),
                                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.l",
                                                                                                uuid = "08964EA4-D7F7-4A0C-BF39-2A6FE9D938B9",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, -90.00),
                                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Ranger_Bow",
                                                                                                        uuid = "109DAEF5-4736-4BB3-B63F-DD6B5B44855E",
                                                                                                        position = Vector3(0.00, 0.00, 0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                                                        components = {
                                                                                                            {
                                                                                                                type = "MeshRenderer",
                                                                                                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/bow_withString.obj",
                                                                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/ranger_texture.png",
                                                                                                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                                meshIndex = 0,
                                                                                                                multiMesh = false
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
                                                                }
                                                            },
                                                            {
                                                                name = "upperarm.r",
                                                                uuid = "0E0C67B4-D021-47E3-8F72-A485BFC87DC6",
                                                                position = Vector3(-0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-20.64, -66.91, 139.67),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.r",
                                                                        uuid = "E5B6B75F-147D-4477-8084-1D15986F517E",
                                                                        position = Vector3(-0.00, 0.24, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 44.26),
                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.r",
                                                                                uuid = "3AC8ADA2-B039-4438-92D1-4FA79F1F2F0F",
                                                                                position = Vector3(-0.00, 0.26, -0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, -3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.r",
                                                                                        uuid = "0BFBA09A-04BA-4516-AA49-8F07C2032614",
                                                                                        position = Vector3(-0.00, 0.07, -0.00),
                                                                                        rotation = Quaternion.FromEulerAngles(-28.98, -45.33, -5.62),
                                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.r",
                                                                                                uuid = "5F01E38D-E9AC-46EF-81CB-2F439BD1306F",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.01, 89.91),
                                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Ranger_Arrow",
                                                                                                        uuid = "R4NG3R00-ARRO-4000-8000-000000000001",
                                                                                                        active = false,
                                                                                                        position = Vector3(0.63, 0.79, -1.16),
                                                                                                        rotation = Quaternion.FromEulerAngles(-52.14, -150.14, 95.92),
                                                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                                                        components = {
                                                                                                            {
                                                                                                                type = "MeshRenderer",
                                                                                                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/arrow_bow.obj",
                                                                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/ranger_texture.png",
                                                                                                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                                meshIndex = 0,
                                                                                                                multiMesh = false
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
                                                                }
                                                            },
                                                            {
                                                                name = "head",
                                                                uuid = "C733BE2F-9D91-40B5-B63D-6EF9A02D5257",
                                                                position = Vector3(0.00, 0.27, -0.00),
                                                                rotation = Quaternion.FromEulerAngles(-0.50, 3.59, -0.04),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.l",
                                                uuid = "99A671E3-FA32-4222-BEEC-7C9757774FF2",
                                                position = Vector3(0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(10.10, 172.41, -173.97),
                                                scale = Vector3(1.00, 1.00, 1.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.l",
                                                        uuid = "922E0DA8-D636-4982-A610-6C56CC401DB5",
                                                        position = Vector3(0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(30.64, -0.00, -0.00),
                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                        children = {
                                                            {
                                                                name = "foot.l",
                                                                uuid = "110886C1-A83F-40FE-8B68-FC24F2E8C303",
                                                                position = Vector3(0.00, 0.15, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-64.16, 19.75, -6.73),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.l",
                                                                        uuid = "2DC5DA98-335A-479A-8B30-CBED96801F0A",
                                                                        position = Vector3(0.00, 0.17, 0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.07, -180.00, -0.00),
                                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                                    },
                                                                }
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.r",
                                                uuid = "0E2ECE82-9C85-4ABF-8BB7-B404F5285237",
                                                position = Vector3(-0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(6.80, 176.17, 176.38),
                                                scale = Vector3(1.00, 1.00, 1.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.r",
                                                        uuid = "FD5B45FE-6637-4772-8CE9-8B3F54F5EA60",
                                                        position = Vector3(0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(31.96, 0.00, 0.00),
                                                        scale = Vector3(1.00, 1.00, 1.00),
                                                        children = {
                                                            {
                                                                name = "foot.r",
                                                                uuid = "CF8BCD2B-2AE5-4426-A71C-25377ECCEECC",
                                                                position = Vector3(0.00, 0.15, -0.00),
                                                                rotation = Quaternion.FromEulerAngles(-67.03, -27.17, 16.96),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.r",
                                                                        uuid = "6F30C2FE-29EF-469A-B98B-C948563D6B7B",
                                                                        position = Vector3(0.00, 0.17, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.08, 180.00, 0.01),
                                                                        scale = Vector3(1.00, 1.00, 1.00),
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
                        }
                    },
                    {
                        name = "MainCamera",
                        uuid = "0E7CE370-7C0E-45A8-89CA-AD7A3E70C216",
                        position = Vector3(0.00, 8.48, -7.07),
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
