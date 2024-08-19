#include "stdafx.hpp"
#include "EngineHelpers.hpp"

#include "DXSampleHelper.h"

bool EngineHelpers::assetsPathInitialized = false;
std::string EngineHelpers::m_assetsPath;

std::string EngineHelpers::GetAssetFullPath(std::string assetName)
{
    if (!assetsPathInitialized)
    {
        WCHAR assetsPath[512];
        GetAssetsPath(assetsPath, _countof(assetsPath));
        std::string stringAssetsPath = std::filesystem::path(assetsPath).string();
        m_assetsPath = stringAssetsPath + "Resources\\";
    }
    return m_assetsPath + assetName;
}
