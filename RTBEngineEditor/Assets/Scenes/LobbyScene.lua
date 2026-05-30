function CreateScene()
    return {
        name = "LobbyScene",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "AA4B84E7-4DBA-4F4B-89D4-5B84E5C1F001",
                position = Vector3(0.00, 1.00, -6.00),
                components = {
                    {
                        type = "CameraComponent",
                        fov = 60.00,
                        nearClip = 0.10,
                        farClip = 1000.00,
                        projectionType = 0,
                        orthographicSize = 10.00,
                        syncWithTransform = true,
                        isMainCamera = true
                    },
                }
            },
            {
                name = "Directional Light",
                uuid = "9F2A3B3B-FD20-41CC-8452-86B5322C7001",
                rotation = Quaternion.FromEulerAngles(-10.00, -35.00, 0.00),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        color = Color(1.00, 1.00, 1.00, 1.00),
                        intensity = 0.85,
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
                uuid = "1B12E0D8-20EE-4B4F-9014-5D6A0D694001",
                components = {
                    {
                        type = "Canvas",
                        renderMode = "ScreenSpaceOverlay",
                        canvasSize = Vector2(1920.00, 1080.00),
                        sortOrder = 0
                    },
                    {
                        type = "CursorUnlocker"
                    },
                    {
                        type = "LobbyMenuController",
                        statusText = "C2786523-71A6-4329-AC85-55D5A5CB4001/UIText",
                        lobbyIdText = "E6727C41-8D17-45C9-9A26-A1D133A94001/UIText",
                        playerCountText = "2D0F2F5B-8CF3-45C2-9918-B4D177294001/UIText",
                        eventLogText = "F8A91C20-4E2B-4D9A-8C15-2B6E4D890001/UIText",
                        joinHintText = "2BE1B91B-BE70-44B7-8E8B-923EA89F4001/UIText",
                        createButton = "F510EC7C-B006-494D-A1B1-A0EC72854001/UIButton",
                        joinButton = "1A33655A-FD08-4408-8B67-617BA5C94001/UIButton",
                        copyLobbyIdButton = "D37CC43E-2B4F-42A1-A91F-E0830D7C4001/UIButton",
                        finishButton = "B46F861C-FEA4-47B8-A8D6-BAB71A784001/UIButton",
                        startGameButton = "B031EEDB-A10E-42A7-845B-C5B673134001/UIButton",
                        gameScenePath = "Assets/Scenes/DefaultScene.lua",
                        lobbyBucketId = "RTBEngine",
                        joinLobbyId = "",
                        maxMembers = 6,
                        autoLoginOnStart = true
                    },
                },
                children = {
                    {
                        name = "Background",
                        uuid = "01AE71B1-F3F5-42DB-B9F9-855278184001",
                        components = {
                            {
                                type = "UIImage",
                                texture = "Assets/UI/MainMenuBG.png",
                                tintColor = Color(0.38, 0.43, 0.58, 1.00),
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
                        name = "Panel",
                        uuid = "0636133F-EA6E-4809-A7EE-73F773A24001",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.08, 0.09, 0.12, 0.88),
                                borderColor = Color(0.62, 0.69, 0.94, 1.00),
                                borderThickness = 2.00,
                                hasBorder = true,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 0.00),
                                sizeDelta = Vector2(720.00, 660.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        },
                        children = {
                            {
                                name = "Title",
                                uuid = "F41A6476-1872-40AD-859F-5FB733994001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Online Lobby",
                                        color = Color(1.00, 1.00, 1.00, 1.00),
                                        fontSize = 42.00,
                                        alignment = "Center",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(0.00, -44.00),
                                        sizeDelta = Vector2(560.00, 58.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "StatusText",
                                uuid = "C2786523-71A6-4329-AC85-55D5A5CB4001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Choose how to enter the lobby.",
                                        color = Color(0.86, 0.90, 1.00, 1.00),
                                        fontSize = 20.00,
                                        alignment = "Center",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(0.00, -122.00),
                                        sizeDelta = Vector2(620.00, 92.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "LobbyIdText",
                                uuid = "E6727C41-8D17-45C9-9A26-A1D133A94001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Lobby ID: None",
                                        color = Color(0.95, 0.86, 0.54, 1.00),
                                        fontSize = 18.00,
                                        alignment = "Center",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(-82.00, -226.00),
                                        sizeDelta = Vector2(470.00, 34.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "CopyLobbyIdButton",
                                uuid = "D37CC43E-2B4F-42A1-A91F-E0830D7C4001",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.26, 0.28, 0.38, 1.00),
                                        borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                        borderThickness = 1.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(252.00, -224.00),
                                        sizeDelta = Vector2(136.00, 34.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                    {
                                        type = "UIButton",
                                        normalColor = Color(1.00, 1.00, 1.00, 0.23),
                                        hoveredColor = Color(0.90, 0.90, 0.90, 0.44),
                                        pressedColor = Color(0.70, 0.70, 0.70, 1.00),
                                        disabledColor = Color(0.50, 0.50, 0.50, 0.50),
                                        interactable = false,
                                        enableDefaultHoverVisuals = false
                                    },
                                    {
                                        type = "ButtonStyle",
                                        backgroundPanel = "D37CC43E-2B4F-42A1-A91F-E0830D7C4001/UIPanel",
                                        label = "91675087-65CA-4A28-958F-E8F7C7214001/UIText",
                                        normalPanelColor = Color(0.26, 0.28, 0.38, 1.00),
                                        normalTextColor = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverPanelColor = Color(0.39, 0.43, 0.62, 1.00),
                                        hoverTextColor = Color(1.00, 0.96, 0.68, 1.00),
                                        hoverScaleBoost = 1.04,
                                        hoverRotationDeg = 0.00,
                                        clickPanelColor = Color(0.18, 0.20, 0.31, 1.00),
                                        clickTextColor = Color(0.86, 0.90, 1.00, 1.00),
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
                                        uuid = "91675087-65CA-4A28-958F-E8F7C7214001",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Copy ID",
                                                color = Color(1.00, 1.00, 1.00, 1.00),
                                                fontSize = 15.00,
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
                                name = "PlayerCountText",
                                uuid = "2D0F2F5B-8CF3-45C2-9918-B4D177294001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Players: 0 / 6",
                                        color = Color(0.78, 0.94, 0.80, 1.00),
                                        fontSize = 18.00,
                                        alignment = "Center",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(0.00, -262.00),
                                        sizeDelta = Vector2(640.00, 28.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "EventLogText",
                                uuid = "F8A91C20-4E2B-4D9A-8C15-2B6E4D890001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "",
                                        color = Color(0.82, 0.86, 0.94, 1.00),
                                        fontSize = 15.00,
                                        alignment = "Left",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(0.00, -300.00),
                                        sizeDelta = Vector2(640.00, 96.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "JoinHintText",
                                uuid = "2BE1B91B-BE70-44B7-8E8B-923EA89F4001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Type or paste a Lobby ID, then press Join Lobby.",
                                        color = Color(0.72, 0.78, 0.90, 1.00),
                                        fontSize = 16.00,
                                        alignment = "Center",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(0.00, -404.00),
                                        sizeDelta = Vector2(640.00, 30.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "CreateLobbyButton",
                                uuid = "F510EC7C-B006-494D-A1B1-A0EC72854001",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.15, 0.28, 0.47, 1.00),
                                        borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                        borderThickness = 1.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(-165.00, -442.00),
                                        sizeDelta = Vector2(300.00, 54.00),
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
                                        backgroundPanel = "F510EC7C-B006-494D-A1B1-A0EC72854001/UIPanel",
                                        label = "235D9940-2514-42BC-B668-6D3848084001/UIText",
                                        normalPanelColor = Color(0.15, 0.28, 0.47, 1.00),
                                        normalTextColor = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverPanelColor = Color(0.28, 0.42, 0.72, 1.00),
                                        hoverTextColor = Color(1.00, 0.96, 0.68, 1.00),
                                        hoverScaleBoost = 1.04,
                                        hoverRotationDeg = 0.00,
                                        clickPanelColor = Color(0.11, 0.22, 0.40, 1.00),
                                        clickTextColor = Color(0.86, 0.90, 1.00, 1.00),
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
                                        uuid = "235D9940-2514-42BC-B668-6D3848084001",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Create Lobby",
                                                color = Color(1.00, 1.00, 1.00, 1.00),
                                                fontSize = 18.00,
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
                                name = "JoinLobbyButton",
                                uuid = "1A33655A-FD08-4408-8B67-617BA5C94001",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.20, 0.21, 0.30, 1.00),
                                        borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                        borderThickness = 1.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(165.00, -442.00),
                                        sizeDelta = Vector2(300.00, 54.00),
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
                                        backgroundPanel = "1A33655A-FD08-4408-8B67-617BA5C94001/UIPanel",
                                        label = "7B43110F-D650-42B2-AE35-B79214844001/UIText",
                                        normalPanelColor = Color(0.20, 0.21, 0.30, 1.00),
                                        normalTextColor = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverPanelColor = Color(0.34, 0.36, 0.54, 1.00),
                                        hoverTextColor = Color(1.00, 0.96, 0.68, 1.00),
                                        hoverScaleBoost = 1.04,
                                        hoverRotationDeg = 0.00,
                                        clickPanelColor = Color(0.12, 0.13, 0.24, 1.00),
                                        clickTextColor = Color(0.86, 0.90, 1.00, 1.00),
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
                                        uuid = "7B43110F-D650-42B2-AE35-B79214844001",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Join Lobby",
                                                color = Color(1.00, 1.00, 1.00, 1.00),
                                                fontSize = 18.00,
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
                                name = "FinishLobbyButton",
                                uuid = "B46F861C-FEA4-47B8-A8D6-BAB71A784001",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.43, 0.18, 0.20, 1.00),
                                        borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                        borderThickness = 1.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(-165.00, -516.00),
                                        sizeDelta = Vector2(300.00, 54.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                    {
                                        type = "UIButton",
                                        normalColor = Color(1.00, 1.00, 1.00, 0.23),
                                        hoveredColor = Color(0.90, 0.90, 0.90, 0.44),
                                        pressedColor = Color(0.70, 0.70, 0.70, 1.00),
                                        disabledColor = Color(0.50, 0.50, 0.50, 0.50),
                                        interactable = false,
                                        enableDefaultHoverVisuals = false
                                    },
                                    {
                                        type = "ButtonStyle",
                                        backgroundPanel = "B46F861C-FEA4-47B8-A8D6-BAB71A784001/UIPanel",
                                        label = "CC6F3E5A-B403-4FA7-82C4-D0222F4E4001/UIText",
                                        normalPanelColor = Color(0.43, 0.18, 0.20, 1.00),
                                        normalTextColor = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverPanelColor = Color(0.66, 0.27, 0.31, 1.00),
                                        hoverTextColor = Color(1.00, 0.96, 0.68, 1.00),
                                        hoverScaleBoost = 1.04,
                                        hoverRotationDeg = 0.00,
                                        clickPanelColor = Color(0.31, 0.11, 0.14, 1.00),
                                        clickTextColor = Color(0.96, 0.86, 0.86, 1.00),
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
                                        uuid = "CC6F3E5A-B403-4FA7-82C4-D0222F4E4001",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Finish Lobby",
                                                color = Color(1.00, 1.00, 1.00, 1.00),
                                                fontSize = 18.00,
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
                                name = "StartGameButton",
                                uuid = "B031EEDB-A10E-42A7-845B-C5B673134001",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.14, 0.40, 0.29, 1.00),
                                        borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                        borderThickness = 1.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(165.00, -516.00),
                                        sizeDelta = Vector2(300.00, 54.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                    {
                                        type = "UIButton",
                                        normalColor = Color(1.00, 1.00, 1.00, 0.23),
                                        hoveredColor = Color(0.90, 0.90, 0.90, 0.44),
                                        pressedColor = Color(0.70, 0.70, 0.70, 1.00),
                                        disabledColor = Color(0.50, 0.50, 0.50, 0.50),
                                        interactable = false,
                                        enableDefaultHoverVisuals = false
                                    },
                                    {
                                        type = "ButtonStyle",
                                        backgroundPanel = "B031EEDB-A10E-42A7-845B-C5B673134001/UIPanel",
                                        label = "CB8F00A0-9E25-414F-8DC5-5052840B4001/UIText",
                                        normalPanelColor = Color(0.14, 0.40, 0.29, 1.00),
                                        normalTextColor = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverPanelColor = Color(0.24, 0.59, 0.43, 1.00),
                                        hoverTextColor = Color(1.00, 0.96, 0.68, 1.00),
                                        hoverScaleBoost = 1.04,
                                        hoverRotationDeg = 0.00,
                                        clickPanelColor = Color(0.10, 0.29, 0.20, 1.00),
                                        clickTextColor = Color(0.86, 0.96, 0.90, 1.00),
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
                                        uuid = "CB8F00A0-9E25-414F-8DC5-5052840B4001",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Start Game",
                                                color = Color(1.00, 1.00, 1.00, 1.00),
                                                fontSize = 18.00,
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
                                uuid = "85E511F8-8200-4182-80CE-2457439E4001",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.16, 0.16, 0.18, 1.00),
                                        borderColor = Color(1.00, 1.00, 1.00, 1.00),
                                        borderThickness = 1.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
                                        anchorMin = Vector2(0.50, 1.00),
                                        anchorMax = Vector2(0.50, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(0.00, -590.00),
                                        sizeDelta = Vector2(260.00, 46.00),
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
                                        backgroundPanel = "85E511F8-8200-4182-80CE-2457439E4001/UIPanel",
                                        label = "B0AA3EF1-92B3-4DD5-97C9-B7BD88954001/UIText",
                                        normalPanelColor = Color(0.16, 0.16, 0.18, 1.00),
                                        normalTextColor = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverPanelColor = Color(0.30, 0.30, 0.36, 1.00),
                                        hoverTextColor = Color(1.00, 0.96, 0.68, 1.00),
                                        hoverScaleBoost = 1.04,
                                        hoverRotationDeg = 0.00,
                                        clickPanelColor = Color(0.10, 0.10, 0.14, 1.00),
                                        clickTextColor = Color(0.86, 0.90, 1.00, 1.00),
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
                                        uuid = "B0AA3EF1-92B3-4DD5-97C9-B7BD88954001",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Back",
                                                color = Color(1.00, 1.00, 1.00, 1.00),
                                                fontSize = 17.00,
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
    }
end
