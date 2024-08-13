#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

// OS includes
#include <windows.h>
#include <windowsx.h>
#include <wincodec.h>

// my imports
#include <d3d12.h>
#include "directx/d3dx12.h"
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include "DirectXMath.h"

#include "tiny_obj_loader.h"

#include <string>
#include <wrl.h>
#include <shellapi.h>
#include <iostream>
#include <memory>
#include <filesystem>