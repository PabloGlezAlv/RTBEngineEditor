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
                position = Vector3(-0.02, -0.09, 0.00),
                scale = Vector3(30.00, 0.10, 30.00),
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
                            targetObject = nil,
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
                        name = "Enemy Model",
                        uuid = "E1F5A9B7-1030-4F9B-9A18-77AD11B06E20",
                        scale = Vector3(0.85, 0.85, 0.85),
                        components = {
                            {
                                type = "Animator",
                                modelRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
                                currentClipName = "Idle_A",
                                defaultClip = "Idle_A",
                                speed = 1.00,
                                playing = false,
                                looping = true,
                                additionalModels = {
                                    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
                                    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
                                    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatMelee.fbx",
                                }
                            },
                        },
                        children = {
{
                                name = "root",
                                uuid = "A913A790-4692-4110-A796-722BD0230BA6",
                                children = {
                                    {
                                        name = "hips",
                                        uuid = "75E2BE78-0EE1-42BE-AA7A-D3A8C29E1FB8",
                                        children = {
                                            {
                                                name = "spine",
                                                uuid = "DD98E0C1-34F9-4278-97A7-A9D387C9B7D6",
                                                children = {
                                                    {
                                                        name = "chest",
                                                        uuid = "43575E6B-BFCC-4D3C-B8CA-F2013655F36D",
                                                        children = {
                                                            {
                                                                name = "upperarm.l",
                                                                uuid = "1FCC041D-0715-42CF-91FA-2F66AC3580A2",
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.l",
                                                                        uuid = "1C97F497-3C41-4D85-928F-C0A2541C8C96",
                                                                        children = {
                                                                            {
                                                                                name = "wrist.l",
                                                                                uuid = "208F9A02-2C1B-4824-8AFA-5BEDF19AAA4F",
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.l",
                                                                                        uuid = "2443ACB2-27D0-4F54-9FDE-92E6851FC96E",
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.l",
                                                                                                uuid = "885764D7-E9A8-42CE-A12F-D36FBB8544F2",
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
                                                                uuid = "0E1BCC73-ACD6-437C-BF99-45FD955A92CD",
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.r",
                                                                        uuid = "A8F06D92-C01A-4029-9F09-0BFBB47622C4",
                                                                        children = {
                                                                            {
                                                                                name = "wrist.r",
                                                                                uuid = "BB8FD2BC-B586-4DE6-AFC5-96CACC0874C1",
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.r",
                                                                                        uuid = "EFC307FE-69E8-4CAB-8988-C013159E3EBF",
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.r",
                                                                                                uuid = "F332D7B2-D5C8-404A-997D-33F1147A41E4",
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Barbarian_Axe",
                                                                                                        uuid = "C50212F0-226D-456B-A058-A747589E732E",
                                                                                                        position = Vector3(0.00, 0.10, -0.06),
                                                                                                        components = {
                                                                                                            {
                                                                                                                type = "MeshRenderer",
                                                                                                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/axe_2handed.obj",
                                                                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/barbarian_texture.png",
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
                                                                uuid = "CB92EDB4-E54D-4FCD-951D-98144EF7717D",
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.l",
                                                uuid = "E103DC17-6004-40E2-8F41-E7CDC03BEE48",
                                                children = {
                                                    {
                                                        name = "lowerleg.l",
                                                        uuid = "E20ED9C6-C758-46A0-A0F9-1F434C7C14C2",
                                                        children = {
                                                            {
                                                                name = "foot.l",
                                                                uuid = "13ED9E96-764C-4787-A961-BC3C38BA154C",
                                                                children = {
                                                                    {
                                                                        name = "toes.l",
                                                                        uuid = "5CC4532D-1FAC-4D94-B799-DA63988E17AC",
                                                                    },
                                                                }
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.r",
                                                uuid = "4F32A35D-7711-4952-91A4-61D10C419E4B",
                                                children = {
                                                    {
                                                        name = "lowerleg.r",
                                                        uuid = "285046BA-A1FA-4131-A7CE-79EA2A854686",
                                                        children = {
                                                            {
                                                                name = "foot.r",
                                                                uuid = "758AE098-C0C2-459F-B813-7EFBC4FD2D5E",
                                                                children = {
                                                                    {
                                                                        name = "toes.r",
                                                                        uuid = "EE957398-A104-4E17-A52C-73E31C519A6B",
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
                                                        text = "Player",
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
                                                                sizeDelta = Vector2(168.00, 12.00),
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
                                currentClipName = "ThirdPerson.Idle",
                                defaultClip = "Ranged_Bow_Aiming_Idle",
                                speed = 1.00,
                                playing = true,
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
                                children = {
                                    {
                                        name = "hips",
                                        uuid = "69657A7D-CE8E-42BB-AE35-5AC07910643C",
                                        children = {
                                            {
                                                name = "spine",
                                                uuid = "5C4C8BA9-6DC6-4D34-B001-841D4E3DCA8B",
                                                children = {
                                                    {
                                                        name = "chest",
                                                        uuid = "C02222AD-8A4A-44A9-9568-713639DFE8FE",
                                                        children = {
                                                            {
                                                                name = "upperarm.l",
                                                                uuid = "EC6C2857-949D-4DA2-BDF4-ED2450B6C5E5",
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.l",
                                                                        uuid = "4DB13D0A-4290-419D-A32D-54B0C4E831A5",
                                                                        children = {
                                                                            {
                                                                                name = "wrist.l",
                                                                                uuid = "2F5ABDB7-48FE-4ADC-B568-EEC24DC4B0A1",
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.l",
                                                                                        uuid = "B2E987EF-3736-4B47-93F3-D0C2A9364C95",
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.l",
                                                                                                uuid = "08964EA4-D7F7-4A0C-BF39-2A6FE9D938B9",
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Ranger_Bow",
                                                                                                        uuid = "109DAEF5-4736-4BB3-B63F-DD6B5B44855E",
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
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
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.r",
                                                                        uuid = "E5B6B75F-147D-4477-8084-1D15986F517E",
                                                                        children = {
                                                                            {
                                                                                name = "wrist.r",
                                                                                uuid = "3AC8ADA2-B039-4438-92D1-4FA79F1F2F0F",
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.r",
                                                                                        uuid = "0BFBA09A-04BA-4516-AA49-8F07C2032614",
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.r",
                                                                                                uuid = "5F01E38D-E9AC-46EF-81CB-2F439BD1306F",
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Ranger_Arrow",
                                                                                                        uuid = "R4NG3R00-ARRO-4000-8000-000000000001",
                                                                                                        active = false,
                                                                                                        rotation = Quaternion.FromEulerAngles(-90.00, 0.00, 0.00),
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
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.l",
                                                uuid = "99A671E3-FA32-4222-BEEC-7C9757774FF2",
                                                children = {
                                                    {
                                                        name = "lowerleg.l",
                                                        uuid = "922E0DA8-D636-4982-A610-6C56CC401DB5",
                                                        children = {
                                                            {
                                                                name = "foot.l",
                                                                uuid = "110886C1-A83F-40FE-8B68-FC24F2E8C303",
                                                                children = {
                                                                    {
                                                                        name = "toes.l",
                                                                        uuid = "2DC5DA98-335A-479A-8B30-CBED96801F0A",
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
                                                children = {
                                                    {
                                                        name = "lowerleg.r",
                                                        uuid = "FD5B45FE-6637-4772-8CE9-8B3F54F5EA60",
                                                        children = {
                                                            {
                                                                name = "foot.r",
                                                                uuid = "CF8BCD2B-2AE5-4426-A71C-25377ECCEECC",
                                                                children = {
                                                                    {
                                                                        name = "toes.r",
                                                                        uuid = "6F30C2FE-29EF-469A-B98B-C948563D6B7B",
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
