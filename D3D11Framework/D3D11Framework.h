#pragma once
//4506 에러를 그냥 무시
#pragma warning(disable : 4506)

#define NOMINMAX

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

//Window
#include <Windows.h>
#include <assert.h>

//STL
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <string>
#include <deque>
#include <queue>
#include <vector>
#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <variant>

//DirectX
#include <d3dcompiler.h>
#include <d3d11_4.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

typedef unsigned int uint;
typedef unsigned long ulong;

//Macro function
#define SAFE_RELEASE(p)         { if (p) { (p)->Release(); (p) = nullptr; } }
#define SAFE_DELETE(p)          { if (p) { delete (p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p)    { if (p) { delete[] (p); (p) = nullptr; } }

//전역변수
static const std::string NOT_ASSIGNED_STR = "N/A";
static const std::string MODEL_BIN_EXTENSION = ".model";
static const std::string MESH_BIN_EXTENSION = ".mesh";
static const std::string TEXTURE_BIN_EXTENSION = ".texture";
static const std::string MATERIAL_BIN_EXTENSION = ".material";

//Math
#include "Math/Math.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Color4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "Math/BoundBox.h"
#include "Math/Ray.h"
#include "Math/Plane.h"
#include "Math/Frustum.h"

//Smart Pointer
#include <memory>

//Utility
#include "Utility/GUID_Generator.h"
#include "Utility/Geometry_Generator.h"
#include "Utility/FileSystem.h"
#include "Utility/FileStream.h"
#include "Utility/Log/Log.h"

//Main System
#include "Core/Settings.h"
#include "Core/Context.h"
#include "Core/Engine.h"

//Event System
#include "Core/EventSystem/EventSystem.h"

//Subsystem
#include "Core/Subsystem/Timer.h"
#include "Core/Subsystem/Input.h"
#include "Core/Subsystem/Audio.h"
#include "Core/Subsystem/Graphics.h"
#include "Core/Subsystem/Thread.h"
#include "Core/Subsystem/Scripting.h"
#include "Core/Subsystem/ResourceManager.h"
#include "Core/Subsystem/SceneManager.h"
#include "Core/Subsystem/Renderer.h"

//Basic Data
#include "Core/D3D11Graphics/BasicData/D3D11_ConstantData.h"
#include "Core/D3D11Graphics/BasicData/Vertex.h"
#include "Core/D3D11Graphics/BasicData/Geometry.h"

//D3D11 Wrapper class
#include "Core/D3D11Graphics/D3D11_Object.h"
#include "Core/D3D11Graphics/IA/VertexBuffer.h"
#include "Core/D3D11Graphics/IA/IndexBuffer.h"
#include "Core/D3D11Graphics/IA/InputLayout.h"
#include "Core/D3D11Graphics/Shader/VertexShader.h"
#include "Core/D3D11Graphics/Shader/PixelShader.h"
#include "Core/D3D11Graphics/Shader/ConstantBuffer.h"
#include "Core/D3D11Graphics/Shader/SamplerState.h"
#include "Core/D3D11Graphics/RS/RasterizerState.h"
#include "Core/D3D11Graphics/OM/DepthStencilState.h"
#include "Core/D3D11Graphics/OM/BlendState.h"

//Shader
#include "Shader/Shader.h"

//Resource
#include "Resource/AudioClip.h"
#include "Resource/Texture2D.h"
#include "Resource/Mesh.h"
#include "Resource/Material.h"
#include "Resource/Skeleton.h"
#include "Resource/Animation.h"
#include "Resource/Model.h"
