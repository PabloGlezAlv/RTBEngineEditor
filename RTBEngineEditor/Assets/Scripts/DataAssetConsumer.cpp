#include "DataAssetConsumer.h"
#include "TestDataAsset.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Data/DataAsset.h>

using ThisClass = DataAssetConsumer;

RTB_REGISTER_COMPONENT(DataAssetConsumer)
    RTB_PROPERTY_DATA_ASSET(dataAssetRef)
RTB_END_REGISTER(DataAssetConsumer)

void DataAssetConsumer::OnStart()
{
    LogLoadedValues();
}

void DataAssetConsumer::LogLoadedValues()
{
    if (dataAssetRef.empty()) {
        RTB_WARN("DataAssetConsumer: dataAssetRef is empty.");
        return;
    }

    RTBEngine::Data::DataAsset* asset =
        RTBEngine::Core::ResourceManager::GetInstance().LoadDataAsset(dataAssetRef);
    if (!asset) {
        RTB_ERROR("DataAssetConsumer: Failed to load data asset '" + dataAssetRef + "'.");
        return;
    }

    auto* testAsset = dynamic_cast<TestDataAsset*>(asset);
    if (!testAsset) {
        RTB_WARN("DataAssetConsumer: Loaded asset is not a TestDataAsset.");
        return;
    }

    RTB_INFO(
        "DataAssetConsumer: Loaded '" + dataAssetRef + "' -> displayName='"
        + testAsset->displayName + "', floatValue=" + std::to_string(testAsset->floatValue)
        + ", textureRef='" + testAsset->textureRef + "'.");

    if (!testAsset->nestedDataAssetRef.empty()) {
        if (RTBEngine::Data::DataAsset* nested =
                RTBEngine::Core::ResourceManager::GetInstance().LoadDataAsset(testAsset->nestedDataAssetRef)) {
            RTB_INFO("DataAssetConsumer: Nested asset type='" + std::string(nested->GetTypeName()) + "'.");
        }
    }
}
