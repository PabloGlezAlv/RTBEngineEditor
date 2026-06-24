#include "TestDataAsset.h"

using ThisClass = TestDataAsset;

RTB_REGISTER_DATA_ASSET(TestDataAsset)
    RTB_PROPERTY(displayName)
    RTB_PROPERTY(floatValue)
    RTB_PROPERTY_ASSET_PATH(textureRef, "texture")
    RTB_PROPERTY_DATA_ASSET(nestedDataAssetRef)
RTB_END_REGISTER_DATA_ASSET(TestDataAsset)
