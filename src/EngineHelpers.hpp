#pragma once
#include <string>

class EngineHelpers
{
public:
    static std::string GetAssetFullPath(std::string assetName);

private:
    // Root assets path.
    static std::string m_assetsPath;
    static bool assetsPathInitialized;
};
