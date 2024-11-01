#pragma once
#include "StandardMaterial.hpp"
class DefaultTexturedMaterial : public StandardMaterial
{
public:
    DefaultTexturedMaterial() = default;
    DefaultTexturedMaterial(const std::string vertexShaderFileName, const std::string pixelShaderFileName);

};
