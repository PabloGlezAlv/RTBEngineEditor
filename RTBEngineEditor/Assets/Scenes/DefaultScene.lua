function CreateScene()
    return {
        name = "Test Scene",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "MainCamera",
                uuid = "0E7CE370-7C0E-45A8-89CA-AD7A3E70C216",
                position = Vector3(-0.00, 0.35, 4.99),
                rotation = Quaternion.FromEulerAngles(-0.00, 180.00, -0.00),
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
                                alignment = "Left",
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
                                        alignment = "Left",
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
                position = Vector3(-0.46, -0.05, 0.00),
                scale = Vector3(10.00, 0.10, 10.00),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Default/Models/cube.obj",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
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
                        bodyType = "Dynamic"
                    },
                    {
                        type = "Connector",
                        targetRef = "8FB4850C-0A3A-41D9-866E-BB98439FE7DD"
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
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "SphereTest",
                uuid = "8FB4850C-0A3A-41D9-866E-BB98439FE7DD",
                position = Vector3(-4.57, 1.10, 1.15),
                scale = Vector3(1.22, 1.22, 1.22),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Default/Models/sphere.obj",
                        textureRef = "Assets/Textures/testTexture.png",
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "RigidBodyComponent",
                        mass = 1.00,
                        friction = 0.50,
                        restitution = 0.10,
                        bodyType = "Dynamic"
                    },
                    {
                        type = "Connectable"
                    },
                    {
                        type = "SphereColliderComponent",
                        radius = 0.80,
                        centerOffset = Vector3(0.00, 0.00, 0.00),
                        isTrigger = false
                    },
                }
            },
            {
                name = "CharacterMultiAnim",
                uuid = "C2B03D21-BCCB-4623-BDAE-957423F3BE39",
                position = Vector3(2.00, 0.00, 2.00),
                rotation = Quaternion.FromEulerAngles(0.00, 69.24, -0.00),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Attack",
                        defaultClip = "Hit",
                        speed = 0.74,
                        playing = true,
                        looping = true,
                        additionalModels = {
                            "Assets/Models/attack.fbx",
                            "Assets/Models/hit.fbx",
                        }
                    },
                }
            },
            {
                name = "Character",
                uuid = "01164FFD-CC4A-4AC5-B136-38D65E6494B7",
                position = Vector3(-1.94, 0.00, 0.00),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "A37A22DA-E9E6-43B7-BCFB-7194EDFDFBA7",
                position = Vector3(-3.04, 0.00, 0.00),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "992181D6-F860-4EE5-8CC8-126E21520808",
                position = Vector3(-4.64, 0.00, 0.00),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "E05A347A-C148-4859-BBCE-2F44B5400156",
                position = Vector3(-6.34, 0.00, 0.00),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "F713E660-94AA-4137-BE38-953ABAB23ABF",
                position = Vector3(-6.34, 0.00, -0.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "1DE66719-F406-4EC7-8437-14FA8C78DB27",
                position = Vector3(-5.64, 0.00, -0.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "272722B5-4BD2-493E-BD56-6228722ED5FE",
                position = Vector3(-4.04, 0.00, -0.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "1AE1CC90-6840-4236-9AA9-5D4CC20A0E40",
                position = Vector3(-2.84, 0.00, -0.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "3EB536E7-9522-4CD3-9A76-5EAD539E59B0",
                position = Vector3(-1.94, 0.00, -0.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "E7B2DA5F-BF1D-4356-9D2C-3B655966244A",
                position = Vector3(-0.34, 0.00, -0.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "02FD3CC1-CB76-42B7-AC31-D495BB12ECE1",
                position = Vector3(-0.34, 0.00, -1.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "3A43E170-DEE4-4526-BD08-C21A39812EA2",
                position = Vector3(-1.34, 0.00, -1.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "49F491A5-3B69-49E3-91E2-97A79EF46287",
                position = Vector3(-2.14, 0.00, -1.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "0875B2C0-12BC-41D7-8DCA-9C6D9B87574F",
                position = Vector3(-3.14, 0.00, -1.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "2F390F59-F275-4F35-81B5-52C55E454710",
                position = Vector3(-4.14, 0.00, -1.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "4034DF85-779B-458C-8B32-6E5C0BA521A9",
                position = Vector3(-4.94, 0.00, -1.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "7E57FDCA-EA6C-4896-BA15-085E3FA28B57",
                position = Vector3(-6.14, 0.00, -1.70),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "61394A72-98DA-4B43-A92A-547220F3F5F7",
                position = Vector3(-5.94, 0.00, -3.10),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "56709BD3-B970-4194-8781-92D8FDFF3856",
                position = Vector3(-4.04, 0.00, -3.10),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "SphereTest",
                uuid = "7BD36968-3613-49E3-B986-F233E5710893",
                position = Vector3(-2.64, 1.10, 1.15),
                scale = Vector3(1.22, 1.22, 1.22),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Default/Models/sphere.obj",
                        textureRef = "Assets/Textures/testTexture.png",
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "RigidBodyComponent",
                        mass = 1.00,
                        friction = 0.50,
                        restitution = 0.10,
                        bodyType = "Dynamic"
                    },
                    {
                        type = "Connectable"
                    },
                    {
                        type = "SphereColliderComponent",
                        radius = 0.80,
                        centerOffset = Vector3(0.00, 0.00, 0.00),
                        isTrigger = false
                    },
                }
            },
            {
                name = "SphereTest",
                uuid = "437310A5-043C-440D-8A9C-A4B6ED30D59F",
                position = Vector3(-3.97, 1.10, 2.45),
                scale = Vector3(1.22, 1.22, 1.22),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Default/Models/sphere.obj",
                        textureRef = "Assets/Textures/testTexture.png",
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "RigidBodyComponent",
                        mass = 1.00,
                        friction = 0.50,
                        restitution = 0.10,
                        bodyType = "Dynamic"
                    },
                    {
                        type = "Connectable"
                    },
                    {
                        type = "SphereColliderComponent",
                        radius = 0.80,
                        centerOffset = Vector3(0.00, 0.00, 0.00),
                        isTrigger = false
                    },
                }
            },
            {
                name = "Character",
                uuid = "A490A324-479F-4598-9068-90885A9105EC",
                position = Vector3(-2.94, 0.00, -3.10),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "079EE1EA-23DE-4E61-86A8-FD9B2AA26A59",
                position = Vector3(-1.34, 0.00, -3.10),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
            {
                name = "Character",
                uuid = "451D1C0F-051B-449A-9666-ECD1E65C62F2",
                position = Vector3(0.36, 0.00, -3.10),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/Models/walking.fbx",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00)
                    },
                    {
                        type = "Animator",
                        modelRef = "Assets/Models/walking.fbx",
                        currentClipName = "Walking",
                        defaultClip = "Walking",
                        speed = 1.00,
                        playing = true,
                        looping = true
                    },
                }
            },
        }
    }
end
