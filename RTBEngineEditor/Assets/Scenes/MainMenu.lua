function CreateScene()
    return {
        name = "MainMenu",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "198966E8-23E3-4EC4-9F05-BEB0810D1A43",
                position = Vector3(-7.19, 1.09, 1.75),
                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                components = {
                    {
                        type = "CameraComponent",
                        fov = 48.00,
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
                name = "CharacterPreviewStage",
                uuid = "CSEL-PRVW-0001-4000-8000-000000000001",
                position = Vector3(-3.80, 0.00, 0.00),
                rotation = Quaternion.FromEulerAngles(0.00, 105.00, 0.00),
                components = {
                    {
                        type = "MainMenuCharacterPreview",
                        previewOffset = Vector3(0.00, 0.00, 0.00),
                        previewYawDegrees = 0.00,
                        previewScale = 0.90,
                        idleAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx|Idle_A"
                    },
                },
                children = {
                    {
                        name = "Character Preview Ranger",
                        uuid = "MM-PRVW-RANGER-4000-8000-000000000001",
                        prefab = "Character Preview Ranger",
                        scale = Vector3(0.90, 0.90, 0.90),
                        overrides = {
                            components = {
                                { type = "Animator",
                                    additionalModels = {
                                    },
                                },
                            },
                        },
                        children = {
                            {
                                name = "root",
                                uuid = "C5AD6486-1C69-44C8-8FA5-1631DCC3890A",
                                children = {
                                    {
                                        name = "hips",
                                        uuid = "69657A7D-CE8E-42BB-AE35-5AC07910643C",
                                        children = {
                                            {
                                                name = "spine",
                                                uuid = "5C4C8BA9-6DC6-4D34-B001-841D4E3DCA8B",
                                                children = {
                                                    {
                                                        name = "chest",
                                                        uuid = "C02222AD-8A4A-44A9-9568-713639DFE8FE",
                                                        children = {
                                                            {
                                                                name = "upperarm.l",
                                                                uuid = "EC6C2857-949D-4DA2-BDF4-ED2450B6C5E5",
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.l",
                                                                        uuid = "4DB13D0A-4290-419D-A32D-54B0C4E831A5",
                                                                        children = {
                                                                            {
                                                                                name = "wrist.l",
                                                                                uuid = "2F5ABDB7-48FE-4ADC-B568-EEC24DC4B0A1",
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.l",
                                                                                        uuid = "B2E987EF-3736-4B47-93F3-D0C2A9364C95",
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.l",
                                                                                                uuid = "08964EA4-D7F7-4A0C-BF39-2A6FE9D938B9",
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Ranger_Bow",
                                                                                                        uuid = "109DAEF5-4736-4BB3-B63F-DD6B5B44855E",
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
                                                                uuid = "0E0C67B4-D021-47E3-8F72-A485BFC87DC6",
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.r",
                                                                        uuid = "E5B6B75F-147D-4477-8084-1D15986F517E",
                                                                        children = {
                                                                            {
                                                                                name = "wrist.r",
                                                                                uuid = "3AC8ADA2-B039-4438-92D1-4FA79F1F2F0F",
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.r",
                                                                                        uuid = "0BFBA09A-04BA-4516-AA49-8F07C2032614",
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.r",
                                                                                                uuid = "5F01E38D-E9AC-46EF-81CB-2F439BD1306F",
                                                                                                children = {
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
                                                                uuid = "C733BE2F-9D91-40B5-B63D-6EF9A02D5257",
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.l",
                                                uuid = "99A671E3-FA32-4222-BEEC-7C9757774FF2",
                                                children = {
                                                    {
                                                        name = "lowerleg.l",
                                                        uuid = "922E0DA8-D636-4982-A610-6C56CC401DB5",
                                                        children = {
                                                            {
                                                                name = "foot.l",
                                                                uuid = "110886C1-A83F-40FE-8B68-FC24F2E8C303",
                                                                children = {
                                                                    {
                                                                        name = "toes.l",
                                                                        uuid = "2DC5DA98-335A-479A-8B30-CBED96801F0A",
                                                                    },
                                                                }
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.r",
                                                uuid = "0E2ECE82-9C85-4ABF-8BB7-B404F5285237",
                                                children = {
                                                    {
                                                        name = "lowerleg.r",
                                                        uuid = "FD5B45FE-6637-4772-8CE9-8B3F54F5EA60",
                                                        children = {
                                                            {
                                                                name = "foot.r",
                                                                uuid = "CF8BCD2B-2AE5-4426-A71C-25377ECCEECC",
                                                                children = {
                                                                    {
                                                                        name = "toes.r",
                                                                        uuid = "6F30C2FE-29EF-469A-B98B-C948563D6B7B",
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
            },
            {
                name = "Directional Light",
                uuid = "48103FD7-E1F1-4AAB-8127-C0C2ED5B4111",
                rotation = Quaternion.FromEulerAngles(-33.04, -43.06, 32.48),
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
                name = "Fill Light",
                uuid = "MM-FILL-0001-4000-8000-000000000001",
                rotation = Quaternion.FromEulerAngles(-6.14, 136.67, 27.37),
                components = {
                    {
                        type = "LightComponent",
                        lightType = "Directional",
                        color = Color(0.78, 0.84, 1.00, 1.00),
                        intensity = 0.40,
                        range = 10.00,
                        spotAngle = 45.00,
                        spotInnerAngle = 30.00,
                        syncPosition = true,
                        syncDirection = true
                    },
                }
            },
            {
                name = "MenuEnvironment",
                uuid = "MM-ENV0-0001-4000-8000-000000000001",
                children = {
                    {
                        name = "Floor",
                        uuid = "MM-ENV-FLR-0001-4000-8000-000000000001",
                        children = {
                            {
                                name = "floor_nw",
                                uuid = "MM-ENV-FLR-0002-4000-8000-000000000001",
                                position = Vector3(-7.80, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "floor_n",
                                uuid = "MM-ENV-FLR-0003-4000-8000-000000000001",
                                position = Vector3(-3.80, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "floor_ne",
                                uuid = "MM-ENV-FLR-0004-4000-8000-000000000001",
                                position = Vector3(0.20, 0.00, -4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "floor_w",
                                uuid = "MM-ENV-FLR-0005-4000-8000-000000000001",
                                position = Vector3(-7.80, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "floor_c",
                                uuid = "MM-ENV-FLR-0006-4000-8000-000000000001",
                                position = Vector3(-3.80, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "floor_e",
                                uuid = "MM-ENV-FLR-0007-4000-8000-000000000001",
                                position = Vector3(0.20, 0.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "floor_sw",
                                uuid = "MM-ENV-FLR-0008-4000-8000-000000000001",
                                position = Vector3(-7.80, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "floor_s",
                                uuid = "MM-ENV-FLR-0009-4000-8000-000000000001",
                                position = Vector3(-3.80, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "floor_se",
                                uuid = "MM-ENV-FLR-0010-4000-8000-000000000001",
                                position = Vector3(0.20, 0.00, 4.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/floor_tile_large.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "Walls",
                        uuid = "MM-ENV-WAL-0001-4000-8000-000000000001",
                        children = {
                            {
                                name = "wall_corner_sw",
                                uuid = "MM-ENV-WAL-0002-4000-8000-000000000001",
                                position = Vector3(-9.80, 0.00, -6.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "wall_corner_se",
                                uuid = "MM-ENV-WAL-0003-4000-8000-000000000001",
                                position = Vector3(2.20, 0.00, -6.00),
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
                                }
                            },
                            {
                                name = "wall_corner_nw",
                                uuid = "MM-ENV-WAL-0004-4000-8000-000000000001",
                                position = Vector3(-9.80, 0.00, 6.00),
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
                                }
                            },
                            {
                                name = "wall_corner_ne",
                                uuid = "MM-ENV-WAL-0005-4000-8000-000000000001",
                                position = Vector3(2.20, 0.00, 6.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_corner.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "wall_s_w",
                                uuid = "MM-ENV-WAL-0006-4000-8000-000000000001",
                                position = Vector3(-7.80, 0.00, -6.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "wall_s_c",
                                uuid = "MM-ENV-WAL-0007-4000-8000-000000000001",
                                position = Vector3(-3.80, 0.00, -6.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "wall_s_e",
                                uuid = "MM-ENV-WAL-0008-4000-8000-000000000001",
                                position = Vector3(0.20, 0.00, -6.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "wall_n_w",
                                uuid = "MM-ENV-WAL-0009-4000-8000-000000000001",
                                position = Vector3(-7.80, 0.00, 6.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "wall_n_c",
                                uuid = "MM-ENV-WAL-0010-4000-8000-000000000001",
                                position = Vector3(-3.80, 0.00, 6.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "wall_n_e",
                                uuid = "MM-ENV-WAL-0011-4000-8000-000000000001",
                                position = Vector3(0.20, 0.00, 6.00),
                                rotation = Quaternion.FromEulerAngles(0.00, 180.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "wall_w_n",
                                uuid = "MM-ENV-WAL-0012-4000-8000-000000000001",
                                position = Vector3(-9.80, 0.00, -4.00),
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
                                }
                            },
                            {
                                name = "wall_w_c",
                                uuid = "MM-ENV-WAL-0013-4000-8000-000000000001",
                                position = Vector3(-9.80, 0.00, 0.00),
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
                                }
                            },
                            {
                                name = "wall_w_s",
                                uuid = "MM-ENV-WAL-0014-4000-8000-000000000001",
                                position = Vector3(-9.80, 0.00, 4.00),
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
                                }
                            },
                            {
                                name = "wall_e_n",
                                uuid = "MM-ENV-WAL-0015-4000-8000-000000000001",
                                position = Vector3(2.20, 0.00, -4.00),
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
                                }
                            },
                            {
                                name = "wall_e_c",
                                uuid = "MM-ENV-WAL-0016-4000-8000-000000000001",
                                position = Vector3(2.20, 0.00, 0.00),
                                rotation = Quaternion.FromEulerAngles(0.00, -90.00, 0.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/wall_archedwindow_open.obj",
                                        textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                        colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                        meshIndex = 0,
                                        multiMesh = false
                                    },
                                }
                            },
                            {
                                name = "wall_e_s",
                                uuid = "MM-ENV-WAL-0017-4000-8000-000000000001",
                                position = Vector3(2.20, 0.00, 4.00),
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
                                }
                            },
                        }
                    },
                    {
                        name = "ColumnLeft",
                        uuid = "MM-ENV-COL-0001-4000-8000-000000000001",
                        position = Vector3(-1.20, 0.00, -2.60),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/column.obj",
                                textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 0,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "ColumnRight",
                        uuid = "MM-ENV-COL-0002-4000-8000-000000000001",
                        position = Vector3(-1.20, 0.00, 2.60),
                        components = {
                            {
                                type = "MeshRenderer",
                                meshRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/column.obj",
                                textureRef = "Assets/3D/KayKit_DungeonRemastered_1.1_FREE/Assets/obj/dungeon_texture.png",
                                colorRef = Color(1.00, 1.00, 1.00, 1.00),
                                meshIndex = 0,
                                multiMesh = false
                            },
                        }
                    },
                    {
                        name = "Torch",
                        uuid = "MM-ENV-TOR-0001-4000-8000-000000000001",
                        position = Vector3(-2.10, 0.00, -2.20),
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
                }
            },
            {
                name = "Canvas",
                uuid = "073830E4-5E61-4968-B33A-433B8A60D77C",
                components = {
                    {
                        type = "Canvas",
                        renderMode = "ScreenSpaceOverlay",
                        canvasSize = Vector2(1920.00, 1080.00),
                        pixelsPerUnit = 100.00,
                        sortOrder = 0,
                        faceCamera = false,
                        faceCameraLockY = false
                    },
                    {
                        type = "AudioSourceComponent",
                        volume = 0.85,
                        pitch = 1.00,
                        loop = false,
                        playOnStart = false,
                        audioClip = "Assets/Audio/UI/button_click.mp3"
                    },
                    {
                        type = "CursorUnlocker"
                    },
                    {
                        type = "MainMenuController",
                        playerNameInput = "8A6DDA41-9A67-4B95-A2B6-E2159C420101/UIInputField",
                        playButton = "2E0ED540-A92F-40A9-B2E7-4906659B3E6E/UIButton",
                        multiplayerButton = "MM-MENU-MPBTN-4000-8000-000000000001/UIButton",
                        statusMessageText = "MM-STATUS-0001-4000-8000-000000000001/UIText",
                        gameScenePath = "Assets/Scenes/DefaultScene.lua",
                        multiplayerMenuScenePath = "Assets/Scenes/MultiplayerMenu.lua"
                    },
                    {
                        type = "CharacterSelectMenuController",
                        characterSelectRoot = "CSEL-OVERLAY-0001-4000-8000-000000000001",
                        overlayStatsText = "CSEL-STATS-0001-4000-8000-000000000001/UIText",
                        summaryStatsText = "CSEL-SUMM-0002-4000-8000-000000000002/UIText",
                        summaryNameText = "CSEL-SUMM-0001-4000-8000-000000000001/UIText",
                        openSelectButton = "CSEL-OPEN-0001-4000-8000-000000000001/UIButton",
                        backButton = "CSEL-BACK-0001-4000-8000-000000000001/UIButton",
                        confirmButton = "CSEL-CONF-0001-4000-8000-000000000001/UIButton",
                        characterCard0 = "CSEL-CARD-0001-4000-8000-000000000001",
                        characterCard1 = "CSEL-CARD-0002-4000-8000-000000000002",
                        characterCard2 = "CSEL-CARD-0003-4000-8000-000000000003",
                        characterCard3 = "CSEL-CARD-0004-4000-8000-000000000004",
                        characterCard4 = "CSEL-CARD-0005-4000-8000-000000000005",
                        quickSelectButton0 = "CSEL-QBTN-0001-4000-8000-000000000001/UIButton",
                        quickSelectButton1 = "CSEL-QBTN-0002-4000-8000-000000000002/UIButton",
                        quickSelectButton2 = "CSEL-QBTN-0003-4000-8000-000000000003/UIButton",
                        quickSelectButton3 = "CSEL-QBTN-0004-4000-8000-000000000004/UIButton",
                        quickSelectButton4 = "CSEL-QBTN-0005-4000-8000-000000000005/UIButton",
                        characterPreview = "CSEL-PRVW-0001-4000-8000-000000000001/MainMenuCharacterPreview",
                        clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent"
                    },
                },
                children = {
                    {
                        name = "Background",
                        uuid = "9E8BE53F-0896-4AEF-B348-7CF8F3C8F881",
                        components = {
                            {
                                type = "UIImage",
                                texture = "Assets/UI/GuildSlate/bg_main_menu.png",
                                tintColor = Color(1.00, 1.00, 1.00, 0.00),
                                preserveAspect = false,
                                isVisible = false,
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
                        name = "CharacterSummaryPanel",
                        uuid = "CSEL-SUMM-0000-4000-8000-000000000000",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.10, 0.12, 0.16, 0.72),
                                borderColor = Color(0.91, 0.66, 0.29, 0.55),
                                borderThickness = 1.50,
                                hasBorder = true,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(1.00, 0.50),
                                anchorMax = Vector2(1.00, 0.50),
                                pivot = Vector2(1.00, 0.50),
                                anchoredPosition = Vector2(-36.00, 0.00),
                                sizeDelta = Vector2(380.00, 460.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        },
                        children = {
                            {
                                name = "SummaryTitle",
                                uuid = "CSEL-SUMM-0001-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Character: Arcanist",
                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                        fontSize = 51.60,
                                        alignment = "Left",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.00, 1.00),
                                        anchorMax = Vector2(1.00, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(20.00, -5.00),
                                        sizeDelta = Vector2(-28.00, 32.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "SummaryStats",
                                uuid = "CSEL-SUMM-0002-4000-8000-000000000002",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Health: 85\nDamage: 13\nSpeed: 4.0\nSprint: x1.60\nShots: 5\nReload: 5.5 s\nArrow speed: 17",
                                        color = Color(0.78, 0.80, 0.86, 1.00),
                                        fontSize = 34.00,
                                        alignment = "Left",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.00, 1.00),
                                        anchorMax = Vector2(1.00, 1.00),
                                        pivot = Vector2(0.50, 1.00),
                                        anchoredPosition = Vector2(19.00, -69.00),
                                        sizeDelta = Vector2(-28.00, -118.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "QuickSelectHint",
                                uuid = "CSEL-QHNT-0001-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Quick select:",
                                        color = Color(0.72, 0.74, 0.80, 1.00),
                                        fontSize = 29.00,
                                        alignment = "Left",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.00, 0.00),
                                        anchorMax = Vector2(1.00, 0.00),
                                        pivot = Vector2(0.50, 0.00),
                                        anchoredPosition = Vector2(18.00, 137.00),
                                        sizeDelta = Vector2(-28.00, 22.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "QuickSelect",
                                uuid = "0205CCE2-A920-4DB3-A1C5-83C4BC4A55AB",
                                position = Vector3(0.00, 1.80, 0.00),
                                children = {
                                    {
                                        name = "QuickSelect_Ranger",
                                        uuid = "CSEL-QBTN-0001-4000-8000-000000000001",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                                borderColor = Color(0.42, 0.44, 0.50, 0.85),
                                                borderThickness = 1.50,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.00),
                                                anchorMax = Vector2(0.50, 0.00),
                                                pivot = Vector2(0.50, 0.00),
                                                anchoredPosition = Vector2(-136.00, 14.00),
                                                sizeDelta = Vector2(62.00, 34.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.10),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.16),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "Label",
                                                uuid = "CSEL-QTX-0001-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Ranger",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 13.00,
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
                                        name = "QuickSelect_Knight",
                                        uuid = "CSEL-QBTN-0002-4000-8000-000000000002",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                                borderColor = Color(0.42, 0.44, 0.50, 0.85),
                                                borderThickness = 1.50,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.00),
                                                anchorMax = Vector2(0.50, 0.00),
                                                pivot = Vector2(0.50, 0.00),
                                                anchoredPosition = Vector2(-68.00, 14.00),
                                                sizeDelta = Vector2(62.00, 34.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.10),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.16),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "Label",
                                                uuid = "CSEL-QTX-0002-4000-8000-000000000002",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Knight",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 13.00,
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
                                        name = "QuickSelect_Arcanist",
                                        uuid = "CSEL-QBTN-0003-4000-8000-000000000003",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.18, 0.20, 0.28, 0.98),
                                                borderColor = Color(0.91, 0.66, 0.29, 1.00),
                                                borderThickness = 2.50,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.00),
                                                anchorMax = Vector2(0.50, 0.00),
                                                pivot = Vector2(0.50, 0.00),
                                                anchoredPosition = Vector2(0.00, 14.00),
                                                sizeDelta = Vector2(62.00, 34.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.10),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.16),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "Label",
                                                uuid = "CSEL-QTX-0003-4000-8000-000000000003",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Arcanist",
                                                        color = Color(1.00, 0.84, 0.47, 1.00),
                                                        fontSize = 13.00,
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
                                        name = "QuickSelect_Rogue",
                                        uuid = "CSEL-QBTN-0004-4000-8000-000000000004",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                                borderColor = Color(0.42, 0.44, 0.50, 0.85),
                                                borderThickness = 1.50,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.00),
                                                anchorMax = Vector2(0.50, 0.00),
                                                pivot = Vector2(0.50, 0.00),
                                                anchoredPosition = Vector2(68.00, 14.00),
                                                sizeDelta = Vector2(62.00, 34.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.10),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.16),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "Label",
                                                uuid = "CSEL-QTX-0004-4000-8000-000000000004",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Rogue",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 13.00,
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
                                        name = "QuickSelect_Barbarian",
                                        uuid = "CSEL-QBTN-0005-4000-8000-000000000005",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                                borderColor = Color(0.42, 0.44, 0.50, 0.85),
                                                borderThickness = 1.50,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.00),
                                                anchorMax = Vector2(0.50, 0.00),
                                                pivot = Vector2(0.50, 0.00),
                                                anchoredPosition = Vector2(136.00, 14.00),
                                                sizeDelta = Vector2(62.00, 34.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.10),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.16),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "Label",
                                                uuid = "CSEL-QTX-0005-4000-8000-000000000005",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Barbarian",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 13.00,
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
                    {
                        name = "StatusMessage",
                        uuid = "MM-STATUS-0001-4000-8000-000000000001",
                        components = {
                            {
                                type = "UIText",
                                text = "",
                                color = Color(1.00, 0.55, 0.45, 1.00),
                                fontSize = 18.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = false,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 168.00),
                                sizeDelta = Vector2(520.00, 48.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "PlayerNameLabel",
                        uuid = "A80E2F9B-2D3A-4A38-BE1D-7D4A2C780101",
                        components = {
                            {
                                type = "UIText",
                                text = "Player Name",
                                color = Color(0.96, 0.94, 0.88, 1.00),
                                fontSize = 18.00,
                                alignment = "Center",
                                font = nil,
                                isVisible = true,
                                raycastTarget = false,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 208.00),
                                sizeDelta = Vector2(260.00, 28.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                        }
                    },
                    {
                        name = "PlayerNameInputField",
                        uuid = "8A6DDA41-9A67-4B95-A2B6-E2159C420101",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                borderColor = Color(0.91, 0.66, 0.29, 0.65),
                                borderThickness = 1.00,
                                hasBorder = true,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 160.00),
                                sizeDelta = Vector2(260.00, 42.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIInputField",
                                text = "",
                                placeholder = "Enter name",
                                maxLength = 24,
                                interactable = true,
                                textColor = Color(0.96, 0.94, 0.88, 1.00),
                                placeholderColor = Color(0.62, 0.60, 0.56, 1.00),
                                normalColor = Color(0.12, 0.14, 0.18, 0.92),
                                focusedColor = Color(0.18, 0.22, 0.28, 1.00),
                                disabledColor = Color(0.20, 0.20, 0.20, 0.50),
                                textComponent = "D68D1E33-2706-41F1-9F13-923EBD130101/UIText",
                                backgroundPanel = "8A6DDA41-9A67-4B95-A2B6-E2159C420101/UIPanel"
                            },
                        },
                        children = {
                            {
                                name = "Text",
                                uuid = "D68D1E33-2706-41F1-9F13-923EBD130101",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Enter name",
                                        color = Color(0.62, 0.60, 0.56, 1.00),
                                        fontSize = 16.00,
                                        alignment = "Left",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.00, 0.00),
                                        anchorMax = Vector2(1.00, 1.00),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(12.00, 0.00),
                                        sizeDelta = Vector2(-24.00, 0.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "Button",
                        uuid = "2E0ED540-A92F-40A9-B2E7-4906659B3E6E",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.20, 0.20, 0.20, 0.00),
                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                borderThickness = 0.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 8.00),
                                sizeDelta = Vector2(320.00, 68.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIButton",
                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                hoveredColor = Color(1.00, 1.00, 1.00, 0.00),
                                pressedColor = Color(1.00, 1.00, 1.00, 0.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                interactable = true,
                                enableDefaultHoverVisuals = false
                            },
                            {
                                type = "ButtonStyle",
                                backgroundImage = "E8F1A2B3-C4D5-4E6F-9012-ABCDEF010101/UIImage",
                                label = "6C243A1E-F469-47BE-9CB9-7DAC1471F31F/UIText",
                                normalTexture = "Assets/UI/GuildSlate/btn_accent_normal.png",
                                hoverTexture = "Assets/UI/GuildSlate/btn_accent_hover.png",
                                pressedTexture = "Assets/UI/GuildSlate/btn_accent_pressed.png",
                                normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverScaleBoost = 1.05,
                                hoverRotationDeg = 0.00,
                                clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                clickScaleBoost = 0.97,
                                disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12,
                                playClickSound = true,
                                clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent"
                            },
                        },
                        children = {
                            {
                                name = "Background",
                                uuid = "E8F1A2B3-C4D5-4E6F-9012-ABCDEF010101",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Assets/UI/GuildSlate/btn_accent_normal.png",
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
                                name = "Text",
                                uuid = "6C243A1E-F469-47BE-9CB9-7DAC1471F31F",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Play",
                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                        fontSize = 22.00,
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
                        name = "MultiplayerButton",
                        uuid = "MM-MENU-MPBTN-4000-8000-000000000001",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.20, 0.20, 0.20, 0.00),
                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                borderThickness = 0.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, -68.00),
                                sizeDelta = Vector2(320.00, 68.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIButton",
                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                hoveredColor = Color(1.00, 1.00, 1.00, 0.00),
                                pressedColor = Color(1.00, 1.00, 1.00, 0.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                interactable = true,
                                enableDefaultHoverVisuals = false
                            },
                            {
                                type = "ButtonStyle",
                                backgroundImage = "MPBG01-2345-6789-ABCD-EF0123456701/UIImage",
                                label = "MPLBL01-2345-6789-ABCD-EF0123456701/UIText",
                                normalTexture = "Assets/UI/GuildSlate/btn_accent_normal.png",
                                hoverTexture = "Assets/UI/GuildSlate/btn_accent_hover.png",
                                pressedTexture = "Assets/UI/GuildSlate/btn_accent_pressed.png",
                                normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverScaleBoost = 1.05,
                                hoverRotationDeg = 0.00,
                                clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                clickScaleBoost = 0.97,
                                disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12,
                                playClickSound = true,
                                clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent"
                            },
                        },
                        children = {
                            {
                                name = "Background",
                                uuid = "MPBG01-2345-6789-ABCD-EF0123456701",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Assets/UI/GuildSlate/btn_accent_normal.png",
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
                                name = "Text",
                                uuid = "MPLBL01-2345-6789-ABCD-EF0123456701",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Multiplayer",
                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                        fontSize = 22.00,
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
                        name = "Button (1)",
                        uuid = "6F093F7A-8C5E-47AE-B7DA-84C929A60009",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.20, 0.20, 0.20, 0.00),
                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                borderThickness = 0.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, -144.00),
                                sizeDelta = Vector2(320.00, 68.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIButton",
                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                hoveredColor = Color(1.00, 1.00, 1.00, 0.00),
                                pressedColor = Color(1.00, 1.00, 1.00, 0.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                interactable = true,
                                enableDefaultHoverVisuals = false
                            },
                            {
                                type = "ButtonStyle",
                                backgroundImage = "E8F1A2B3-C4D5-4E6F-9012-ABCDEF010102/UIImage",
                                label = "6C4B352B-BAFB-4A6C-806C-2E24DB7CB30B/UIText",
                                normalTexture = "Assets/UI/GuildSlate/btn_danger_normal.png",
                                hoverTexture = "Assets/UI/GuildSlate/btn_danger_hover.png",
                                pressedTexture = "Assets/UI/GuildSlate/btn_danger_pressed.png",
                                normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverScaleBoost = 1.05,
                                hoverRotationDeg = 0.00,
                                clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                clickScaleBoost = 0.97,
                                disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12,
                                playClickSound = true,
                                clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent"
                            },
                            {
                                type = "ApplicationQuitButton"
                            },
                        },
                        children = {
                            {
                                name = "Background",
                                uuid = "E8F1A2B3-C4D5-4E6F-9012-ABCDEF010102",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Assets/UI/GuildSlate/btn_danger_normal.png",
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
                                name = "Text",
                                uuid = "6C4B352B-BAFB-4A6C-806C-2E24DB7CB30B",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Exit",
                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                        fontSize = 22.00,
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
                        name = "SelectCharacterButton",
                        uuid = "CSEL-OPEN-0001-4000-8000-000000000001",
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.20, 0.20, 0.20, 0.00),
                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                borderThickness = 0.00,
                                hasBorder = false,
                                isVisible = true,
                                raycastTarget = true,
                                anchorMin = Vector2(0.50, 0.50),
                                anchorMax = Vector2(0.50, 0.50),
                                pivot = Vector2(0.50, 0.50),
                                anchoredPosition = Vector2(0.00, 76.00),
                                sizeDelta = Vector2(320.00, 68.00),
                                rotation = 0.00,
                                scale = Vector2(1.00, 1.00)
                            },
                            {
                                type = "UIButton",
                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                hoveredColor = Color(1.00, 1.00, 1.00, 0.00),
                                pressedColor = Color(1.00, 1.00, 1.00, 0.00),
                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                interactable = true,
                                enableDefaultHoverVisuals = false
                            },
                            {
                                type = "ButtonStyle",
                                backgroundImage = "CSEL-OPEN-0002-4000-8000-000000000002/UIImage",
                                label = "CSEL-OPEN-0003-4000-8000-000000000003/UIText",
                                normalTexture = "Assets/UI/GuildSlate/btn_accent_normal.png",
                                hoverTexture = "Assets/UI/GuildSlate/btn_accent_hover.png",
                                pressedTexture = "Assets/UI/GuildSlate/btn_accent_pressed.png",
                                normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                hoverScaleBoost = 1.05,
                                hoverRotationDeg = 0.00,
                                clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                clickScaleBoost = 0.97,
                                disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
                                hoverInTimeSec = 0.12,
                                hoverOutTimeSec = 0.18,
                                pressInTimeSec = 0.08,
                                pressOutTimeSec = 0.12,
                                playClickSound = true,
                                clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent"
                            },
                        },
                        children = {
                            {
                                name = "Background",
                                uuid = "CSEL-OPEN-0002-4000-8000-000000000002",
                                components = {
                                    {
                                        type = "UIImage",
                                        texture = "Assets/UI/GuildSlate/btn_accent_normal.png",
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
                                name = "Text",
                                uuid = "CSEL-OPEN-0003-4000-8000-000000000003",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Choose character",
                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                        fontSize = 22.00,
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
                        name = "CharacterSelectOverlay",
                        uuid = "CSEL-OVERLAY-0001-4000-8000-000000000001",
                        active = false,
                        components = {
                            {
                                type = "UIPanel",
                                backgroundColor = Color(0.00, 0.00, 0.00, 0.00),
                                borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                borderThickness = 0.00,
                                hasBorder = false,
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
                        },
                        children = {
                            {
                                name = "DimBackground",
                                uuid = "CSEL-DIMB-0001-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.04, 0.05, 0.08, 0.48),
                                        borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                        borderThickness = 0.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
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
                                name = "OverlayTitle",
                                uuid = "CSEL-TITL-0001-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIText",
                                        text = "Choose character",
                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                        fontSize = 34.00,
                                        alignment = "Center",
                                        font = nil,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 0.50),
                                        anchorMax = Vector2(0.50, 0.50),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(0.00, 248.00),
                                        sizeDelta = Vector2(640.00, 48.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                }
                            },
                            {
                                name = "OverlayStatsPanel",
                                uuid = "CSEL-STAT-0000-4000-8000-000000000000",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.10, 0.12, 0.16, 0.92),
                                        borderColor = Color(0.42, 0.44, 0.50, 0.70),
                                        borderThickness = 1.50,
                                        hasBorder = true,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 0.50),
                                        anchorMax = Vector2(0.50, 0.50),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(0.00, 80.00),
                                        sizeDelta = Vector2(920.00, 280.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                },
                                children = {
                                    {
                                        name = "OverlayStatsText",
                                        uuid = "CSEL-STATS-0001-4000-8000-000000000001",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Arcanist\nSwift artillery. Fast reload and more shots, but each arrow deals less damage.\n\nHealth: 85\nSpeed: 4.0\nSprint: x1.60\nShots: 5\nReload: 5.5 s\nDamage: 13\nArrow speed: 17",
                                                color = Color(0.78, 0.80, 0.86, 1.00),
                                                fontSize = 28.00,
                                                alignment = "Left",
                                                font = nil,
                                                isVisible = true,
                                                raycastTarget = false,
                                                anchorMin = Vector2(0.00, 1.00),
                                                anchorMax = Vector2(1.00, 1.00),
                                                pivot = Vector2(0.50, 1.00),
                                                anchoredPosition = Vector2(27.00, 5.00),
                                                sizeDelta = Vector2(-32.00, -28.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                        }
                                    },
                                }
                            },
                            {
                                name = "BackButton",
                                uuid = "CSEL-BACK-0001-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.20, 0.20, 0.20, 0.00),
                                        borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                        borderThickness = 0.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
                                        anchorMin = Vector2(0.50, 0.50),
                                        anchorMax = Vector2(0.50, 0.50),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(-140.00, -370.00),
                                        sizeDelta = Vector2(220.00, 60.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                    {
                                        type = "UIButton",
                                        normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                        hoveredColor = Color(1.00, 1.00, 1.00, 0.00),
                                        pressedColor = Color(1.00, 1.00, 1.00, 0.00),
                                        disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                        interactable = true,
                                        enableDefaultHoverVisuals = false
                                    },
                                    {
                                        type = "ButtonStyle",
                                        backgroundImage = "CSEL-BACK-0002-4000-8000-000000000002/UIImage",
                                        label = "CSEL-BACK-0003-4000-8000-000000000003/UIText",
                                        normalTexture = "Assets/UI/GuildSlate/btn_primary_normal.png",
                                        hoverTexture = "Assets/UI/GuildSlate/btn_primary_hover.png",
                                        pressedTexture = "Assets/UI/GuildSlate/btn_primary_pressed.png",
                                        normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                        normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                        hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverScaleBoost = 1.04,
                                        hoverRotationDeg = 0.00,
                                        clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                        clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                        clickScaleBoost = 0.97,
                                        disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                        disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
                                        hoverInTimeSec = 0.12,
                                        hoverOutTimeSec = 0.18,
                                        pressInTimeSec = 0.08,
                                        pressOutTimeSec = 0.12,
                                        playClickSound = true,
                                        clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent"
                                    },
                                },
                                children = {
                                    {
                                        name = "Background",
                                        uuid = "CSEL-BACK-0002-4000-8000-000000000002",
                                        components = {
                                            {
                                                type = "UIImage",
                                                texture = "Assets/UI/GuildSlate/btn_primary_normal.png",
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
                                        name = "Text",
                                        uuid = "CSEL-BACK-0003-4000-8000-000000000003",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Back",
                                                color = Color(0.96, 0.94, 0.88, 1.00),
                                                fontSize = 20.00,
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
                                name = "ConfirmButton",
                                uuid = "CSEL-CONF-0001-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.20, 0.20, 0.20, 0.00),
                                        borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                        borderThickness = 0.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = true,
                                        anchorMin = Vector2(0.50, 0.50),
                                        anchorMax = Vector2(0.50, 0.50),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(140.00, -370.00),
                                        sizeDelta = Vector2(220.00, 60.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                    {
                                        type = "UIButton",
                                        normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                        hoveredColor = Color(1.00, 1.00, 1.00, 0.00),
                                        pressedColor = Color(1.00, 1.00, 1.00, 0.00),
                                        disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                        interactable = true,
                                        enableDefaultHoverVisuals = false
                                    },
                                    {
                                        type = "ButtonStyle",
                                        backgroundImage = "CSEL-CONF-0002-4000-8000-000000000002/UIImage",
                                        label = "CSEL-CONF-0003-4000-8000-000000000003/UIText",
                                        normalTexture = "Assets/UI/GuildSlate/btn_accent_normal.png",
                                        hoverTexture = "Assets/UI/GuildSlate/btn_accent_hover.png",
                                        pressedTexture = "Assets/UI/GuildSlate/btn_accent_pressed.png",
                                        normalTextColor = Color(0.96, 0.94, 0.88, 1.00),
                                        normalImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverTextColor = Color(1.00, 0.82, 0.47, 1.00),
                                        hoverImageTint = Color(1.00, 1.00, 1.00, 1.00),
                                        hoverScaleBoost = 1.04,
                                        hoverRotationDeg = 0.00,
                                        clickTextColor = Color(0.86, 0.84, 0.78, 1.00),
                                        clickImageTint = Color(0.92, 0.92, 0.92, 1.00),
                                        clickScaleBoost = 0.97,
                                        disabledTextColor = Color(0.55, 0.54, 0.50, 1.00),
                                        disabledImageTint = Color(0.45, 0.45, 0.45, 1.00),
                                        hoverInTimeSec = 0.12,
                                        hoverOutTimeSec = 0.18,
                                        pressInTimeSec = 0.08,
                                        pressOutTimeSec = 0.12,
                                        playClickSound = true,
                                        clickAudio = "073830E4-5E61-4968-B33A-433B8A60D77C/AudioSourceComponent"
                                    },
                                },
                                children = {
                                    {
                                        name = "Background",
                                        uuid = "CSEL-CONF-0002-4000-8000-000000000002",
                                        components = {
                                            {
                                                type = "UIImage",
                                                texture = "Assets/UI/GuildSlate/btn_accent_normal.png",
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
                                        name = "Text",
                                        uuid = "CSEL-CONF-0003-4000-8000-000000000003",
                                        components = {
                                            {
                                                type = "UIText",
                                                text = "Confirm",
                                                color = Color(0.96, 0.94, 0.88, 1.00),
                                                fontSize = 20.00,
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
                                name = "CharacterCards",
                                uuid = "CSEL-CARDS-0000-4000-8000-000000000000",
                                components = {
                                    {
                                        type = "UIPanel",
                                        backgroundColor = Color(0.00, 0.00, 0.00, 0.00),
                                        borderColor = Color(0.00, 0.00, 0.00, 0.00),
                                        borderThickness = 0.00,
                                        hasBorder = false,
                                        isVisible = true,
                                        raycastTarget = false,
                                        anchorMin = Vector2(0.50, 0.50),
                                        anchorMax = Vector2(0.50, 0.50),
                                        pivot = Vector2(0.50, 0.50),
                                        anchoredPosition = Vector2(0.00, -200.00),
                                        sizeDelta = Vector2(880.00, 210.00),
                                        rotation = 0.00,
                                        scale = Vector2(1.00, 1.00)
                                    },
                                },
                                children = {
                                    {
                                        name = "CharacterCard_0",
                                        uuid = "CSEL-CARD-0001-4000-8000-000000000001",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                                borderColor = Color(0.42, 0.44, 0.50, 0.85),
                                                borderThickness = 1.50,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.50),
                                                anchorMax = Vector2(0.50, 0.50),
                                                pivot = Vector2(0.50, 0.50),
                                                anchoredPosition = Vector2(-344.00, 0.00),
                                                sizeDelta = Vector2(156.00, 196.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.08),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.14),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "PortraitPlaceholder",
                                                uuid = "CSEL-PORT-0001-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "UIImage",
                                                        texture = "Assets/UI/GuildSlate/btn_primary_normal.png",
                                                        tintColor = Color(0.55, 0.58, 0.64, 0.35),
                                                        preserveAspect = true,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.50, 1.00),
                                                        anchorMax = Vector2(0.50, 1.00),
                                                        pivot = Vector2(0.50, 1.00),
                                                        anchoredPosition = Vector2(0.00, -141.00),
                                                        sizeDelta = Vector2(120.00, 120.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "NameLabel",
                                                uuid = "CSEL-LABL-0001-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Ranger",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 18.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 28.00),
                                                        sizeDelta = Vector2(-12.00, 28.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "SelectHint",
                                                uuid = "CSEL-HINT-0001-4000-8000-000000000001",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Select",
                                                        color = Color(0.91, 0.66, 0.29, 1.00),
                                                        fontSize = 14.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 8.00),
                                                        sizeDelta = Vector2(-12.00, 20.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "CharacterCard_1",
                                        uuid = "CSEL-CARD-0002-4000-8000-000000000002",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                                borderColor = Color(0.42, 0.44, 0.50, 0.85),
                                                borderThickness = 1.50,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.50),
                                                anchorMax = Vector2(0.50, 0.50),
                                                pivot = Vector2(0.50, 0.50),
                                                anchoredPosition = Vector2(-172.00, 0.00),
                                                sizeDelta = Vector2(156.00, 196.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.08),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.14),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "PortraitPlaceholder",
                                                uuid = "CSEL-PORT-0002-4000-8000-000000000002",
                                                components = {
                                                    {
                                                        type = "UIImage",
                                                        texture = "Assets/UI/GuildSlate/btn_primary_normal.png",
                                                        tintColor = Color(0.55, 0.58, 0.64, 0.35),
                                                        preserveAspect = true,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.50, 1.00),
                                                        anchorMax = Vector2(0.50, 1.00),
                                                        pivot = Vector2(0.50, 1.00),
                                                        anchoredPosition = Vector2(0.00, -141.00),
                                                        sizeDelta = Vector2(120.00, 120.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "NameLabel",
                                                uuid = "CSEL-LABL-0002-4000-8000-000000000002",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Knight",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 18.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 28.00),
                                                        sizeDelta = Vector2(-12.00, 28.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "SelectHint",
                                                uuid = "CSEL-HINT-0002-4000-8000-000000000002",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Select",
                                                        color = Color(0.91, 0.66, 0.29, 1.00),
                                                        fontSize = 14.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 8.00),
                                                        sizeDelta = Vector2(-12.00, 20.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "CharacterCard_2",
                                        uuid = "CSEL-CARD-0003-4000-8000-000000000003",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.18, 0.20, 0.28, 0.98),
                                                borderColor = Color(0.91, 0.66, 0.29, 1.00),
                                                borderThickness = 3.00,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.50),
                                                anchorMax = Vector2(0.50, 0.50),
                                                pivot = Vector2(0.50, 0.50),
                                                anchoredPosition = Vector2(0.00, 0.00),
                                                sizeDelta = Vector2(156.00, 196.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.08),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.14),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "PortraitPlaceholder",
                                                uuid = "CSEL-PORT-0003-4000-8000-000000000003",
                                                components = {
                                                    {
                                                        type = "UIImage",
                                                        texture = "Assets/UI/GuildSlate/btn_primary_normal.png",
                                                        tintColor = Color(0.55, 0.58, 0.64, 0.35),
                                                        preserveAspect = true,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.50, 1.00),
                                                        anchorMax = Vector2(0.50, 1.00),
                                                        pivot = Vector2(0.50, 1.00),
                                                        anchoredPosition = Vector2(0.00, -141.00),
                                                        sizeDelta = Vector2(120.00, 120.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "NameLabel",
                                                uuid = "CSEL-LABL-0003-4000-8000-000000000003",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Arcanist",
                                                        color = Color(1.00, 0.84, 0.47, 1.00),
                                                        fontSize = 18.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 28.00),
                                                        sizeDelta = Vector2(-12.00, 28.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "SelectHint",
                                                uuid = "CSEL-HINT-0003-4000-8000-000000000003",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Select",
                                                        color = Color(0.91, 0.66, 0.29, 1.00),
                                                        fontSize = 14.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 8.00),
                                                        sizeDelta = Vector2(-12.00, 20.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "CharacterCard_3",
                                        uuid = "CSEL-CARD-0004-4000-8000-000000000004",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                                borderColor = Color(0.42, 0.44, 0.50, 0.85),
                                                borderThickness = 1.50,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.50),
                                                anchorMax = Vector2(0.50, 0.50),
                                                pivot = Vector2(0.50, 0.50),
                                                anchoredPosition = Vector2(172.00, 0.00),
                                                sizeDelta = Vector2(156.00, 196.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.08),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.14),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "PortraitPlaceholder",
                                                uuid = "CSEL-PORT-0004-4000-8000-000000000004",
                                                components = {
                                                    {
                                                        type = "UIImage",
                                                        texture = "Assets/UI/GuildSlate/btn_primary_normal.png",
                                                        tintColor = Color(0.55, 0.58, 0.64, 0.35),
                                                        preserveAspect = true,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.50, 1.00),
                                                        anchorMax = Vector2(0.50, 1.00),
                                                        pivot = Vector2(0.50, 1.00),
                                                        anchoredPosition = Vector2(0.00, -141.00),
                                                        sizeDelta = Vector2(120.00, 120.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "NameLabel",
                                                uuid = "CSEL-LABL-0004-4000-8000-000000000004",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Rogue",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 18.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 28.00),
                                                        sizeDelta = Vector2(-12.00, 28.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "SelectHint",
                                                uuid = "CSEL-HINT-0004-4000-8000-000000000004",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Select",
                                                        color = Color(0.91, 0.66, 0.29, 1.00),
                                                        fontSize = 14.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 8.00),
                                                        sizeDelta = Vector2(-12.00, 20.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                        }
                                    },
                                    {
                                        name = "CharacterCard_4",
                                        uuid = "CSEL-CARD-0005-4000-8000-000000000005",
                                        components = {
                                            {
                                                type = "UIPanel",
                                                backgroundColor = Color(0.12, 0.14, 0.18, 0.92),
                                                borderColor = Color(0.42, 0.44, 0.50, 0.85),
                                                borderThickness = 1.50,
                                                hasBorder = true,
                                                isVisible = true,
                                                raycastTarget = true,
                                                anchorMin = Vector2(0.50, 0.50),
                                                anchorMax = Vector2(0.50, 0.50),
                                                pivot = Vector2(0.50, 0.50),
                                                anchoredPosition = Vector2(344.00, 0.00),
                                                sizeDelta = Vector2(156.00, 196.00),
                                                rotation = 0.00,
                                                scale = Vector2(1.00, 1.00)
                                            },
                                            {
                                                type = "UIButton",
                                                normalColor = Color(1.00, 1.00, 1.00, 0.00),
                                                hoveredColor = Color(1.00, 1.00, 1.00, 0.08),
                                                pressedColor = Color(1.00, 1.00, 1.00, 0.14),
                                                disabledColor = Color(0.50, 0.50, 0.50, 0.00),
                                                interactable = true,
                                                enableDefaultHoverVisuals = false
                                            },
                                        },
                                        children = {
                                            {
                                                name = "PortraitPlaceholder",
                                                uuid = "CSEL-PORT-0005-4000-8000-000000000005",
                                                components = {
                                                    {
                                                        type = "UIImage",
                                                        texture = "Assets/UI/GuildSlate/btn_primary_normal.png",
                                                        tintColor = Color(0.55, 0.58, 0.64, 0.35),
                                                        preserveAspect = true,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.50, 1.00),
                                                        anchorMax = Vector2(0.50, 1.00),
                                                        pivot = Vector2(0.50, 1.00),
                                                        anchoredPosition = Vector2(0.00, -141.00),
                                                        sizeDelta = Vector2(120.00, 120.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "NameLabel",
                                                uuid = "CSEL-LABL-0005-4000-8000-000000000005",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Barbarian",
                                                        color = Color(0.96, 0.94, 0.88, 1.00),
                                                        fontSize = 18.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 28.00),
                                                        sizeDelta = Vector2(-12.00, 28.00),
                                                        rotation = 0.00,
                                                        scale = Vector2(1.00, 1.00)
                                                    },
                                                }
                                            },
                                            {
                                                name = "SelectHint",
                                                uuid = "CSEL-HINT-0005-4000-8000-000000000005",
                                                components = {
                                                    {
                                                        type = "UIText",
                                                        text = "Select",
                                                        color = Color(0.91, 0.66, 0.29, 1.00),
                                                        fontSize = 14.00,
                                                        alignment = "Center",
                                                        font = nil,
                                                        isVisible = true,
                                                        raycastTarget = false,
                                                        anchorMin = Vector2(0.00, 0.00),
                                                        anchorMax = Vector2(1.00, 0.00),
                                                        pivot = Vector2(0.50, 0.00),
                                                        anchoredPosition = Vector2(0.00, 8.00),
                                                        sizeDelta = Vector2(-12.00, 20.00),
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
            },
        }
    }
end
