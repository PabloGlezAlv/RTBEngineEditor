function CreateScene()
    return {
        name = "Test Scene",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "MainCamera",
                uuid = "0E7CE370-7C0E-45A8-89CA-AD7A3E70C216",
                position = Vector3(-0.00, 1.00, 4.94),
                rotation = Quaternion.FromEulerAngles(-0.00, 180.00, -0.00),
                components = {
                    {
                        type = "CameraComponent",
                        fov = 45.00,
                        nearClip = 0.10,
                        farClip = 100.00,
                        projectionType = 0,
                        orthographicSize = 3.50,
                        syncWithTransform = true,
                        isMainCamera = true
                    },
                    {
                        type = "FreeLookCamera",
                        moveSpeed = 10.00,
                        lookSpeed = 0.20,
                        rotationSpeed = 90.00
                    },
                }
            },
            {
                name = "MainLight",
                uuid = "63347DB4-BEBE-4CCF-90C0-A270A9F7BF1E",
                components = {
                    {
                        type = "LightComponent",
                        lightType = 0,
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
                        lightType = 1,
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
                        lightType = 2,
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
                        type = "Canvas"
                    },
                },
                children = {
                    {
                        name = "LogoImage",
                        uuid = "A1B2C3D4-0003-4000-8000-000000000003",
                        components = {
                            {
                                type = "UIImage",
                                texture = "Assets/Textures/testTexture.png",
                                tintColor = Color(1.00, 1.00, 1.00, 1.00),
                                preserveAspect = true,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.00, 0.00),
                                anchorMax = Vector2(0.00, 0.00),
                                anchoredPosition = Vector2(198.50, 22.10),
                                sizeDelta = Vector2(25.00, 25.00)
                            },
                        }
                    },
                    {
                        name = "TitleText",
                        uuid = "A1B2C3D4-0002-4000-8000-000000000002",
                        components = {
                            {
                                type = "UIText",
                                text = "RTBEngine Demo",
                                color = Color(1.00, 1.00, 1.00, 1.00),
                                fontSize = 28.00,
                                alignment = 0,
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.00, 0.00),
                                anchorMax = Vector2(0.00, 0.00),
                                anchoredPosition = Vector2(69.80, 20.70),
                                sizeDelta = Vector2(100.00, 100.00)
                            },
                        }
                    },
                    {
                        name = "Button",
                        uuid = "8EA9A9F4-55AD-46BB-B8AF-B7CD5969EF80",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(1.00, 1.00, 1.00, 0.48),
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
                                normalColor = Color(1.00, 1.00, 1.00, 0.48),
                                hoveredColor = Color(0.90, 0.90, 0.90, 1.00),
                                pressedColor = Color(0.70, 0.70, 0.70, 1.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.50),
                                interactable = true
                            },
                        },
                        children = {
                            {
                                name = "Text",
                                uuid = "0A595D55-A62A-43F8-8CC5-B0229AB450E2",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "PULSAR",
                                        color = Color(1.00, 1.00, 1.00, 1.00),
                                        fontSize = 16.00,
                                        alignment = 1,
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
            {
                name = "Floor",
                uuid = "737688CD-BE46-4776-B38D-CA30B01FB6D1",
                position = Vector3(0.00, -0.05, 0.00),
                scale = Vector3(10.00, 0.10, 10.00),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Default/Models/cube.obj",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                }
            },
            {
                name = "Cube",
                uuid = "64B0D534-9E35-4D68-A32D-CFFCC1F5E1F3",
                position = Vector3(2.00, 1.00, 0.00),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Default/Models/cube.obj",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                }
            },
            {
                name = "Character",
                uuid = "E3D87585-8C8B-4639-9B1B-520E44A062F9",
                position = Vector3(-0.04, 0.00, 0.00),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = nil,
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "mixamo.com",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "CubeTest",
                uuid = "8FB4850C-0A3A-41D9-866E-BB98439FE7DD",
                position = Vector3(-2.03, 1.10, 0.00),
                scale = Vector3(1.22, 1.22, 1.22),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Default/Models/sphere.obj",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "NewComponent"
                    },
                }
            },
        }
    }
end
