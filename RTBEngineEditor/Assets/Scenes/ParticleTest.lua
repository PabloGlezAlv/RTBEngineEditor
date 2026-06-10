function CreateScene()
    return {
        name = "Particle Test",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "A1000001-0000-4000-8000-000000000001",
                position = Vector3(0.00, 3.00, -8.00),
                rotation = Quaternion.FromEulerAngles(15.00, 0.00, 0.00),
                components = {
                    {
                        type = "CameraComponent",
                        fov = 60.00,
                        nearClip = 0.10,
                        farClip = 100.00,
                        projectionType = "Perspective",
                        orthographicSize = 5.00,
                        syncWithTransform = true,
                        isMainCamera = true
                    },
                }
            },
            {
                name = "Directional Light",
                uuid = "A1000002-0000-4000-8000-000000000002",
                rotation = Quaternion.FromEulerAngles(50.00, -30.00, 0.00),
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
                name = "Particle System",
                uuid = "A1000003-0000-4000-8000-000000000003",
                position = Vector3(0.00, 1.00, 0.00),
                components = {
                    {
                        type = "ParticleSystem",
                        maxParticles = 256,
                        emissionRate = 40.00,
                        emitterShape = "Cone",
                        shapeRadius = 0.50,
                        coneAngle = 25.00,
                        boxSize = Vector3(1.00, 1.00, 1.00),
                        startLifetime = 1.50,
                        startSpeed = 2.00,
                        startSize = 0.30,
                        endSize = 0.05,
                        startColor = Color(0.75, 0.75, 0.75, 0.85),
                        endColor = Color(0.50, 0.50, 0.50, 0.00),
                        gravity = Vector3(0.00, -2.00, 0.00),
                        worldSimulation = true,
                        visible = true,
                        loop = true,
                        playOnAwake = true,
                        simulateInEditMode = true,
                        burstCount = 10
                    },
                }
            },
        }
    }
end
