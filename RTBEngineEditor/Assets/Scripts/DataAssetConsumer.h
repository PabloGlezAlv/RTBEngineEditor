#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <string>

class TestDataAsset;

class DataAssetConsumer : public RTBEngine::ECS::Component {
public:
    std::string dataAssetRef = "Assets/Data/TestExample.rtbasset";

    RTB_COMPONENT(DataAssetConsumer)

    void OnStart() override;

private:
    void LogLoadedValues();
};
