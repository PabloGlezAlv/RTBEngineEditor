function CreateScene()
    return {
        name = "Test Scene",
        gameObjects = {
            -- Main Camera
            {
                name = "MainCamera",
                position = Vector3(0.0, 2.0, 5.0),
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
                        lookSpeed = 0.2
                    }
                }
            },
            -- Directional Light
            {
                name = "MainLight",
                rotation = Quaternion.FromEulerAngles(45.0, 45.0, 0.0),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        color = Vector3(1.0, 1.0, 1.0),
                        intensity = 1.0,
                        castShadows = true
                    }
                }
            },
            -- Simple Cube
            {
                name = "Cube",
                position = Vector3(0.0, 0.0, 0.0),
                scale = Vector3(1.0, 1.0, 1.0),
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
