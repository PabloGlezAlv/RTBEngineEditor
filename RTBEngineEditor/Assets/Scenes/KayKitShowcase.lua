function CreateScene()
    return {
        name = "KayKit Showcase",
        skyboxEnabled = true,
        skybox = "Assets/Cubemap/NewCubemap.cubemap",
        gameObjects = {
            {
                name = "Main Camera",
                uuid = "B1000001-0000-4000-8000-000000000001",
                position = Vector3(0.00, 2.50, -10.00),
                rotation = Quaternion.FromEulerAngles(10.00, 0.00, 0.00),
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
                rotation = Quaternion.FromEulerAngles(41.56, -41.93, -30.79),
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
                        playing = true,
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
                        children = {
                            {
                                name = "root",
                                uuid = "2444A1A0-040B-4D06-A8C5-558DE7518C06",
                                children = {
                                    {
                                        name = "hips",
                                        uuid = "D98ED918-9305-4374-9B20-F915EFA18530",
                                        position = Vector3(0.00, 0.41, 0.00),
                                        children = {
                                            {
                                                name = "spine",
                                                uuid = "57C88A11-D7A8-4AFE-956A-A736F81CF9AD",
                                                position = Vector3(-0.00, 0.19, 0.00),
                                                children = {
                                                    {
                                                        name = "chest",
                                                        uuid = "AA2E9030-2046-4992-B5E7-B252207E1074",
                                                        position = Vector3(-0.00, 0.37, 0.00),
                                                        children = {
                                                            {
                                                                name = "upperarm.l",
                                                                uuid = "2EAD8102-8D21-4FE0-AFA8-68B58D2C3E9E",
                                                                position = Vector3(0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-3.28, 90.00, -90.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.l",
                                                                        uuid = "A89B67CD-BD7C-457A-A4A0-B7CA5DB110A8",
                                                                        position = Vector3(-0.00, 0.24, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, -6.34),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.l",
                                                                                uuid = "C7CA8759-3493-482B-83A8-9408DD048465",
                                                                                position = Vector3(0.00, 0.26, 0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.l",
                                                                                        uuid = "F1090621-C795-4B86-8BBF-223CA1314379",
                                                                                        position = Vector3(0.00, 0.07, 0.00),
                                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.l",
                                                                                                uuid = "DF555AFD-00AF-4D03-B8A5-0C4332B5B3F7",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -90.00),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Knight_Shield",
                                                                                                        components = {
                                                                                                            {
                                                                                                                type = "MeshRenderer",
                                                                                                                model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/shield_square.fbx",
                                                                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/knight_texture.png",
                                                                                                                shader = "basic"
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
                                                                position = Vector3(-0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-3.28, -90.00, 90.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.r",
                                                                        uuid = "ECAB2189-412B-41FB-85E2-CAAD265E01B0",
                                                                        position = Vector3(0.00, 0.24, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, 6.34),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.r",
                                                                                uuid = "EA3EFC06-9B7B-407A-A898-5AAD59D7B1D1",
                                                                                position = Vector3(-0.00, 0.26, 0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.r",
                                                                                        uuid = "DFA07C87-73B0-4107-828F-F60485955882",
                                                                                        position = Vector3(-0.00, 0.07, 0.00),
                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.r",
                                                                                                uuid = "248FFEC0-15B2-4A1F-BE8A-BA66B738A18A",
                                                                                                position = Vector3(-0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 90.00),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Knight_Sword",
                                                                                                        components = {
                                                                                                            {
                                                                                                                type = "MeshRenderer",
                                                                                                                model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/sword_1handed.fbx",
                                                                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/knight_texture.png",
                                                                                                                shader = "basic"
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
                                                                position = Vector3(-0.00, 0.27, -0.00),
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.l",
                                                uuid = "BD5BB1C6-ADE9-4DFE-8BA2-7C062EFD6376",
                                                position = Vector3(0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(1.99, 180.00, 180.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.l",
                                                        uuid = "DDEFC779-C160-4296-9FEF-05A95C232F94",
                                                        position = Vector3(0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(12.20, -0.00, -0.00),
                                                        children = {
                                                            {
                                                                name = "foot.l",
                                                                uuid = "96F6D0F4-713A-4317-98D2-724D715AD9D5",
                                                                position = Vector3(-0.00, 0.15, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-54.16, -0.00, 0.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.l",
                                                                        uuid = "7BEA2AB2-9B95-470D-96EE-6C14C9FCC7F4",
                                                                        position = Vector3(0.00, 0.17, 0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.04, -180.00, -0.00),
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
                                                position = Vector3(-0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(1.99, 180.00, -180.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.r",
                                                        uuid = "2EACD0E9-5809-4008-98D7-CABAA209302C",
                                                        position = Vector3(-0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(12.20, 0.00, 0.00),
                                                        children = {
                                                            {
                                                                name = "foot.r",
                                                                uuid = "03B1A0C5-E9E9-4604-98DF-9778EA07B3D6",
                                                                position = Vector3(0.00, 0.15, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-54.16, -0.00, 0.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.r",
                                                                        uuid = "C325E49B-855C-4F17-97FE-E299CF4ED607",
                                                                        position = Vector3(-0.00, 0.17, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.04, -180.00, -0.00),
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
                        playing = true,
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
                        name = "handslot.l",
                        children = {
                            {
                                name = "Barbarian_Shield",
                                position = Vector3(0.00, 0.10, -0.06),
                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, -90.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/shield_round_barbarian.obj",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/barbarian_texture.png",
                                        shader = "basic"
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "handslot.r",
                        children = {
                            {
                                name = "Barbarian_Axe",
                                position = Vector3(0.00, 0.10, -0.06),
                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 90.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/axe_2handed.obj",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/barbarian_texture.png",
                                        shader = "basic"
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
                        playing = true,
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
                        children = {
                            {
                                name = "root",
                                uuid = "7BD6D9AC-6D1E-4BB4-BDA6-61432173859F",
                                children = {
                                    {
                                        name = "hips",
                                        uuid = "D29D3385-D09C-470A-ACDD-3F9CBA5FEC85",
                                        position = Vector3(0.00, 0.41, 0.00),
                                        children = {
                                            {
                                                name = "spine",
                                                uuid = "FBBB3E8C-D912-4E88-946A-98278935A008",
                                                position = Vector3(-0.00, 0.19, 0.00),
                                                children = {
                                                    {
                                                        name = "chest",
                                                        uuid = "95468F1C-74E7-4A49-94F8-2858DF8CD276",
                                                        position = Vector3(-0.00, 0.37, 0.00),
                                                        children = {
                                                            {
                                                                name = "upperarm.l",
                                                                uuid = "59347BA7-70FA-461E-A112-7C699DF997EE",
                                                                position = Vector3(0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-3.28, 90.00, -90.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.l",
                                                                        uuid = "C01D3F70-5EDF-4A2A-A5CA-00913F391453",
                                                                        position = Vector3(-0.00, 0.24, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, -6.34),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.l",
                                                                                uuid = "743E122E-F991-4DD3-9C56-74CF4B5D03BA",
                                                                                position = Vector3(0.00, 0.26, 0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.l",
                                                                                        uuid = "97B3A052-78F7-467A-B02E-FD279F851529",
                                                                                        position = Vector3(0.00, 0.07, 0.00),
                                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.l",
                                                                                                uuid = "D4D03F2F-C0E5-4A73-BA78-7F2EFBDD295D",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -90.00),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Mage_Spellbook",
                                                                                                        components = {
                                                                                                            {
                                                                                                                type = "MeshRenderer",
                                                                                                                model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/spellbook_open.fbx",
                                                                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/mage_texture.png",
                                                                                                                shader = "basic"
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
                                                                position = Vector3(-0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-3.28, -90.00, 90.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.r",
                                                                        uuid = "2FED6737-76D4-4359-909C-EA0705EFDEDC",
                                                                        position = Vector3(0.00, 0.24, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, 6.34),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.r",
                                                                                uuid = "1665C12A-065A-49A0-8B86-A0419E8C2BB2",
                                                                                position = Vector3(-0.00, 0.26, 0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.r",
                                                                                        uuid = "31CFFDAC-16DE-4EA1-8087-A4471B20A856",
                                                                                        position = Vector3(-0.00, 0.07, 0.00),
                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.r",
                                                                                                uuid = "916A9099-94D8-45A2-BA15-A2D292CDB206",
                                                                                                position = Vector3(-0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 90.00),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Mage_Staff",
                                                                                                        components = {
                                                                                                            {
                                                                                                                type = "MeshRenderer",
                                                                                                                model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/staff.fbx",
                                                                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/mage_texture.png",
                                                                                                                shader = "basic"
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
                                                                position = Vector3(-0.00, 0.27, -0.00),
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.l",
                                                uuid = "BA9B352E-7393-464A-8BBF-8F6D9E23470E",
                                                position = Vector3(0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(1.99, 180.00, 180.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.l",
                                                        uuid = "712C49FC-FF36-434A-AF73-342329B105F8",
                                                        position = Vector3(0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(12.20, -0.00, -0.00),
                                                        children = {
                                                            {
                                                                name = "foot.l",
                                                                uuid = "3E6EF9C4-DEAA-4317-BBE7-7F21D0F7EC3C",
                                                                position = Vector3(-0.00, 0.15, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-54.16, -0.00, 0.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.l",
                                                                        uuid = "D080703E-5EF1-4EA8-B06F-A319ADFD9120",
                                                                        position = Vector3(0.00, 0.17, 0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.04, -180.00, -0.00),
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
                                                position = Vector3(-0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(1.99, 180.00, -180.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.r",
                                                        uuid = "4A23FE6B-8312-4EAB-B8C4-C0D1788126F4",
                                                        position = Vector3(-0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(12.20, 0.00, 0.00),
                                                        children = {
                                                            {
                                                                name = "foot.r",
                                                                uuid = "8F894E65-99F9-4E67-A7CB-A9F6D8441353",
                                                                position = Vector3(0.00, 0.15, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-54.16, -0.00, 0.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.r",
                                                                        uuid = "D5BA142F-1A06-46C4-AB9D-5B5A3B1E7C98",
                                                                        position = Vector3(-0.00, 0.17, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.04, -180.00, -0.00),
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
                        playing = true,
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
                        name = "handslot.l",
                        children = {
                            {
                                name = "Ranger_Bow",
                                position = Vector3(0.00, 0.10, -0.06),
                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, -90.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/bow_withString.obj",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/ranger_texture.png",
                                        shader = "basic"
                                    },
                                }
                            },
                        }
                    },
                    {
                        name = "handslot.r",
                        children = {
                            {
                                name = "Ranger_Arrow",
                                position = Vector3(0.00, 0.10, -0.06),
                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 90.00),
                                components = {
                                    {
                                        type = "MeshRenderer",
                                        model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/arrow_bow.obj",
                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/ranger_texture.png",
                                        shader = "basic"
                                    },
                                }
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
                        playing = true,
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
                        children = {
                            {
                                name = "root",
                                uuid = "AED578F8-4615-4B55-8071-89AD8E9B140F",
                                children = {
                                    {
                                        name = "hips",
                                        uuid = "7E04A8AB-CE92-4C23-8998-54FC7ADDEA79",
                                        position = Vector3(0.00, 0.41, 0.00),
                                        children = {
                                            {
                                                name = "spine",
                                                uuid = "1AD128FF-9250-47E5-80FA-70B763862261",
                                                position = Vector3(-0.00, 0.19, 0.00),
                                                children = {
                                                    {
                                                        name = "chest",
                                                        uuid = "1AEE9925-0F5D-434D-A6B3-C6152905EF6D",
                                                        position = Vector3(-0.00, 0.37, 0.00),
                                                        children = {
                                                            {
                                                                name = "Rogue_Crossbow",
                                                                position = Vector3(0.00, 0.10, -0.18),
                                                                rotation = Quaternion.FromEulerAngles(-70.00, 0.00, 0.00),
                                                                components = {
                                                                    {
                                                                        type = "MeshRenderer",
                                                                        model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/crossbow_2handed.fbx",
                                                                        textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/rogue_texture.png",
                                                                        shader = "basic"
                                                                    },
                                                                }
                                                            },
                                                            {
                                                                name = "upperarm.l",
                                                                uuid = "DCB7813D-CC4D-452F-B095-5C21FE0D6220",
                                                                position = Vector3(0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-3.28, 90.00, -90.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.l",
                                                                        uuid = "4AD7FC37-BDA7-4DC0-B330-2B92317BFEA8",
                                                                        position = Vector3(-0.00, 0.24, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, -6.34),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.l",
                                                                                uuid = "51C6C0E1-D5FC-4D8C-98A3-F5E0472B8A56",
                                                                                position = Vector3(0.00, 0.26, 0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.l",
                                                                                        uuid = "E0A76D6E-1003-4219-BAC5-64516FBC05D7",
                                                                                        position = Vector3(0.00, 0.07, 0.00),
                                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, -0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.l",
                                                                                                uuid = "E0C3877C-7262-4D38-A107-5A385EBAB621",
                                                                                                position = Vector3(0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -90.00),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Rogue_Smokebomb",
                                                                                                        components = {
                                                                                                            {
                                                                                                                type = "MeshRenderer",
                                                                                                                model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/smokebomb.fbx",
                                                                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/rogue_texture.png",
                                                                                                                shader = "basic"
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
                                                                position = Vector3(-0.21, 0.13, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-3.28, -90.00, 90.00),
                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                children = {
                                                                    {
                                                                        name = "lowerarm.r",
                                                                        uuid = "F9870941-FE30-4E1C-9DAA-6FDCCED79596",
                                                                        position = Vector3(0.00, 0.24, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(0.00, -0.00, 6.34),
                                                                        children = {
                                                                            {
                                                                                name = "wrist.r",
                                                                                uuid = "CEA2CB5C-301A-4CF2-B982-641E044D8038",
                                                                                position = Vector3(-0.00, 0.26, 0.00),
                                                                                rotation = Quaternion.FromEulerAngles(0.00, -0.00, -3.06),
                                                                                scale = Vector3(1.00, 1.00, 1.00),
                                                                                children = {
                                                                                    {
                                                                                        name = "hand.r",
                                                                                        uuid = "ECFCE6D5-E6FE-4D79-AFF3-91D7C9954F8C",
                                                                                        position = Vector3(-0.00, 0.07, 0.00),
                                                                                        rotation = Quaternion.FromEulerAngles(0.00, 0.00, 0.00),
                                                                                        children = {
                                                                                            {
                                                                                                name = "handslot.r",
                                                                                                uuid = "032CE981-3E1B-4825-8D05-E06FFC35A4FB",
                                                                                                position = Vector3(-0.00, 0.10, -0.06),
                                                                                                rotation = Quaternion.FromEulerAngles(0.00, 0.00, 90.00),
                                                                                                children = {
                                                                                                    {
                                                                                                        name = "Rogue_Dagger",
                                                                                                        components = {
                                                                                                            {
                                                                                                                type = "MeshRenderer",
                                                                                                                model = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/fbx(unity)/dagger.fbx",
                                                                                                                textureRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/rogue_texture.png",
                                                                                                                shader = "basic"
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
                                                                position = Vector3(-0.00, 0.27, -0.00),
                                                            },
                                                        }
                                                    },
                                                }
                                            },
                                            {
                                                name = "upperleg.l",
                                                uuid = "9C6B8CB2-088A-4C3C-932E-9B14DB4DA1F0",
                                                position = Vector3(0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(1.99, 180.00, 180.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.l",
                                                        uuid = "E1C0A255-E13C-4565-9D92-31A721D77A99",
                                                        position = Vector3(0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(12.20, -0.00, -0.00),
                                                        children = {
                                                            {
                                                                name = "foot.l",
                                                                uuid = "09C1EE86-304A-4A36-A576-951601130EEF",
                                                                position = Vector3(-0.00, 0.15, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-54.16, -0.00, 0.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.l",
                                                                        uuid = "82358F2E-9B43-444E-9543-0ED67AEF47A9",
                                                                        position = Vector3(0.00, 0.17, 0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.04, -180.00, -0.00),
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
                                                position = Vector3(-0.17, 0.11, 0.00),
                                                rotation = Quaternion.FromEulerAngles(1.99, 180.00, -180.00),
                                                children = {
                                                    {
                                                        name = "lowerleg.r",
                                                        uuid = "27938078-D909-43F1-B564-C6B3962E2430",
                                                        position = Vector3(-0.00, 0.23, -0.00),
                                                        rotation = Quaternion.FromEulerAngles(12.20, 0.00, 0.00),
                                                        children = {
                                                            {
                                                                name = "foot.r",
                                                                uuid = "D8EEC920-7883-425B-8857-CF895F82D9F1",
                                                                position = Vector3(0.00, 0.15, 0.00),
                                                                rotation = Quaternion.FromEulerAngles(-54.16, -0.00, 0.00),
                                                                children = {
                                                                    {
                                                                        name = "toes.r",
                                                                        uuid = "FEAA3E1C-0151-41BC-9D65-5C1D40D397BA",
                                                                        position = Vector3(-0.00, 0.17, -0.00),
                                                                        rotation = Quaternion.FromEulerAngles(-46.04, -180.00, -0.00),
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
                }
            },
        }
    }
end
