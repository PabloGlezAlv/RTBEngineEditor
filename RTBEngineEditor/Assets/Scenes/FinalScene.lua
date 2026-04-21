function CreateScene()
    return {
        name = "FinalScene",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "9C583539-BB4F-42EF-A694-11195B9D3B8E",
                position = Vector3(0.00, 1.00, -5.00),
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
                uuid = "33E609A2-DB15-43A6-B8D1-EB017F1606F7",
                rotation = Quaternion.FromEulerAngles(-28.02, -40.52, 21.88),
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
                uuid = "6233C2FA-C05A-4C30-B771-E1185E584E8A",
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
                        type = "FinalSceneController",
                        resultText = "D7B61DB1-BE7B-4859-B224-B33818C081C2/UIText",
                        winMessage = "You won\nThanks for playing the demo",
                        loseMessage = "You lost",
                        fallbackMessage = "Thanks for playing the demo"
                    },
                },
                children = {
                    {
                        name = "Background",
                        uuid = "A6C5C515-CE68-4F34-AFC5-44DA266B0F12",
                        components = {
                            {
                                type = "UIImage",
                                texture = "Assets/UI/EndGameBG.png",
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
                        name = "ResultText",
                        uuid = "D7B61DB1-BE7B-4859-B224-B33818C081C2",
                        components = {
                            {
                                type = "UIText",
                                text = "Thanks for playing the demo",
                                color = Color(1.00, 1.00, 1.00, 1.00),
                                fontSize = 44.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 105.00),
                                sizeDelta = Vector2(920.00, 150.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "BackToMenuButton",
                        uuid = "C48D3773-B4BA-40D0-A211-F66A1F0579C9",
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
                                anchoredPosition = Vector2(0.00, -105.00),
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
                                backgroundPanel = "C48D3773-B4BA-40D0-A211-F66A1F0579C9/UIPanel",
                                label = "47563C6B-497E-4D65-BA86-17F69EFD0D6D/UIText",
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
                                scenePath = "Assets/Scenes/MainMenu.lua"
                            },
                        },
                        children = {
                            {
                                name = "Text",
                                uuid = "47563C6B-497E-4D65-BA86-17F69EFD0D6D",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Back to Menu",
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
                }
            },
        }
    }
end
