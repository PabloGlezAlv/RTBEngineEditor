function CreateScene()
    return {
        name = "KayKit Showcase",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "B1000001-0000-4000-8000-000000000001",
                position = Vector3(0.00, 4.00, -14.00),
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
                uuid = "B1000002-0000-4000-8000-000000000002",
                rotation = Quaternion.FromEulerAngles(4.70, -56.04, -49.84),
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
                name = "Dungeon Environment",
                uuid = "B1000020-0000-4000-8000-000000000020",
                children = {
                    {
                        name = "Floor",
                        uuid = "B1000021-0000-4000-8000-000000000021",
                        children = {
                            {
                                name = "floor_0_0",
                                uuid = "D6CAB5D9-521C-4117-AD73-6EDF650133E5",
                                position = Vector3(-12.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_0_1",
                                uuid = "DF9BA2FC-5543-49EB-A50C-926C0E851AA5",
                                position = Vector3(-12.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_0_2",
                                uuid = "3AF6F753-33FF-4736-9DE1-AC0811CF8228",
                                position = Vector3(-12.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_0_3",
                                uuid = "97276964-B209-4003-B831-69992D4CA4F5",
                                position = Vector3(-12.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_0_4",
                                uuid = "9BF72547-E8A9-47AF-A775-9560D601BB6E",
                                position = Vector3(-12.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_0_5",
                                uuid = "6FEB375D-0F18-4CDA-A43D-1596DB1301D8",
                                position = Vector3(-12.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_0_6",
                                uuid = "B6C6D94E-AFBB-4696-9BB1-572FC54064E6",
                                position = Vector3(-12.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_1_0",
                                uuid = "9008EE44-799A-45E3-951B-7AACEE135E57",
                                position = Vector3(-8.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_1_1",
                                uuid = "CA1A85F6-C66C-42D6-B2CE-277DC0F4449F",
                                position = Vector3(-8.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_1_2",
                                uuid = "AE696519-66AB-4CB0-841D-92E1A739311B",
                                position = Vector3(-8.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_1_3",
                                uuid = "41213F23-B9D7-4404-AF54-140D96DEF59F",
                                position = Vector3(-8.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_1_4",
                                uuid = "D42D2D47-AFB2-4418-8DDA-5E8BA0373B92",
                                position = Vector3(-8.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_1_5",
                                uuid = "1EE912B0-CF07-478C-96D7-F80C5132591B",
                                position = Vector3(-8.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_1_6",
                                uuid = "2D3C37F7-8299-4759-87C0-645D84C6712A",
                                position = Vector3(-8.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_2_0",
                                uuid = "4D245D28-FED1-454B-AD24-B4A6C5D802F8",
                                position = Vector3(-4.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_2_1",
                                uuid = "F5F32573-E08B-4934-ABA3-E51AFD85BCA7",
                                position = Vector3(-4.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_2_2",
                                uuid = "ABC171AA-E1E1-4182-8557-C652C137EE6D",
                                position = Vector3(-4.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_2_3",
                                uuid = "B5DDF8E7-0CF4-44B1-92E8-E8DFC3727AB1",
                                position = Vector3(-4.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_2_4",
                                uuid = "3023787C-6DB4-495F-B627-245122A0A914",
                                position = Vector3(-4.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_2_5",
                                uuid = "F3D8254C-9E9B-47BE-A97A-4DAE09D390B7",
                                position = Vector3(-4.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_2_6",
                                uuid = "EACD7AA4-900D-4469-954B-C4289D05E36E",
                                position = Vector3(-4.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_3_0",
                                uuid = "BFA5FB35-2802-40E0-9316-D863918F2454",
                                position = Vector3(0.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_3_1",
                                uuid = "ADD76D24-4527-49A9-8FA0-0F88A51B1763",
                                position = Vector3(0.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_3_2",
                                uuid = "5EC45AE8-3AB2-404B-9B0E-2978422EBE02",
                                position = Vector3(0.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_3_3",
                                uuid = "CF6D686D-BAAC-47B9-A6B6-E3AF0E5873EC",
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_3_4",
                                uuid = "8E109A38-B0FC-401C-92AE-839F564BF9B1",
                                position = Vector3(0.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_3_5",
                                uuid = "4CB87FF2-9E48-42FB-A649-0C52F06C9F2B",
                                position = Vector3(0.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_3_6",
                                uuid = "2651EFC1-B343-4792-8FA2-E17302346508",
                                position = Vector3(0.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_4_0",
                                uuid = "CF5ADC5F-B102-44D3-BF23-0A2A87618143",
                                position = Vector3(4.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_4_1",
                                uuid = "B4689EF1-8EE0-445E-909C-43BEC4D91D27",
                                position = Vector3(4.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_4_2",
                                uuid = "4CDA7E51-7575-422A-A6A1-84C2D075C66B",
                                position = Vector3(4.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_4_3",
                                uuid = "72496574-7673-4B67-ACC2-7F835D8A85CC",
                                position = Vector3(4.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_4_4",
                                uuid = "95B58E08-A6A1-4914-AEDB-12721918D145",
                                position = Vector3(4.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_4_5",
                                uuid = "C9D446B1-1B68-4C9A-A349-94BA037B87A2",
                                position = Vector3(4.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_4_6",
                                uuid = "03226558-5FB7-4278-84E3-9947CB5A43CE",
                                position = Vector3(4.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_5_0",
                                uuid = "6218E69C-0A49-4D5B-8B8D-5CD4C9AEB07F",
                                position = Vector3(8.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_5_1",
                                uuid = "E0AD57BE-98AB-4FB5-8FBE-0F18775722B3",
                                position = Vector3(8.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_5_2",
                                uuid = "7F40CA75-4BF1-4E11-A1EA-9BCF5E1DED52",
                                position = Vector3(8.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_5_3",
                                uuid = "F79523BA-7F20-4EF5-8072-7265B350FC43",
                                position = Vector3(8.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_5_4",
                                uuid = "2C108C41-A87B-4437-A967-A3857D635905",
                                position = Vector3(8.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_5_5",
                                uuid = "37EB7B4F-A606-4F8F-BACF-94E27B999B59",
                                position = Vector3(8.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_5_6",
                                uuid = "BC85CB8D-1D9C-4893-890C-3676EDEE8109",
                                position = Vector3(8.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_6_0",
                                uuid = "5439B7E4-CD86-4661-9CC2-DB0AB616255B",
                                position = Vector3(12.00, 0.00, -12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_6_1",
                                uuid = "ED662410-6DFC-4DAE-ADEF-B97527130D2F",
                                position = Vector3(12.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_6_2",
                                uuid = "B692E98C-89DF-4B8C-8BDB-C565378E85D3",
                                position = Vector3(12.00, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_6_3",
                                uuid = "F3C8481F-4040-4CEE-87A5-F60EAE2DBCDC",
                                position = Vector3(12.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_6_4",
                                uuid = "A4DEA01F-3963-4D3A-A750-2530ABF4D40E",
                                position = Vector3(12.00, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_6_5",
                                uuid = "F00AB30E-5E78-48C7-910D-71371AC1FB98",
                                position = Vector3(12.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                                name = "floor_6_6",
                                uuid = "6FE3F004-6A2E-43F8-BAED-D95E677D4C12",
                                position = Vector3(12.00, 0.00, 12.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 0.15, 4.00),
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
                        }
                    },
                    {
                        name = "Walls",
                        uuid = "B1000022-0000-4000-8000-000000000022",
                        children = {
                            {
                                name = "wall_corner_sw",
                                uuid = "38216572-6871-4AC0-8A47-020B420EB216",
                                position = Vector3(-14.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_corner_se",
                                uuid = "F06AB5EB-04F4-496C-956F-58D76675B193",
                                position = Vector3(14.00, 0.00, -14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_corner_nw",
                                uuid = "B43BFF7C-0523-42C1-BD3A-10CBCBA83540",
                                position = Vector3(-14.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_corner_ne",
                                uuid = "91C6703D-BF74-45E1-B350-80FC7795B32A",
                                position = Vector3(14.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_s_-12",
                                uuid = "82DA6E4B-743D-4D53-8391-FF14CE13A236",
                                position = Vector3(-12.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_s_-8",
                                uuid = "702EDC44-253A-405B-95B5-A70FFADF6B29",
                                position = Vector3(-8.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_s_-4",
                                uuid = "368BA828-B06F-44E6-B684-5C68F64AFC0B",
                                position = Vector3(-4.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_s_0",
                                uuid = "A1807318-F47A-4F36-A7D5-9083719FE0CF",
                                position = Vector3(0.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_s_4",
                                uuid = "90E3611C-F7A5-4E14-8926-3D34B9F196CD",
                                position = Vector3(4.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_s_8",
                                uuid = "616CFE83-F8C9-43E5-B13F-4C8D1380D6BF",
                                position = Vector3(8.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_s_12",
                                uuid = "8E2ACEA4-040D-4FE5-BD7E-E161CF48442F",
                                position = Vector3(12.00, 0.00, -14.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_n_-12",
                                uuid = "4292830E-9A6A-47C0-B107-7330CD1E1709",
                                position = Vector3(-12.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_n_-8",
                                uuid = "6796F649-3C46-4BDC-B44E-1D6957A5A27D",
                                position = Vector3(-8.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_n_-4",
                                uuid = "57E78EB3-B020-439E-843C-1C294DA7E197",
                                position = Vector3(-4.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_n_0",
                                uuid = "4798839B-2BAF-4F47-B1F3-F02331D956F4",
                                position = Vector3(0.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_n_4",
                                uuid = "92AEB059-7D88-4720-BE44-73032DB4AB16",
                                position = Vector3(4.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_n_8",
                                uuid = "48ABB9EB-6E18-49F6-B8C0-C777C1577D6F",
                                position = Vector3(8.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_n_12",
                                uuid = "2BE90B92-2CF2-4EDD-AF65-05975B2D4D15",
                                position = Vector3(12.00, 0.00, 14.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_w_-12",
                                uuid = "3CC8563E-0230-49F8-8708-CC0211063B80",
                                position = Vector3(-14.00, 0.00, -12.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_w_-8",
                                uuid = "882A1FAA-F328-4D8A-968E-8537C680FCF5",
                                position = Vector3(-14.00, 0.00, -8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_w_-4",
                                uuid = "83F824BB-4671-45FB-B132-6B5FC32FE4ED",
                                position = Vector3(-14.00, 0.00, -4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_w_0",
                                uuid = "7E8BEB04-EE27-4A2D-A66E-D4AF82B2F957",
                                position = Vector3(-14.00, 0.00, 0.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_w_4",
                                uuid = "5510113E-4C31-48C6-BCA7-51E9C02FC846",
                                position = Vector3(-14.00, 0.00, 4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_w_8",
                                uuid = "07976824-E099-4746-8681-9FD8D25FA7C4",
                                position = Vector3(-14.00, 0.00, 8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_w_12",
                                uuid = "16803484-1D94-4978-A729-A56B14EBB37F",
                                position = Vector3(-14.00, 0.00, 12.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_e_-12",
                                uuid = "3E8D2CE0-FE9F-4C44-B937-9DB742F81558",
                                position = Vector3(14.00, 0.00, -12.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_e_-8",
                                uuid = "A0482A13-A088-4277-86C0-7C40BE32ECB4",
                                position = Vector3(14.00, 0.00, -8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_e_-4",
                                uuid = "22774CEB-4983-4524-AF62-4CB422E10D01",
                                position = Vector3(14.00, 0.00, -4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_e_0",
                                uuid = "FE376A38-ECB6-4E36-9E55-70A8E92C7854",
                                position = Vector3(14.00, 0.00, 0.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_e_4",
                                uuid = "1CE3E399-52AE-4F24-8957-D45D7DA4FE45",
                                position = Vector3(14.00, 0.00, 4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_e_8",
                                uuid = "6DD07016-7921-4B80-AF64-621B88D35FD4",
                                position = Vector3(14.00, 0.00, 8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_e_12",
                                uuid = "3F1FF51B-7057-4C6C-800D-C9516758B3B7",
                                position = Vector3(14.00, 0.00, 12.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_cross_x_-8",
                                uuid = "200689BB-5A08-4E30-86D7-D1D60D539BE3",
                                position = Vector3(-8.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_cross_x_-4",
                                uuid = "77A1F584-6933-48D4-9D89-3EBC56797183",
                                position = Vector3(-4.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_cross_x_4",
                                uuid = "2EEF0D11-8F87-49F9-9C92-52DE26FE92A3",
                                position = Vector3(4.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_cross_x_8",
                                uuid = "967B6BE5-EF78-4140-89E5-39E4D4C00A88",
                                position = Vector3(8.00, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_cross_z_-8",
                                uuid = "22E1DD0D-84F9-4B91-87FC-F6CE179F3833",
                                position = Vector3(0.00, 0.00, -8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_cross_z_-4",
                                uuid = "12D6DF2C-4188-4B07-A328-C504FAA795EA",
                                position = Vector3(0.00, 0.00, -4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_cross_z_4",
                                uuid = "1944C36B-2C00-48CF-A327-955A5CD290C9",
                                position = Vector3(0.00, 0.00, 4.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                                name = "wall_cross_z_8",
                                uuid = "E46A1B8C-6B51-4EF5-AAC0-C0451E5BE3C6",
                                position = Vector3(0.00, 0.00, 8.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(4.00, 4.00, 1.00),
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
                        }
                    },
                    {
                        name = "Props",
                        uuid = "B1000023-0000-4000-8000-000000000023",
                        children = {
                            {
                                name = "pillar_ne",
                                uuid = "263D13D2-EB33-4DE4-AED1-D977D8E5121A",
                                position = Vector3(8.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/pillar_decorated.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(1.70, 4.00, 1.70),
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
                                name = "pillar_nw",
                                uuid = "20B57E8C-7C0B-4F18-845C-93F3C1F52473",
                                position = Vector3(-8.00, 0.00, 8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/pillar_decorated.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(1.70, 4.00, 1.70),
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
                                name = "pillar_se",
                                uuid = "78080C3E-68FD-4788-AADB-752C8CB6B908",
                                position = Vector3(8.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/pillar_decorated.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(1.70, 4.00, 1.70),
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
                                name = "pillar_sw",
                                uuid = "019D9191-4831-4751-AEE0-B72E4C213361",
                                position = Vector3(-8.00, 0.00, -8.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/pillar_decorated.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                    {
                                        type = "BoxColliderComponent",
                                        size = Vector3(1.70, 4.00, 1.70),
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
                                name = "torch_s_1",
                                uuid = "77C4F17D-D8C8-469A-8CD7-082F13D15B5A",
                                position = Vector3(-6.00, 0.00, -13.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/torch_lit.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "torch_s_2",
                                uuid = "82C4A073-30E3-4AE8-BB22-14574DF8218D",
                                position = Vector3(6.00, 0.00, -13.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/torch_lit.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "torch_n",
                                uuid = "84E40076-04D5-4F62-BAA8-4004321A923F",
                                position = Vector3(0.00, 0.00, 13.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/torch_lit.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "barrel_1",
                                uuid = "DDB8E53E-38AE-48A3-BF1B-616D91646CC0",
                                position = Vector3(-10.00, 0.00, -10.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/barrel_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "barrel_2",
                                uuid = "81C4CBF1-6E9B-453E-B4F1-401079FAC925",
                                position = Vector3(10.00, 0.00, -10.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/barrel_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "chest_1",
                                uuid = "21E33F15-82CC-4984-B465-02E5FB0B7027",
                                position = Vector3(-10.00, 0.00, 10.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/chest.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "banner_1",
                                uuid = "7055605E-FFCD-43EC-9613-B376D18DCDDA",
                                position = Vector3(0.00, 0.00, 13.50),
                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/banner_red.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                        }
                    },
                }
            },
            {
                name = "Knight",
                uuid = "B1000010-0000-4000-8000-000000000010",
                position = Vector3(-8.00, 0.00, 0.00),
                components = {
                    {
                        type = "Animator",
                        modelRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                        currentClipName = "Idle_A",
                        defaultClip = "Idle_A",
                        speed = 1.00,
                        playing = false,
                        looping = true,
                        additionalModels = {
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
                        }
                    },
                },
                children = {
                    {
                        name = "Knight_Head",
                        uuid = "BA7146DB-9009-4AE9-BD6E-BAFB3A898353",
                        position = Vector3(0.00, 1.22, -0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/knight_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 0,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Knight_Helmet",
                        uuid = "ADDBF3A7-EC08-44EF-8503-97F7DE340FDA",
                        position = Vector3(0.00, 1.22, -0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/knight_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 1,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Knight_HelmetVisor",
                        uuid = "60070E92-5190-4099-8EC6-B7305AF7811B",
                        position = Vector3(0.00, 1.75, 0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/knight_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 2,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Knight_Body",
                        uuid = "3DB20872-F5B2-4D24-A264-02401C15687C",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/knight_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 3,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Knight_ArmLeft",
                        uuid = "B33316E6-284A-4DB9-9FA6-A8937BBAB4EC",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/knight_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 4,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Knight_LegLeft",
                        uuid = "52A87923-66BF-44EB-9D52-B1A1C2E932C9",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/knight_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 5,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Knight_LegRight",
                        uuid = "9CE39988-2059-4595-AE7A-E12584C41489",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/knight_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 6,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Knight_ArmRight",
                        uuid = "F0E281D0-E8A1-4678-83C3-295741387FA6",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/knight_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 7,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Knight_Cape",
                        uuid = "15F6AD15-615F-403F-A881-65AC8BB3708E",
                        position = Vector3(0.00, 1.22, -0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/knight_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 8,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Rig_Medium",
                        uuid = "C655A4AF-C715-4137-90FB-E73FDB3170A3",
                    },
                    {
                        name = "root",
                        uuid = "2444A1A0-040B-4D06-A8C5-558DE7518C06",
                        children = {
                            {
                                name = "hips",
                                uuid = "D98ED918-9305-4374-9B20-F915EFA18530",
                                children = {
                                    {
                                        name = "spine",
                                        uuid = "57C88A11-D7A8-4AFE-956A-A736F81CF9AD",
                                        children = {
                                            {
                                                name = "chest",
                                                uuid = "AA2E9030-2046-4992-B5E7-B252207E1074",
                                                children = {
                                                    {
                                                        name = "upperarm.l",
                                                        uuid = "2EAD8102-8D21-4FE0-AFA8-68B58D2C3E9E",
                                                        children = {
                                                            {
                                                                name = "lowerarm.l",
                                                                uuid = "A89B67CD-BD7C-457A-A4A0-B7CA5DB110A8",
                                                                children = {
                                                                    {
                                                                        name = "wrist.l",
                                                                        uuid = "C7CA8759-3493-482B-83A8-9408DD048465",
                                                                        children = {
                                                                            {
                                                                                name = "hand.l",
                                                                                uuid = "F1090621-C795-4B86-8BBF-223CA1314379",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.l",
                                                                                        uuid = "DF555AFD-00AF-4D03-B8A5-0C4332B5B3F7",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Knight_Shield",
                                                                                                uuid = "DE0E24BE-F412-4DF1-AF63-035427C06A6F",
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/shield_square.fbx",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/knight_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "upperarm.r",
                                                        uuid = "DC55FD24-DDAE-4CAB-A39E-ADC265C7A885",
                                                        children = {
                                                            {
                                                                name = "lowerarm.r",
                                                                uuid = "ECAB2189-412B-41FB-85E2-CAAD265E01B0",
                                                                children = {
                                                                    {
                                                                        name = "wrist.r",
                                                                        uuid = "EA3EFC06-9B7B-407A-A898-5AAD59D7B1D1",
                                                                        children = {
                                                                            {
                                                                                name = "hand.r",
                                                                                uuid = "DFA07C87-73B0-4107-828F-F60485955882",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.r",
                                                                                        uuid = "248FFEC0-15B2-4A1F-BE8A-BA66B738A18A",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Knight_Sword",
                                                                                                uuid = "B7DB9463-CD71-4C6A-87B2-D9299F7FEA1A",
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/sword_1handed.fbx",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/knight_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "head",
                                                        uuid = "5663156A-E15C-4749-BB3B-352490821851",
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.l",
                                        uuid = "BD5BB1C6-ADE9-4DFE-8BA2-7C062EFD6376",
                                        children = {
                                            {
                                                name = "lowerleg.l",
                                                uuid = "DDEFC779-C160-4296-9FEF-05A95C232F94",
                                                children = {
                                                    {
                                                        name = "foot.l",
                                                        uuid = "96F6D0F4-713A-4317-98D2-724D715AD9D5",
                                                        children = {
                                                            {
                                                                name = "toes.l",
                                                                uuid = "7BEA2AB2-9B95-470D-96EE-6C14C9FCC7F4",
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.r",
                                        uuid = "229AA610-4338-4E48-B140-00223E9C5F25",
                                        children = {
                                            {
                                                name = "lowerleg.r",
                                                uuid = "2EACD0E9-5809-4008-98D7-CABAA209302C",
                                                children = {
                                                    {
                                                        name = "foot.r",
                                                        uuid = "03B1A0C5-E9E9-4604-98DF-9778EA07B3D6",
                                                        children = {
                                                            {
                                                                name = "toes.r",
                                                                uuid = "C325E49B-855C-4F17-97FE-E299CF4ED607",
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
                name = "Barbarian",
                uuid = "B1000011-0000-4000-8000-000000000011",
                position = Vector3(-4.00, 0.00, 0.00),
                components = {
                    {
                        type = "Animator",
                        modelRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
                        currentClipName = "Idle_A",
                        defaultClip = "Idle_A",
                        speed = 1.00,
                        playing = false,
                        looping = true,
                        additionalModels = {
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
                        }
                    },
                },
                children = {
                    {
                        name = "Barbarian_Body",
                        uuid = "09C214B9-54C6-43CD-A0B8-BA0F23203214",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/barbarian_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 0,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Barbarian_Head",
                        uuid = "9FB9D89D-D1AD-4ECF-A2D2-9A15CA34D2FB",
                        position = Vector3(0.00, 1.22, -0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/barbarian_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 1,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Barbarian_BearHat",
                        uuid = "B26CEFB2-BF27-4D5E-8BAD-B9FF266BD07D",
                        position = Vector3(0.00, 1.22, -0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/barbarian_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 2,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Barbarian_LegLeft",
                        uuid = "3FDACFBF-B334-46FE-BB7D-F1891B2C91BD",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/barbarian_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 3,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Barbarian_LegRight",
                        uuid = "8026F316-011F-4B0B-A648-B28189E07491",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/barbarian_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 4,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Barbarian_ArmRight",
                        uuid = "77B9A8B7-A19D-4A96-90E0-772EC7D2D987",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/barbarian_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 5,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Barbarian_ArmLeft",
                        uuid = "7025B27E-4062-43F6-BC7F-390E7ED414B7",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/barbarian_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 6,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "root",
                        uuid = "C3AE797E-31D9-4E5E-872A-8115332BDCA5",
                        children = {
                            {
                                name = "hips",
                                uuid = "234F1BA2-B83E-450F-B2F4-64FEF78C55DE",
                                children = {
                                    {
                                        name = "spine",
                                        uuid = "AFA0FBF6-4EC0-4C7B-AE4B-0D197E11B185",
                                        children = {
                                            {
                                                name = "chest",
                                                uuid = "8C6C13F3-F6D2-4F5E-AF04-3FFF402C8749",
                                                children = {
                                                    {
                                                        name = "upperarm.l",
                                                        uuid = "2877A3A2-5FCE-4889-A912-B9CD82FBFDD3",
                                                        children = {
                                                            {
                                                                name = "lowerarm.l",
                                                                uuid = "2F730E93-D480-4D93-986E-E67D3FF4F7DD",
                                                                children = {
                                                                    {
                                                                        name = "wrist.l",
                                                                        uuid = "A5B14C9D-EAC4-4EEE-884E-675CF4F02299",
                                                                        children = {
                                                                            {
                                                                                name = "hand.l",
                                                                                uuid = "6BF1A544-4789-41A3-A216-EB247E3092A5",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.l",
                                                                                        uuid = "1FA8CAD6-E386-430B-AE03-F2472D2794D2",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Barbarian_Shield",
                                                                                                uuid = "34EA3233-292F-4908-878D-FC337D613893",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, -90.00),
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/shield_round_barbarian.obj",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/barbarian_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "upperarm.r",
                                                        uuid = "51E2F679-69E4-4C02-94D0-A7E3932FEA77",
                                                        children = {
                                                            {
                                                                name = "lowerarm.r",
                                                                uuid = "C9482B80-CC9F-4A9A-8494-6835E5E825E4",
                                                                children = {
                                                                    {
                                                                        name = "wrist.r",
                                                                        uuid = "A1481B2C-5C31-46BF-B66A-9CD5C9993FD1",
                                                                        children = {
                                                                            {
                                                                                name = "hand.r",
                                                                                uuid = "8206730C-43B3-4083-AFC0-2C0CAD376094",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.r",
                                                                                        uuid = "C11162D2-9AA7-4574-91AC-E3D62E0CF087",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Barbarian_Axe",
                                                                                                uuid = "2376553D-1F6A-4EBD-8BFA-CA3E960C10C9",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 90.00),
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/axe_2handed.obj",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/barbarian_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "head",
                                                        uuid = "B2A2556D-7F04-48A6-ADE6-15BD46947DC9",
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.l",
                                        uuid = "249805AA-9D55-4384-8E1A-326EF83CE5AE",
                                        children = {
                                            {
                                                name = "lowerleg.l",
                                                uuid = "930ED193-0673-49A1-B0C9-002416C36A92",
                                                children = {
                                                    {
                                                        name = "foot.l",
                                                        uuid = "09979B93-E9A8-46D6-BA80-A49F1A1782EC",
                                                        children = {
                                                            {
                                                                name = "toes.l",
                                                                uuid = "0F2F87D4-523F-4B0A-AC02-E389F1719154",
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.r",
                                        uuid = "9C52C198-6C83-4297-8A13-09B57A01879B",
                                        children = {
                                            {
                                                name = "lowerleg.r",
                                                uuid = "574341CA-EC23-4882-B44E-AA6F846E9C52",
                                                children = {
                                                    {
                                                        name = "foot.r",
                                                        uuid = "48618DCB-4F09-4730-A5DC-E313CD7A3E5C",
                                                        children = {
                                                            {
                                                                name = "toes.r",
                                                                uuid = "5DCB0FC3-5709-4D8E-A2A6-C6E2CD2DCED3",
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
                name = "Mage",
                uuid = "B1000012-0000-4000-8000-000000000012",
                components = {
                    {
                        type = "Animator",
                        modelRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
                        currentClipName = "Idle_A",
                        defaultClip = "Spawn_Ground",
                        speed = 1.00,
                        playing = false,
                        looping = true,
                        additionalModels = {
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
                        }
                    },
                },
                children = {
                    {
                        name = "Mage_LegLeft",
                        uuid = "10CC6DC7-AA61-4877-AF3B-4171FEB7C0D9",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/mage_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 0,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Mage_ArmLeft",
                        uuid = "94F74518-2C0E-41C9-98AC-71F2A1BA8051",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/mage_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 1,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Mage_ArmRight",
                        uuid = "FBAECBC5-2BDF-4C13-A570-DA2F365B2427",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/mage_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 2,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Mage_Cape",
                        uuid = "BABD5E6C-D928-4053-9F06-826312E7DB9F",
                        position = Vector3(0.00, 1.22, -0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/mage_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 3,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Mage_Head",
                        uuid = "C7CEF0D9-2C34-4425-9926-E875E0AEA46B",
                        position = Vector3(0.00, 1.22, -0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/mage_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 4,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Mage_LegRight",
                        uuid = "50DF6EE9-F859-43CB-9920-1038C12DAF11",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/mage_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 5,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Mage_Hat",
                        uuid = "4905FF7E-8E68-4379-B589-EFE06C2E183D",
                        position = Vector3(0.00, 1.22, -0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/mage_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 6,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Mage_Body",
                        uuid = "A6EB2B03-3822-4DDF-AAE4-E91E0433DB39",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/mage_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 7,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Rig_Medium",
                        uuid = "88F6CF7D-3A48-4518-BBE8-6BF5EEE6A39F",
                    },
                    {
                        name = "root",
                        uuid = "7BD6D9AC-6D1E-4BB4-BDA6-61432173859F",
                        children = {
                            {
                                name = "hips",
                                uuid = "D29D3385-D09C-470A-ACDD-3F9CBA5FEC85",
                                children = {
                                    {
                                        name = "spine",
                                        uuid = "FBBB3E8C-D912-4E88-946A-98278935A008",
                                        children = {
                                            {
                                                name = "chest",
                                                uuid = "95468F1C-74E7-4A49-94F8-2858DF8CD276",
                                                children = {
                                                    {
                                                        name = "upperarm.l",
                                                        uuid = "59347BA7-70FA-461E-A112-7C699DF997EE",
                                                        children = {
                                                            {
                                                                name = "lowerarm.l",
                                                                uuid = "C01D3F70-5EDF-4A2A-A5CA-00913F391453",
                                                                children = {
                                                                    {
                                                                        name = "wrist.l",
                                                                        uuid = "743E122E-F991-4DD3-9C56-74CF4B5D03BA",
                                                                        children = {
                                                                            {
                                                                                name = "hand.l",
                                                                                uuid = "97B3A052-78F7-467A-B02E-FD279F851529",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.l",
                                                                                        uuid = "D4D03F2F-C0E5-4A73-BA78-7F2EFBDD295D",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Mage_Spellbook",
                                                                                                uuid = "ABB05A37-CEF6-4F87-AF3A-F6825A717BD2",
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/spellbook_open.fbx",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/mage_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "upperarm.r",
                                                        uuid = "38CEE13F-6A82-4C5B-B981-2F084B9C172A",
                                                        children = {
                                                            {
                                                                name = "lowerarm.r",
                                                                uuid = "2FED6737-76D4-4359-909C-EA0705EFDEDC",
                                                                children = {
                                                                    {
                                                                        name = "wrist.r",
                                                                        uuid = "1665C12A-065A-49A0-8B86-A0419E8C2BB2",
                                                                        children = {
                                                                            {
                                                                                name = "hand.r",
                                                                                uuid = "31CFFDAC-16DE-4EA1-8087-A4471B20A856",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.r",
                                                                                        uuid = "916A9099-94D8-45A2-BA15-A2D292CDB206",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Mage_Staff",
                                                                                                uuid = "0D2A76CF-0673-44F5-8EC7-A8E35C4403C8",
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/staff.fbx",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/mage_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "head",
                                                        uuid = "6840B17C-E20A-4069-AADB-7E46D74149BA",
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.l",
                                        uuid = "BA9B352E-7393-464A-8BBF-8F6D9E23470E",
                                        children = {
                                            {
                                                name = "lowerleg.l",
                                                uuid = "712C49FC-FF36-434A-AF73-342329B105F8",
                                                children = {
                                                    {
                                                        name = "foot.l",
                                                        uuid = "3E6EF9C4-DEAA-4317-BBE7-7F21D0F7EC3C",
                                                        children = {
                                                            {
                                                                name = "toes.l",
                                                                uuid = "D080703E-5EF1-4EA8-B06F-A319ADFD9120",
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.r",
                                        uuid = "F40C664E-D342-4346-9D7B-089C70857AA9",
                                        children = {
                                            {
                                                name = "lowerleg.r",
                                                uuid = "4A23FE6B-8312-4EAB-B8C4-C0D1788126F4",
                                                children = {
                                                    {
                                                        name = "foot.r",
                                                        uuid = "8F894E65-99F9-4E67-A7CB-A9F6D8441353",
                                                        children = {
                                                            {
                                                                name = "toes.r",
                                                                uuid = "D5BA142F-1A06-46C4-AB9D-5B5A3B1E7C98",
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
                name = "Ranger",
                uuid = "B1000013-0000-4000-8000-000000000013",
                position = Vector3(4.00, 0.00, 0.00),
                components = {
                    {
                        type = "Animator",
                        modelRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                        currentClipName = "Idle_A",
                        defaultClip = "Jump_Idle",
                        speed = 1.00,
                        playing = false,
                        looping = true,
                        additionalModels = {
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
                        }
                    },
                },
                children = {
                    {
                        name = "Ranger_Body",
                        uuid = "0D835642-D7F5-41A3-A69B-F23AE8211540",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 0,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Ranger_ArmLeft",
                        uuid = "17FEBFC3-52D5-4ECC-95DF-ABD7E2BA7F68",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 1,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Ranger_LegLeft",
                        uuid = "3EEB246C-6BD0-4D5F-98C3-7EE37D725250",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 2,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Ranger_LegRight",
                        uuid = "3474CED3-60C8-413C-84DF-8777B8C58799",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 3,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Ranger_ArmRight",
                        uuid = "6502FC58-8D4A-4D57-B59A-1A6B27091BE9",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 4,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Ranger_Head",
                        uuid = "A61179D4-F259-486A-BA20-EA6EB0BC5372",
                        position = Vector3(0.00, 1.24, 0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 5,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Ranger_Quiver",
                        uuid = "6BB456D3-FF3B-4B32-AF3D-AC9A89F8ED09",
                        position = Vector3(0.01, 0.91, -0.39),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 6,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Ranger_Cape",
                        uuid = "DD83E765-244F-49AB-ABE4-ACB6490059D1",
                        position = Vector3(0.00, 1.24, 0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/ranger_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 7,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "root",
                        uuid = "74473306-7E54-4139-BACB-4EA670428059",
                        children = {
                            {
                                name = "hips",
                                uuid = "9422806E-9296-4B96-89E4-9C61E4B3E8BB",
                                children = {
                                    {
                                        name = "spine",
                                        uuid = "1D432738-B884-42D0-823B-F2FA57028AE7",
                                        children = {
                                            {
                                                name = "chest",
                                                uuid = "10DDD504-6BE0-4211-8DF3-3BBF6137F221",
                                                children = {
                                                    {
                                                        name = "upperarm.l",
                                                        uuid = "3C3D5D32-068A-4FE1-9580-1E60C23283D8",
                                                        children = {
                                                            {
                                                                name = "lowerarm.l",
                                                                uuid = "6409599E-EA1C-4FEF-9CC0-9151F842F8F4",
                                                                children = {
                                                                    {
                                                                        name = "wrist.l",
                                                                        uuid = "2B839ECC-169D-424F-A5D0-273DD8AA5AF6",
                                                                        children = {
                                                                            {
                                                                                name = "hand.l",
                                                                                uuid = "B8DF8B5C-AE86-44C1-A286-0ACF2B71BE76",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.l",
                                                                                        uuid = "83035D16-4B05-4119-A7C5-777BF903DDCF",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Ranger_Bow",
                                                                                                uuid = "8C4F770B-5726-449C-A9B9-9FD2556806DF",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -180.00, 90.00),
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/bow_withString.obj",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/ranger_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "upperarm.r",
                                                        uuid = "2DE3F9A1-8D94-45FC-93CF-2528294DD30F",
                                                        children = {
                                                            {
                                                                name = "lowerarm.r",
                                                                uuid = "397A7733-C0B2-450A-AB58-99629B11E0A2",
                                                                children = {
                                                                    {
                                                                        name = "wrist.r",
                                                                        uuid = "FE2A4AE4-C606-49E2-A3FA-21431E14DDAD",
                                                                        children = {
                                                                            {
                                                                                name = "hand.r",
                                                                                uuid = "6CA6ED4B-0C0E-463A-83B0-7FDE77408851",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.r",
                                                                                        uuid = "1F20C92A-38B7-4B91-BBC3-34ADF19E2AE2",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Ranger_Arrow",
                                                                                                uuid = "B29D95CE-851B-4793-9B33-648E5B75F0C0",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 90.00),
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/arrow_bow.obj",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/ranger_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "head",
                                                        uuid = "4846DAF7-E8FD-488E-87F4-51A6635CE55D",
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.l",
                                        uuid = "1D4C4AA8-4D91-4A17-B273-531053B99FAE",
                                        children = {
                                            {
                                                name = "lowerleg.l",
                                                uuid = "8F4E1F65-B82E-4B25-8824-76AA973F61A6",
                                                children = {
                                                    {
                                                        name = "foot.l",
                                                        uuid = "6186882C-BDEC-493A-B00E-63FB796B1DD7",
                                                        children = {
                                                            {
                                                                name = "toes.l",
                                                                uuid = "ECFFF998-217E-44A7-BCAF-9AD8556B45FC",
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.r",
                                        uuid = "7F2EE9AA-2573-4BDA-AC5B-0214F4B98D62",
                                        children = {
                                            {
                                                name = "lowerleg.r",
                                                uuid = "3404A8B9-6C58-48B3-BE73-2055B81AFD07",
                                                children = {
                                                    {
                                                        name = "foot.r",
                                                        uuid = "304DD70C-EF73-4798-B52D-DF6B5D8026D3",
                                                        children = {
                                                            {
                                                                name = "toes.r",
                                                                uuid = "E222D931-56F7-4086-BA57-04A95AD17305",
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
                name = "Rogue",
                uuid = "B1000014-0000-4000-8000-000000000014",
                position = Vector3(8.00, 0.00, 0.00),
                components = {
                    {
                        type = "Animator",
                        modelRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Rogue.fbx",
                        currentClipName = "Idle_A",
                        defaultClip = "Walking_C",
                        speed = 1.00,
                        playing = false,
                        looping = true,
                        additionalModels = {
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
                            "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
                        }
                    },
                },
                children = {
                    {
                        name = "Rogue_Head",
                        uuid = "7DE8CEFE-5231-4FEE-9182-D0C465A34C7D",
                        position = Vector3(-0.00, 1.22, 0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Rogue.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/rogue_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 0,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Rig_Medium",
                        uuid = "D9C43027-8CD2-4FE6-BB4A-1211F55C1F01",
                    },
                    {
                        name = "Rogue_LegLeft",
                        uuid = "604040FA-B522-4F93-938E-CBD253281732",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Rogue.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/rogue_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 1,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Rogue_LegRight",
                        uuid = "25C431BD-AB47-4DE3-942B-B70CC05C5D0E",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Rogue.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/rogue_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 2,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Rogue_ArmLeft",
                        uuid = "DFBE986B-2CBA-4EA1-809B-324ABA2E527A",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Rogue.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/rogue_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 3,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Rogue_ArmRight",
                        uuid = "71DB810B-F69E-469B-86A0-A1EE4C587075",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Rogue.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/rogue_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 4,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Rogue_Body",
                        uuid = "EA213FC0-4ADF-48DB-A79B-0CCDD73BD85C",
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Rogue.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/rogue_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 5,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Rogue_Cape",
                        uuid = "9D229296-E501-4F0C-B370-7B4461AAD599",
                        position = Vector3(-0.00, 1.22, 0.00),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Rogue.fbx",
                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/rogue_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 6,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "root",
                        uuid = "AED578F8-4615-4B55-8071-89AD8E9B140F",
                        children = {
                            {
                                name = "hips",
                                uuid = "7E04A8AB-CE92-4C23-8998-54FC7ADDEA79",
                                children = {
                                    {
                                        name = "spine",
                                        uuid = "1AD128FF-9250-47E5-80FA-70B763862261",
                                        children = {
                                            {
                                                name = "chest",
                                                uuid = "1AEE9925-0F5D-434D-A6B3-C6152905EF6D",
                                                children = {
                                                    {
                                                        name = "Rogue_Crossbow",
                                                        uuid = "A9F5BD98-E35E-4002-A0DD-AF5483E14A08",
                                                        position = Vector3(0.00, 0.10, -0.18),
                                                        rotation = Quaternion.FromEulerAngles(-70.00, 0.00, 0.00),
                                                        components = {
                                                            {
                                                                type = "MeshRenderer",
                                                                meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/crossbow_2handed.fbx",
                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/rogue_texture.png",
                                                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                meshIndex = 0,
                                                                multiMesh = false
                                                            },
                                                        }
                                                    },
                                                    {
                                                        name = "upperarm.l",
                                                        uuid = "DCB7813D-CC4D-452F-B095-5C21FE0D6220",
                                                        children = {
                                                            {
                                                                name = "lowerarm.l",
                                                                uuid = "4AD7FC37-BDA7-4DC0-B330-2B92317BFEA8",
                                                                children = {
                                                                    {
                                                                        name = "wrist.l",
                                                                        uuid = "51C6C0E1-D5FC-4D8C-98A3-F5E0472B8A56",
                                                                        children = {
                                                                            {
                                                                                name = "hand.l",
                                                                                uuid = "E0A76D6E-1003-4219-BAC5-64516FBC05D7",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.l",
                                                                                        uuid = "E0C3877C-7262-4D38-A107-5A385EBAB621",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Rogue_Smokebomb",
                                                                                                uuid = "B0EF893A-9220-42E4-8177-699F9583B6B1",
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/smokebomb.fbx",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/rogue_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "upperarm.r",
                                                        uuid = "9F84E3BA-7896-4C15-A1C8-EB966898BE3F",
                                                        children = {
                                                            {
                                                                name = "lowerarm.r",
                                                                uuid = "F9870941-FE30-4E1C-9DAA-6FDCCED79596",
                                                                children = {
                                                                    {
                                                                        name = "wrist.r",
                                                                        uuid = "CEA2CB5C-301A-4CF2-B982-641E044D8038",
                                                                        children = {
                                                                            {
                                                                                name = "hand.r",
                                                                                uuid = "ECFCE6D5-E6FE-4D79-AFF3-91D7C9954F8C",
                                                                                children = {
                                                                                    {
                                                                                        name = "handslot.r",
                                                                                        uuid = "032CE981-3E1B-4825-8D05-E06FFC35A4FB",
                                                                                        children = {
                                                                                            {
                                                                                                name = "Rogue_Dagger",
                                                                                                uuid = "3169CDB4-A81F-404E-B449-5E86B8F4FF45",
                                                                                                components = {
                                                                                                    {
                                                                                                        type = "MeshRenderer",
                                                                                                        meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/dagger.fbx",
                                                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/rogue_texture.png",
                                                                                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                                                                                        meshIndex = 0,
                                                                                                        multiMesh = false
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
                                                        name = "head",
                                                        uuid = "FA20EDB1-D6EE-44B5-93E7-EA1B3F887C1E",
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.l",
                                        uuid = "9C6B8CB2-088A-4C3C-932E-9B14DB4DA1F0",
                                        children = {
                                            {
                                                name = "lowerleg.l",
                                                uuid = "E1C0A255-E13C-4565-9D92-31A721D77A99",
                                                children = {
                                                    {
                                                        name = "foot.l",
                                                        uuid = "09C1EE86-304A-4A36-A576-951601130EEF",
                                                        children = {
                                                            {
                                                                name = "toes.l",
                                                                uuid = "82358F2E-9B43-444E-9543-0ED67AEF47A9",
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "upperleg.r",
                                        uuid = "FAB34B04-2584-475C-A35B-0D0001ECD776",
                                        children = {
                                            {
                                                name = "lowerleg.r",
                                                uuid = "27938078-D909-43F1-B564-C6B3962E2430",
                                                children = {
                                                    {
                                                        name = "foot.r",
                                                        uuid = "D8EEC920-7883-425B-8857-CF895F82D9F1",
                                                        children = {
                                                            {
                                                                name = "toes.r",
                                                                uuid = "FEAA3E1C-0151-41BC-9D65-5C1D40D397BA",
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
    }
end
