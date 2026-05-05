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
                        sortOrder = 0
                    },
                    {
                        type = "RoundUIHandler",
                        roundText = "D7D85F5A-5A9A-4E6A-8AA2-9032EC0F0B11/UIText",
                        countdownText = "A5C9F5F4-2D0B-4D2C-9FE5-443A683E1A22/UIText",
                        roundPrefix = "Round ",
                        countdownPrefix = "Start in "
                    },
                    {
                        type = "PauseMenuController",
                        menuRoot = "F0B5D1DA-3CF0-4563-88C2-4731F9E91A10",
                        pauseSimulation = false,
                        useRelativeMouseWhenClosed = false
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
                                color = Color(1.00, 1.00, 1.00, 1.00),
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
                        name = "CountdownText",
                        uuid = "A5C9F5F4-2D0B-4D2C-9FE5-443A683E1A22",
                        components = {
                            {
                                type = "UIText",
                                text = "Start in 5",
                                color = Color(1.00, 0.95, 0.72, 1.00),
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
                        name = "Slider",
                        uuid = "5DF934F7-228E-4AFE-82E3-D6B742F023D6",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.69, 0.69, 0.69, 1.00),
                                borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                borderThickness = 1.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.00, 0.00),
                                anchorMax = Vector2(0.00, 0.00),
                                pivot = Vector2(0.00, 0.50),
                                anchoredPosition = Vector2(10.00, 30.00),
                                sizeDelta = Vector2(200.00, 20.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UISlider",
                                minValue = 0.00,
                                maxValue = 1.00,
                                value = 0.50,
                                interactable = false,
                                fillPanel = "BCE29D83-01B6-4B80-989F-85CB5E49C359/UIPanel",
                                handlePanel = "96F2BB4D-DF67-4728-87D6-56904800DDC0/UIPanel"
                            },
                            {
                                type = "HealthBarUI",
                                health = "E8682E33-50ED-45D8-BC76-B31113639F9E/HealthComponent",
                                fillPanel = "BCE29D83-01B6-4B80-989F-85CB5E49C359/UIPanel",
                                highHealthColor = Color(0.12, 0.78, 0.24, 1.00),
                                midHealthColor = Color(0.95, 0.74, 0.16, 1.00),
                                lowHealthColor = Color(0.82, 0.18, 0.18, 1.00)
                            },
                        },
                        children = {
                            {
                                name = "Fill",
                                uuid = "BCE29D83-01B6-4B80-989F-85CB5E49C359",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.11, 0.20, 1.00, 1.00),
                                        borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                        borderThickness = 1.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.00, 0.00),
                                        anchorMax = Vector2(0.00, 1.00),
                                        pivot = Vector2(0.00, 0.50),
                                        anchoredPosition = Vector2(0.00, 0.00),
                                        sizeDelta = Vector2(100.00, 0.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "Handle",
                                uuid = "96F2BB4D-DF67-4728-87D6-56904800DDC0",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.92, 0.92, 0.92, 1.00),
                                        borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                        borderThickness = 1.00,
                                        hasBorder = false,
                                        isVisible = false,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.00, 0.50),
                                        anchorMax = Vector2(0.00, 0.50),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(100.00, 0.00),
                                        sizeDelta = Vector2(20.00, 20.00),
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
                                        backgroundColor = Color(0.09, 0.10, 0.13, 0.94),
                                        borderColor = Color(0.95, 0.82, 0.42, 1.00),
                                        borderThickness = 2.00,
                                        hasBorder = true,
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
                                        name = "PauseTitle",
                                        uuid = "B7E6A2A0-D3E6-450C-A7E2-9F43E52D8B74",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Paused",
                                                color = Color(1.00, 0.92, 0.62, 1.00),
                                                fontSize = 42.00,
                                                alignment = "Center",
                                                font = nil,
                                                isVisible = true,
                                                raycastTarget = false,
                                                anchorMin = Vector2(0.50, 1.00),
                                                anchorMax = Vector2(0.50, 1.00),
                                                pivot = Vector2(0.50, 1.00),
                                                anchoredPosition = Vector2(0.00, -42.00),
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
                                                backgroundColor = Color(0.18, 0.25, 0.22, 1.00),
                                                borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                                borderThickness = 1.00,
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
                                                normalColor = Color(1.00, 1.00, 1.00, 0.23),
                                                hoveredColor = Color(0.90, 0.90, 0.90, 0.44),
                                                pressedColor = Color(0.70, 0.70, 0.70, 1.00),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.50),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                            {
                                                type = "ButtonStyle",
                                                backgroundPanel = "D51F4600-5E83-4B08-8402-D0F6189B936A/UIPanel",
                                                label = "5DE3585B-3426-421D-8F36-DBAF43123F50/UIText",
                                                normalPanelColor = Color(0.18, 0.25, 0.22, 1.00),
                                                normalTextColor = Color(1.00, 1.00, 1.00, 1.00),
                                                hoverPanelColor = Color(0.25, 0.44, 0.35, 1.00),
                                                hoverTextColor = Color(1.00, 0.96, 0.68, 1.00),
                                                hoverScaleBoost = 1.06,
                                                hoverRotationDeg = 0.00,
                                                clickPanelColor = Color(0.12, 0.30, 0.23, 1.00),
                                                clickTextColor = Color(0.86, 0.86, 0.92, 1.00),
                                                clickScaleBoost = 0.96,
                                                hoverInTimeSec = 0.12,
                                                hoverOutTimeSec = 0.18,
                                                pressInTimeSec = 0.08,
                                                pressOutTimeSec = 0.12
                                            },
                                            {
                                                type = "ResumeGameButton",
                                                controller = "62E6AE07-7DC2-42B3-8F0E-AA7CA40264F0/PauseMenuController"
                                            },
                                        },
                                        children = {
                                            {
                                                name = "Text",
                                                uuid = "5DE3585B-3426-421D-8F36-DBAF43123F50",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Continue",
                                                        color = Color(1.00, 1.00, 1.00, 1.00),
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
                                        name = "BackToMenuButton",
                                        uuid = "9E2C0BE0-4742-4279-A2DD-0C7009566B7B",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.22, 0.18, 0.18, 1.00),
                                                borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                                borderThickness = 1.00,
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
                                                normalColor = Color(1.00, 1.00, 1.00, 0.23),
                                                hoveredColor = Color(0.90, 0.90, 0.90, 0.44),
                                                pressedColor = Color(0.70, 0.70, 0.70, 1.00),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.50),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                            {
                                                type = "ButtonStyle",
                                                backgroundPanel = "9E2C0BE0-4742-4279-A2DD-0C7009566B7B/UIPanel",
                                                label = "2F7C83F3-A334-41DA-BF4C-A4FDDE59BA6C/UIText",
                                                normalPanelColor = Color(0.22, 0.18, 0.18, 1.00),
                                                normalTextColor = Color(1.00, 1.00, 1.00, 1.00),
                                                hoverPanelColor = Color(0.45, 0.30, 0.25, 1.00),
                                                hoverTextColor = Color(1.00, 0.96, 0.68, 1.00),
                                                hoverScaleBoost = 1.06,
                                                hoverRotationDeg = 0.00,
                                                clickPanelColor = Color(0.32, 0.18, 0.16, 1.00),
                                                clickTextColor = Color(0.86, 0.86, 0.92, 1.00),
                                                clickScaleBoost = 0.96,
                                                hoverInTimeSec = 0.12,
                                                hoverOutTimeSec = 0.18,
                                                pressInTimeSec = 0.08,
                                                pressOutTimeSec = 0.12
                                            },
                                            {
                                                type = "SceneChangeButton",
                                                scenePath = "Assets/Scenes/MainMenu.lua"
                                            },
                                        },
                                        children = {
                                            {
                                                name = "Text",
                                                uuid = "2F7C83F3-A334-41DA-BF4C-A4FDDE59BA6C",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Back to Menu",
                                                        color = Color(1.00, 1.00, 1.00, 1.00),
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
                name = "WorldSpaceCanvasTest",
                uuid = "1A5CC89B-B19E-4B64-B0B0-8013A8CE7D2B",
                position = Vector3(0.00, 1.35, 1.50),
                components = {
                    {
                        type = "Canvas",
                        renderMode = "WorldSpace",
                        canvasSize = Vector2(256.00, 256.00),
                        pixelsPerUnit = 128.00,
                        sortOrder = 10
                    },
                },
                children = {
                    {
                        name = "WorldSpaceLogo",
                        uuid = "3E6E79B6-0F02-46BF-98F0-AB2070C70448",
                        components = {
                            {
                                type = "UIImage",
                                texture = "Default/Textures/logo.png",
                                tintColor = Color(1.00, 1.00, 1.00, 0.85),
                                preserveAspect = true,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 32.00),
                                sizeDelta = Vector2(160.00, 160.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "WorldSpaceText",
                        uuid = "C0CC8AF9-6F29-4B07-A64F-5FF1D434DB93",
                        components = {
                            {
                                type = "UIText",
                                text = "World Text",
                                color = Color(0.20, 0.95, 1.00, 1.00),
                                fontSize = 28.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(-106.00, 70.00),
                                sizeDelta = Vector2(240.00, 48.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
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
                        finalSceneDelay = 5.00,
                        finalScenePath = "Assets/Scenes/FinalScene.lua"
                    },
                }
            },
            {
                name = "Floor",
                uuid = "737688CD-BE46-4776-B38D-CA30B01FB6D1",
                position = Vector3(-0.46, -0.05, 0.00),
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
                rotation = Quaternion.FromEulerAngles(0.00, -135.00, -0.00),
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
                name = "Enemy Melee",
                uuid = "753770D2-B00E-4859-B7D3-5ABF1249B014",
                prefab = "Enemy Melee",
                position = Vector3(-3.00, 0.00, 0.00),
                overrides = {
                    components = {
                        { type = "EnemyTargetTracker",
                            targetObject = "E8682E33-50ED-45D8-BC76-B31113639F9E",
                        },
                        { type = "EnemyAnimationDriver",
                            animator = "20AA6191-BE97-4D61-8ABA-084B99A4C41B/Animator",
                        },
                        { type = "EnemyMeleeAI",
                            health = "753770D2-B00E-4859-B7D3-5ABF1249B014/HealthComponent",
                            targetTracker = "753770D2-B00E-4859-B7D3-5ABF1249B014/EnemyTargetTracker",
                            animationDriver = "753770D2-B00E-4859-B7D3-5ABF1249B014/EnemyAnimationDriver",
                            locomotion = "753770D2-B00E-4859-B7D3-5ABF1249B014/EnemyLocomotionController",
                        },
                    },
                },
                children = {
                    {
                        name = "EnemyAttackOrigin",
                        uuid = "B667AFB3-EEE9-4F0E-AC0B-32ACC9B6786E",
                        prefab = "EnemyAttackOrigin",
                        position = Vector3(0.00, 1.00, 0.85),
                        overrides = {
                        },
                    },
                    {
                        name = "Enemy Model",
                        uuid = "20AA6191-BE97-4D61-8ABA-084B99A4C41B",
                        prefab = "Enemy Model",
                        scale = Vector3(0.01, 0.01, 0.01),
                        overrides = {
                        },
                        children = {
                        }
                    },
                }
            },
            {
                name = "Player GO",
                uuid = "E8682E33-50ED-45D8-BC76-B31113639F9E",
                components = {
                    {
                        type = "ThirdPersonCharacterController",
                        cameraObject = "0E7CE370-7C0E-45A8-89CA-AD7A3E70C216",
                        health = "E8682E33-50ED-45D8-BC76-B31113639F9E/HealthComponent",
                        attackOriginOffset = Vector3(0.00, 1.00, 0.00),
                        moveSpeed = 4.00,
                        sprintMultiplier = 1.75,
                        turnSpeed = 720.00,
                        cameraDistance = 11.00,
                        cameraFocusOffset = Vector3(0.00, 1.20, 0.00),
                        animator = "5200AC9B-039A-416E-82AE-FD5FE1754C95/Animator",
                        attackRange = 1.75,
                        attackCooldown = 0.80,
                        attackDamage = 25.00,
                        attackHitDelay = 0.35,
                        attackSphereRadius = 0.55,
                        attackSphereDistance = 1.15,
                        attackJoystick = "B4C7E92D-5F22-43A9-8E3B-2A6D7D1A8C10/UIJoystick",
                        idleAnimationFbx = "Assets/Models/AnimationsPlayer/Great Sword Idle.fbx",
                        walkAnimationFbx = "Assets/Models/AnimationsPlayer/Great Sword Walk.fbx",
                        runAnimationFbx = "Assets/Models/AnimationsPlayer/Great Sword Run.fbx",
                        attackAnimationFbx = "Assets/Models/AnimationsPlayer/great sword basic atack.fbx",
                        deathAnimationFbx = "Assets/Models/AnimationsPlayer/two handed sword death.fbx"
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
                        radius = 0.35,
                        height = 1.80,
                        centerOffset = Vector3(0.00, 0.90, 0.00),
                        isTrigger = false
                    },
                },
                children = {
                    {
                        name = "Player",
                        uuid = "5200AC9B-039A-416E-82AE-FD5FE1754C95",
                        scale = Vector3(0.01, 0.01, 0.01),
                        components = {
                            {
                                type = "Animator",
                                modelRef = "Assets/Models/Player.fbx",
                                currentClipName = "Attack",
                                defaultClip = "",
                                speed = 1.00,
                                playing = false,
                                looping = true,
                                additionalModels = {
                                    "Assets/Models/AnimationsPlayer/Great Sword Idle.fbx",
                                    "Assets/Models/AnimationsPlayer/Great Sword Run.fbx",
                                    "Assets/Models/AnimationsPlayer/Great Sword Walk.fbx",
                                    "Assets/Models/attack.fbx",
                                    "Assets/Models/hit.fbx",
                                    "Assets/Models/walking.fbx",
                                }
                            },
                        },
                        children = {
                            {
                                name = "mixamorig:Hips",
                                uuid = "A714264A-4699-4F19-80B0-0E51C7679340",
                                position = Vector3(0.00, 105.25, 1.77),
                                children = {
                                    {
                                        name = "mixamorig:Spine",
                                        uuid = "1FF9DF4E-052C-40EA-83FD-7B02F7BBC8AC",
                                        position = Vector3(0.00, 5.25, 0.26),
                                        rotation = Quaternion.FromEulerAngles(-3.61, -0.00, 0.00),
                                        children = {
                                            {
                                                name = "mixamorig:Spine1",
                                                uuid = "527B216F-2BC9-4A0E-83E4-D53AE414FE1B",
                                                position = Vector3(-0.00, 11.07, -0.00),
                                                rotation = Quaternion.FromEulerAngles(-4.06, -0.00, 0.00),
                                                children = {
                                                    {
                                                        name = "mixamorig:Spine2",
                                                        uuid = "FD49F68B-4E10-4663-A772-754A2A70B6D6",
                                                        position = Vector3(0.00, 8.14, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(7.68, 0.00, -0.00),
                                                        children = {
                                                            {
                                                                name = "mixamorig:Neck",
                                                                uuid = "C5D4E2D1-6647-4E67-A733-C0AD7E5EEB55",
                                                                position = Vector3(-0.11, 18.92, -3.74),
                                                                children = {
                                                                    {
                                                                        name = "mixamorig:Head",
                                                                        uuid = "A266E8F1-7496-4F00-BAE7-79EBC5012A87",
                                                                        position = Vector3(0.00, 8.67, 3.32),
                                                                        children = {
                                                                            {
                                                                                name = "mixamorig:HeadTop_End",
                                                                                uuid = "D453D08D-BAB4-4406-88A7-DD01A399261E",
                                                                                position = Vector3(0.00, 18.75, 9.37),
                                                                            },
                                                                        }
                                                                    },
                                                                }
                                                            },
                                                            {
                                                                name = "mixamorig:LeftShoulder",
                                                                uuid = "8F079083-2A6E-4B60-9663-F23B483F1EBE",
                                                                position = Vector3(6.34, 14.95, -4.64),
                                                                rotation = Quaternion.FromEulerAngles(-10.87, -89.95, -100.88),
                                                                children = {
                                                                    {
                                                                        name = "mixamorig:LeftArm",
                                                                        uuid = "A0AA69EE-ECD3-475E-9131-68CD84714AD9",
                                                                        position = Vector3(0.00, 9.14, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(10.56, -1.40, 1.46),
                                                                        children = {
                                                                            {
                                                                                name = "mixamorig:LeftForeArm",
                                                                                uuid = "B070EFB3-C473-4C6B-8F14-A54160BEB4CC",
                                                                                position = Vector3(0.00, 24.57, -0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "mixamorig:LeftHand",
                                                                                        uuid = "33690EAD-41FA-490F-9162-71FC5E1475E2",
                                                                                        position = Vector3(0.00, 23.10, -0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "mixamorig:LeftHandThumb1",
                                                                                                uuid = "9F00811D-BEB8-4B88-84D2-FE33F71E229A",
                                                                                                position = Vector3(-2.01, 3.66, 1.52),
                                                                                                rotation = Quaternion.FromEulerAngles(13.67, -23.08, 37.15),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:LeftHandThumb2",
                                                                                                        uuid = "E447FD90-764E-4399-B18A-6AB2F178FB77",
                                                                                                        position = Vector3(0.00, 3.30, -0.00),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:LeftHandThumb3",
                                                                                                                uuid = "AD4E2C96-1CE0-4B41-BA6C-8E49628B97B2",
                                                                                                                position = Vector3(-0.00, 3.56, 0.00),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:LeftHandThumb4",
                                                                                                                        uuid = "D081E9A4-4B95-437E-A2B6-9901A44BC9A7",
                                                                                                                        position = Vector3(0.00, 3.33, -0.00),
                                                                                                                    },
                                                                                                                }
                                                                                                            },
                                                                                                        }
                                                                                                    },
                                                                                                }
                                                                                            },
                                                                                            {
                                                                                                name = "mixamorig:LeftHandIndex1",
                                                                                                uuid = "C0F6D4A4-2542-4E9F-877C-7BA0DA8851DC",
                                                                                                position = Vector3(-2.33, 11.10, 0.23),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.28),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:LeftHandIndex2",
                                                                                                        uuid = "5F95BC3B-6D06-4786-B6DF-0B951F3E7E18",
                                                                                                        position = Vector3(0.00, 2.60, 0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.25),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:LeftHandIndex3",
                                                                                                                uuid = "779FF783-E171-4E7A-84B5-A417B43A0839",
                                                                                                                position = Vector3(-0.00, 2.00, -0.00),
                                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.11),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:LeftHandIndex4",
                                                                                                                        uuid = "59F67D53-E46A-45E5-A81F-E644F2BB624B",
                                                                                                                        position = Vector3(0.00, 2.46, 0.00),
                                                                                                                    },
                                                                                                                }
                                                                                                            },
                                                                                                        }
                                                                                                    },
                                                                                                }
                                                                                            },
                                                                                            {
                                                                                                name = "mixamorig:LeftHandMiddle1",
                                                                                                uuid = "EA8ECF27-8328-4A3A-B16D-57683D5A206E",
                                                                                                position = Vector3(-0.00, 11.45, -0.00),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.04),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:LeftHandMiddle2",
                                                                                                        uuid = "38ABFE77-907F-4EC2-9D5E-D4B6FD429D7C",
                                                                                                        position = Vector3(0.00, 2.90, 0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.01),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:LeftHandMiddle3",
                                                                                                                uuid = "DA105DB4-29A9-44D7-970D-886E59F085BC",
                                                                                                                position = Vector3(0.00, 2.00, -0.00),
                                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.04),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:LeftHandMiddle4",
                                                                                                                        uuid = "CD156F1F-6E05-4B07-B7DD-7CF634554CA1",
                                                                                                                        position = Vector3(-0.00, 2.29, -0.00),
                                                                                                                    },
                                                                                                                }
                                                                                                            },
                                                                                                        }
                                                                                                    },
                                                                                                }
                                                                                            },
                                                                                            {
                                                                                                name = "mixamorig:LeftHandRing1",
                                                                                                uuid = "038A3B7D-07F1-4F34-8D3F-077F4A5CA575",
                                                                                                position = Vector3(2.25, 10.61, -0.27),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:LeftHandRing2",
                                                                                                        uuid = "5D08C789-F0B6-4B3F-AAA5-9FEE398FC160",
                                                                                                        position = Vector3(0.00, 3.00, 0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.08),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:LeftHandRing3",
                                                                                                                uuid = "21B3CF2A-B5B1-4250-85B9-F4C24CD8A1B4",
                                                                                                                position = Vector3(0.00, 2.20, -0.00),
                                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.09),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:LeftHandRing4",
                                                                                                                        uuid = "85749760-D0AE-4E1F-A4D1-31ABC50BBDB8",
                                                                                                                        position = Vector3(0.00, 2.19, 0.00),
                                                                                                                    },
                                                                                                                }
                                                                                                            },
                                                                                                        }
                                                                                                    },
                                                                                                }
                                                                                            },
                                                                                            {
                                                                                                name = "mixamorig:LeftHandPinky1",
                                                                                                uuid = "5D1EE125-9A1F-472E-8522-74E98BFEDD57",
                                                                                                position = Vector3(4.03, 9.63, 0.34),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.02),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:LeftHandPinky2",
                                                                                                        uuid = "1ED2C689-F1A8-4C6A-AD58-A8AF70E30176",
                                                                                                        position = Vector3(0.00, 2.90, 0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.10),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:LeftHandPinky3",
                                                                                                                uuid = "94978125-E2A7-4181-B7B4-3B75A37AB291",
                                                                                                                position = Vector3(-0.00, 1.30, -0.00),
                                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.10),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:LeftHandPinky4",
                                                                                                                        uuid = "68FAFE06-6C5E-42D3-A222-006B8056C433",
                                                                                                                        position = Vector3(-0.00, 2.01, -0.00),
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
                                                                name = "mixamorig:RightShoulder",
                                                                uuid = "A3EC7C3A-EB28-4791-B5FA-CC662AFCA5AF",
                                                                position = Vector3(-6.34, 14.95, -4.64),
                                                                rotation = Quaternion.FromEulerAngles(-10.87, 89.95, 100.88),
                                                                children = {
                                                                    {
                                                                        name = "mixamorig:RightArm",
                                                                        uuid = "F7892167-C8F8-4CFE-9F5B-3641B6B9814E",
                                                                        position = Vector3(-0.00, 9.14, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(10.56, 1.40, -1.46),
                                                                        children = {
                                                                            {
                                                                                name = "mixamorig:RightForeArm",
                                                                                uuid = "B92388A7-7251-488C-8905-1EA9396B6789",
                                                                                position = Vector3(0.00, 24.57, -0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "mixamorig:RightHand",
                                                                                        uuid = "85185F6E-923B-479D-9433-8A05FEC3FC03",
                                                                                        position = Vector3(-0.00, 23.10, 0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "mixamorig:RightHandThumb1",
                                                                                                uuid = "B4613E40-45A0-4491-9522-28FEE313CA5F",
                                                                                                position = Vector3(2.01, 3.66, 1.52),
                                                                                                rotation = Quaternion.FromEulerAngles(14.06, 22.80, -37.01),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:RightHandThumb2",
                                                                                                        uuid = "7A7E5A1F-AFBD-4AC2-9912-B0DD98EA5398",
                                                                                                        position = Vector3(0.00, 3.30, -0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(-0.01, -0.00, 0.00),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:RightHandThumb3",
                                                                                                                uuid = "E70728E2-6126-4F26-A9D7-92E1564EBFF7",
                                                                                                                position = Vector3(0.00, 3.56, -0.00),
                                                                                                                rotation = Quaternion.FromEulerAngles(0.01, 0.00, 0.00),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:RightHandThumb4",
                                                                                                                        uuid = "61C4C9A4-0420-400B-B5BE-CB21E06D26E1",
                                                                                                                        position = Vector3(0.00, 3.33, -0.00),
                                                                                                                    },
                                                                                                                }
                                                                                                            },
                                                                                                        }
                                                                                                    },
                                                                                                }
                                                                                            },
                                                                                            {
                                                                                                name = "mixamorig:RightHandIndex1",
                                                                                                uuid = "767517D7-15E7-4B2F-AFC3-EFCBB2473822",
                                                                                                position = Vector3(2.33, 11.10, 0.23),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.28),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:RightHandIndex2",
                                                                                                        uuid = "E8EC1C32-F9F8-4420-8B8B-CD1C8988B9FD",
                                                                                                        position = Vector3(0.00, 2.60, -0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.25),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:RightHandIndex3",
                                                                                                                uuid = "DD577AC9-D0FF-4A3F-9A5F-07E44A370167",
                                                                                                                position = Vector3(0.00, 2.00, -0.00),
                                                                                                                rotation = Quaternion.FromEulerAngles(0.02, -0.00, -0.11),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:RightHandIndex4",
                                                                                                                        uuid = "2694E19D-EE1F-4A16-8329-C14A7A160705",
                                                                                                                        position = Vector3(-0.00, 2.46, -0.00),
                                                                                                                    },
                                                                                                                }
                                                                                                            },
                                                                                                        }
                                                                                                    },
                                                                                                }
                                                                                            },
                                                                                            {
                                                                                                name = "mixamorig:RightHandMiddle1",
                                                                                                uuid = "9AA675F0-7648-4102-AAAA-28BD716D2BF3",
                                                                                                position = Vector3(0.00, 11.45, -0.00),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.04),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:RightHandMiddle2",
                                                                                                        uuid = "1E608B43-2A2A-40A0-A5BA-6DCB09DE7939",
                                                                                                        position = Vector3(0.00, 2.90, 0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.01),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:RightHandMiddle3",
                                                                                                                uuid = "C0D7BD15-6BEF-478B-A759-D7243CBC8FB9",
                                                                                                                position = Vector3(0.00, 2.00, 0.00),
                                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.04),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:RightHandMiddle4",
                                                                                                                        uuid = "548C35F6-DD98-4F71-93ED-6C9FC114FC4A",
                                                                                                                        position = Vector3(-0.00, 2.29, 0.00),
                                                                                                                    },
                                                                                                                }
                                                                                                            },
                                                                                                        }
                                                                                                    },
                                                                                                }
                                                                                            },
                                                                                            {
                                                                                                name = "mixamorig:RightHandRing1",
                                                                                                uuid = "EE24CD8B-574D-4F47-A060-9F7E1ADA1E75",
                                                                                                position = Vector3(-2.25, 10.61, -0.27),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:RightHandRing2",
                                                                                                        uuid = "867DA2C0-8203-42B6-994D-FC6000929FD6",
                                                                                                        position = Vector3(-0.00, 3.00, 0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.08),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:RightHandRing3",
                                                                                                                uuid = "67358342-A0E8-4404-8BD8-D22D70748143",
                                                                                                                position = Vector3(-0.00, 2.20, 0.00),
                                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.09),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:RightHandRing4",
                                                                                                                        uuid = "E5875697-6DE0-4DE5-B2A8-A49C122599BF",
                                                                                                                        position = Vector3(0.00, 2.19, -0.00),
                                                                                                                    },
                                                                                                                }
                                                                                                            },
                                                                                                        }
                                                                                                    },
                                                                                                }
                                                                                            },
                                                                                            {
                                                                                                name = "mixamorig:RightHandPinky1",
                                                                                                uuid = "7DB7D46B-571E-4602-9E78-4C1FE334B1B9",
                                                                                                position = Vector3(-4.03, 9.63, 0.34),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.02),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "mixamorig:RightHandPinky2",
                                                                                                        uuid = "9D93716A-2FA7-4FCE-B153-C6FCB39613BC",
                                                                                                        position = Vector3(-0.00, 2.90, 0.00),
                                                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.10),
                                                                                                        children = {
                                                                                                            {
                                                                                                                name = "mixamorig:RightHandPinky3",
                                                                                                                uuid = "D820AB68-DA93-43BD-A4B1-2B9945C45B03",
                                                                                                                position = Vector3(-0.00, 1.30, 0.00),
                                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.10),
                                                                                                                children = {
                                                                                                                    {
                                                                                                                        name = "mixamorig:RightHandPinky4",
                                                                                                                        uuid = "85767DA3-539A-4210-8E56-5705DBA9BA60",
                                                                                                                        position = Vector3(-0.00, 2.01, 0.00),
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
                                                        }
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "mixamorig:LeftUpLeg",
                                        uuid = "FDB86573-787B-4E8E-9431-7055AF56C3CC",
                                        position = Vector3(9.69, -5.69, -1.69),
                                        rotation = Quaternion.FromEulerAngles(1.97, -0.00, 180.00),
                                        children = {
                                            {
                                                name = "mixamorig:LeftLeg",
                                                uuid = "8256479F-9BBF-42B5-A419-4BCC250CA6B9",
                                                position = Vector3(-0.00, 43.17, -0.00),
                                                rotation = Quaternion.FromEulerAngles(-8.00, -0.00, 0.00),
                                                children = {
                                                    {
                                                        name = "mixamorig:LeftFoot",
                                                        uuid = "C4679F0E-6B5A-4CDA-A6B5-7B51C0B8B483",
                                                        position = Vector3(0.00, 40.82, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(-34.50, 1.62, 0.40),
                                                        children = {
                                                            {
                                                                name = "mixamorig:LeftToeBase",
                                                                uuid = "49416134-C362-480D-A843-27C2EF3AA04D",
                                                                position = Vector3(0.00, 18.00, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-59.78, -18.66, -18.73),
                                                                children = {
                                                                    {
                                                                        name = "mixamorig:LeftToe_End",
                                                                        uuid = "FBA2592E-E28D-4188-A1FB-B305DA96A250",
                                                                        position = Vector3(0.00, 13.77, -0.00),
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
                                        name = "mixamorig:RightUpLeg",
                                        uuid = "629668F3-AE58-42C6-B3F7-C708AA7EC15C",
                                        position = Vector3(-9.69, -5.69, -1.69),
                                        rotation = Quaternion.FromEulerAngles(1.97, -0.00, 180.00),
                                        children = {
                                            {
                                                name = "mixamorig:RightLeg",
                                                uuid = "CC42D6C0-29DB-4D34-9FC0-60161D66F348",
                                                position = Vector3(-0.00, 43.17, -0.00),
                                                rotation = Quaternion.FromEulerAngles(-8.00, -0.00, 0.00),
                                                children = {
                                                    {
                                                        name = "mixamorig:RightFoot",
                                                        uuid = "7A80B74F-EA09-4537-917E-3D8868EA0136",
                                                        position = Vector3(0.00, 40.82, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(-34.50, -1.62, -0.40),
                                                        children = {
                                                            {
                                                                name = "mixamorig:RightToeBase",
                                                                uuid = "3434220A-3088-46D8-9B0F-C2BCA301CA63",
                                                                position = Vector3(-0.00, 18.00, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-61.48, -0.15, 2.17),
                                                                children = {
                                                                    {
                                                                        name = "mixamorig:RightToe_End",
                                                                        uuid = "98E1D24C-8C53-4977-B2EF-C08071F85BFB",
                                                                        position = Vector3(-0.00, 13.77, -0.00),
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
                                name = "Maria_J_J_Ong",
                                uuid = "4E1CFEBA-D238-4A2D-8B43-8417A0949530",
                                position = Vector3(0.00, 19.21, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/Models/Player.fbx",
                                        textureRef = "Assets/Models/Player_MariaMat.texture",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "Maria_sword",
                                uuid = "60C5850B-BB79-4F3A-BF1D-8FEF0CC8ACB3",
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/Models/Player.fbx",
                                        textureRef = "Assets/Models/Player_MariaMat.texture",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 1,
                                        multiMesh = false
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "MainCamera",
                        uuid = "0E7CE370-7C0E-45A8-89CA-AD7A3E70C216",
                        position = Vector3(0.00, 9.63, -7.07),
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
