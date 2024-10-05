#pragma once

#include<d3d12.h>
#include<dxgi1_3.h>
#include<wrl/client.h>
#include<assert.h>
#include<vector>
#include<d3dx12.h>
#include<DirectXMath.h>

#define BACKBUFFERCOUNT 3
#define MAXACCESSABLERESOURCEVIEWCOUNT 5
#define USEIMGUI

//used for selecting hardware adaptor
//currently set to nvidia
#define HARDWAREVENDORID 4318

using namespace Microsoft::WRL;
using namespace std;
#define DXASSERT(EXP)	{HRESULT res = EXP; assert(res == S_OK);}

//class to hold frequently used and ease of life functions
class DXUtils
{
public:
	DXUtils();
	~DXUtils();
	static bool IsBarrierSafeToExecute(const D3D12_RESOURCE_BARRIER barrier);
	static DirectX::XMVECTOR Float4tovector(DirectX::XMFLOAT4 value);
	//rotationangle in degree
	static DirectX::XMMATRIX GetTransformationMatrix(float uniformscale, DirectX::XMVECTOR rotationaxis, float rotationangle, DirectX::XMVECTOR translate=DirectX::XMVectorSet(0.0f,0.0f,0.0f,0.0f));
	//rotationangle in degree
	static DirectX::XMMATRIX GetTransformationMatrix(DirectX::XMVECTOR scale, DirectX::XMVECTOR rotationaxis, float rotationangle, DirectX::XMVECTOR translate = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	static void GetIdxSuffix(unsigned idx,wstring& idxsuffix);
	static void GetIdxSuffix(unsigned idx, string& idxsuffix);
private:

};


