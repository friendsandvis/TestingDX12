#include"DXVertexManager.h"

void DXVertexManager::BuildDefaultInputelementdesc(std::vector< D3D12_INPUT_ELEMENT_DESC>& outinputelementdescs, VertexVersion vtype)
{
	switch (vtype)
	{
	case VertexVersion::VERTEXVERSION0:
	{
		{
		D3D12_INPUT_ELEMENT_DESC element1 = {};
		element1.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		element1.InputSlot = 0;
		element1.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		element1.AlignedByteOffset = 0;
		element1.InstanceDataStepRate = 0;
		element1.SemanticName = "POS";
		element1.SemanticIndex = 0;
		outinputelementdescs.push_back(element1);
	}
		{
			D3D12_INPUT_ELEMENT_DESC element2 = {};
			element2.Format = DXGI_FORMAT_R32G32_FLOAT;
			element2.InputSlot = 0;
			element2.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			element2.AlignedByteOffset = 3* sizeof(float);
			element2.InstanceDataStepRate = 0;
			element2.SemanticName = "UV";
			element2.SemanticIndex = 0;
			outinputelementdescs.push_back(element2);
		}
	}
	break;
	case VertexVersion::VERTEXVERSION1:
	{
		{
			D3D12_INPUT_ELEMENT_DESC element1 = {};
			element1.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			element1.InputSlot = 0;
			element1.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			element1.AlignedByteOffset = 0;
			element1.InstanceDataStepRate = 0;
			element1.SemanticName = "POS";
			element1.SemanticIndex = 0;
			outinputelementdescs.push_back(element1);
		}
		{
			D3D12_INPUT_ELEMENT_DESC element2 = {};
			element2.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			element2.InputSlot = 0;
			element2.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			element2.AlignedByteOffset = 3 * sizeof(float);
			element2.InstanceDataStepRate = 0;
			element2.SemanticName = "POSCOL";
			element2.SemanticIndex = 0;
			outinputelementdescs.push_back(element2);
		}
	}
	break;
	case VertexVersion::VERTEXVERSION2:
	{
		{
			D3D12_INPUT_ELEMENT_DESC element1 = {};
			element1.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			element1.InputSlot = 0;
			element1.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			element1.AlignedByteOffset = 0;
			element1.InstanceDataStepRate = 0;
			element1.SemanticName = "POS";
			element1.SemanticIndex = 0;
			outinputelementdescs.push_back(element1);
		}
		{
			D3D12_INPUT_ELEMENT_DESC element2 = {};
			element2.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			element2.InputSlot = 0;
			element2.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			element2.AlignedByteOffset = 3 * sizeof(float);
			element2.InstanceDataStepRate = 0;
			element2.SemanticName = "POSCOL";
			element2.SemanticIndex = 0;
			outinputelementdescs.push_back(element2);
		}
		{
			D3D12_INPUT_ELEMENT_DESC element3 = {};
			element3.Format = DXGI_FORMAT_R32G32_FLOAT;
			element3.InputSlot = 0;
			element3.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			element3.AlignedByteOffset = 6 * sizeof(float);
			element3.InstanceDataStepRate = 0;
			element3.SemanticName = "UV";
			element3.SemanticIndex = 0;
			outinputelementdescs.push_back(element3);
		}
	}
	break;
	default:
		break;
	}
}