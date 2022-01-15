#pragma once

#include<wrl/client.h>
#include<assert.h>

using namespace Microsoft::WRL;

#define DXASSERT(EXP)	{HRESULT res = EXP; assert(res == S_OK);}



