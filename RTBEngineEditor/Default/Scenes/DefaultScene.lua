function CreateScene()
    return {
        name = "Default Scene",
        gameObjects = {
            -- Main Camera
            {
                name = "MainCamera",
                position = Vector3(0.0, 5.0, 10.0),
                rotation = Quaternion.FromEulerAngles(-20.0, 180.0, 0.0),
                components = {
                    {
                        type = "CameraComponent",
                        fov = 45.0,
                        nearPlane = 0.1,
                        farPlane = 1000.0,
                        projection = "Perspective",
                        isMain = true
                    },
                    {
                        type = "FreeLookCamera",
                        moveSpeed = 10.0,
                        lookSpeed = 0.1
                    }
                }
            },
            -- Directional Light (Sun)
            {
                name = "Sun",
                rotation = Quaternion.FromEulerAngles(50.0, -30.0, 0.0),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        color = Vector3(1.0, 0.95, 0.9), -- Sunlight
                        intensity = 1.2,
                        castShadows = true,
                        shadowResolution = 2048,
                        shadowBias = 0.005
                    }
                }
            },
            -- Point Light (Red glow)
            {
                name = "RedLight",
                position = Vector3(-3.0, 2.0, 0.0),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Point",
                        color = Vector3(1.0, 0.2, 0.2),
                        intensity = 3.0,
                        range = 10.0
                    }
                }
            },
            -- Floor
            {
                name = "Floor",
                position = Vector3(0.0, -0.5, 0.0),
                scale = Vector3(10.0, 1.0, 10.0),
                components = {
                    {
                        type = "MeshRenderer",
                        mesh = "Default/Models/plane.obj",
                        shader = "basic"
                    }
                }
            },
            -- Cube
            {
                name = "Cube",
                position = Vector3(0.0, 1.0, 0.0),
                rotation = Quaternion.FromEulerAngles(0.0, 45.0, 0.0),
                components = {
                    {
                        type = "MeshRenderer",
                        mesh = "Default/Models/cube.obj",
                        shader = "basic"
                    }
                }
            }
        }
    }
end
