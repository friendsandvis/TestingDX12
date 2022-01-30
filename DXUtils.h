#pragma once

#include<d3d12.h>
#include<dxgi1_3.h>
#include<wrl/client.h>
#include<assert.h>
#include<vector>
#include<d3dx12.h>

#define BACKBUFFERCOUNT 3

//used for selecting hardware adaptor
//currently set to nvidia
#define HARDWAREVENDORID 4318

using namespace Microsoft::WRL;
using namespace std;
#define DXASSERT(EXP)	{HRESULT res = EXP; assert(res == S_OK);}



