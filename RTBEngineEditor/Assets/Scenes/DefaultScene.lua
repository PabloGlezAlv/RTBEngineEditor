function CreateScene()
    return {
        name = "Test Scene",
        skyboxEnabled = true,
        gameObjects = {
            {
                name = "MainCamera",
                uuid = "0E7CE370-7C0E-45A8-89CA-AD7A3E70C216",
                position = Vector3(0.00, 1.00, 5.00),
                rotation = Quaternion.FromEulerAngles(-0.00, 3.14, -0.00),
                components = {
                    {
                        type = "CameraComponent",
                        fov = 45.00,
                        nearClip = 0.10,
                        farClip = 100.00,
                        projectionType = 0,
                        orthographicSize = 10.00,
                        syncWithTransform = true,
                        isMainCamera = false
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
                        color = Vector4(1.00, 1.00, 1.00, 1.00),
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
                        color = Vector4(0.20, 0.50, 1.00, 1.00),
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
                        color = Vector4(1.00, 0.00, 0.00, 50.00),
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
                        textureRef = "",
                        colorRef = Vector4(1.00, 1.00, 1.00, 1.00)
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
                        textureRef = "",
                        colorRef = Vector4(1.00, 1.00, 1.00, 1.00)
                    },
                }
            },
            {
                name = "Character",
                uuid = "E3D87585-8C8B-4639-9B1B-520E44A062F9",
                position = Vector3(-0.22, 0.00, 0.00),
                scale = Vector3(0.01, 0.01, 0.01),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "",
                        textureRef = "",
                        colorRef = Vector4(1.00, 1.00, 1.00, 1.00)
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
        }
    }
end
