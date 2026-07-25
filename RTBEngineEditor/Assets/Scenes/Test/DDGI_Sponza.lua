function CreateScene()
    return {
        name = "DDGI Sponza",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "DDGI-SPONZA-CAM1-4000-8000-000000000001",
                position = Vector3(-9.64, 5.86, 3.98),
                rotation = Quaternion.FromEulerAngles(-0.09, -116.35, -0.08),
                components = {
                    {
                        type = "CameraComponent",
                        fov = 70.00,
                        nearClip = 0.10,
                        farClip = 250.00,
                        projectionType = "Perspective",
                        orthographicSize = 5.00,
                        syncWithTransform = true,
                        isMainCamera = true
                    },
                    {
                        type = "FreeLookCamera",
                        moveSpeed = 6.00,
                        lookSpeed = 0.12,
                        rotationSpeed = 90.00
                    },
                }
            },
            {
                name = "Sun",
                uuid = "DDGI-SPONZA-LIT1-4000-8000-000000000002",
                position = Vector3(0.98, 12.71, -1.09),
                rotation = Quaternion.FromEulerAngles(-27.34, -3.18, -2.17),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        color = Color(1.00, 0.97, 0.92, 1.00),
                        intensity = 0.35,
                        range = 50.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        syncPosition = true,
                        syncDirection = true
                    },
                }
            },
            {
                name = "DDGI Red Bounce",
                uuid = "DDGI-SPONZA-LITR-4000-8000-000000000003",
                position = Vector3(-6.50, 2.20, 0.00),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Point",
                        color = Color(1.00, 0.15, 0.12, 1.00),
                        intensity = 4.50,
                        range = 18.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        syncPosition = true,
                        syncDirection = true
                    },
                }
            },
            {
                name = "DDGI Green Bounce",
                uuid = "DDGI-SPONZA-LITG-4000-8000-000000000004",
                position = Vector3(0.00, 2.40, 0.00),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Point",
                        color = Color(0.15, 1.00, 0.25, 1.00),
                        intensity = 4.50,
                        range = 18.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        syncPosition = true,
                        syncDirection = true
                    },
                }
            },
            {
                name = "DDGI Blue Bounce",
                uuid = "DDGI-SPONZA-LITB-4000-8000-000000000006",
                position = Vector3(6.50, 2.20, 0.00),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Point",
                        color = Color(0.20, 0.35, 1.00, 1.00),
                        intensity = 4.50,
                        range = 18.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        syncPosition = true,
                        syncDirection = true
                    },
                }
            },
            {
                name = "Sponza",
                uuid = "DDGI-SPONZA-MDL1-4000-8000-000000000005",
                position = Vector3(0.48, 1.01, 0.31),
                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 3.00),
                components = {
                    {
                        type = "MeshRenderer",
                        meshRef = "Assets/3D/Lighting/Sponza/glTF/Sponza.gltf",
                        textureRef = nil,
                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                        shaderRef = "basic",
                        shaderPropertyOverrides = "",
                        meshIndex = 0,
                        multiMesh = true
                    },
                }
            },
            {
                name = "Volume",
                uuid = "FC929949-5F05-4E09-9EC8-A4E4C5479992",
                components = {
                    {
                        type = "VolumeComponent"
                    },
                }
            },
        }
    }
end
