#pragma once

#include <RTBEngine/Data/DataAsset.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <string>

class TestDataAsset : public RTBEngine::Data::DataAsset {
public:
    std::string displayName = "Example";
    float floatValue = 1.0f;
    std::string textureRef;
    std::string nestedDataAssetRef;

    RTB_DATA_ASSET(TestDataAsset)
};
