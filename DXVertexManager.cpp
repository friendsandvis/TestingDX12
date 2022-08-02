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
		element2.AlignedByteOffset = 3 * sizeof(float);
		element2.InstanceDataStepRate = 0;
		element2.SemanticName = "VUV";
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
			element2.SemanticName = "NORMAL";
			element2.SemanticIndex = 0;
			outinputelementdescs.push_back(element2);
		}
	}
	break;
	default:
		break;
	}
}

unsigned DXVertexManager::GetVertexSize(VertexVersion vtype)
{
	switch (vtype)
	{
	case VERTEXVERSION0:
		return(5 * sizeof(float)); break;
	case VERTEXVERSION2:
		return(6 * sizeof(float)); break;
	default:
		return 0;
	}
}

void DXVertexManager::RetriveRawVertexData(vector<float>& outverticiesrawdata, VertexBase* avertex)
{
	switch (avertex->m_vertversion)
	{
	case VERTEXVERSION0:
	{
		VetexV0* vert = static_cast<VetexV0*>(avertex);
		outverticiesrawdata.push_back(vert->m_position.x);
		outverticiesrawdata.push_back(vert->m_position.y);
		outverticiesrawdata.push_back(vert->m_position.z);
		outverticiesrawdata.push_back(vert->m_uv.x);
		outverticiesrawdata.push_back(vert->m_uv.y);
		break;
	}
	case VERTEXVERSION2:
	{
		VetexV2* vert = static_cast<VetexV2*>(avertex);
		outverticiesrawdata.push_back(vert->m_position.x);
		outverticiesrawdata.push_back(vert->m_position.y);
		outverticiesrawdata.push_back(vert->m_position.z);
		outverticiesrawdata.push_back(vert->m_normal.x);
		outverticiesrawdata.push_back(vert->m_normal.y);
		outverticiesrawdata.push_back(vert->m_normal.z);
		break;
	}
	default:
		break;
	}

}