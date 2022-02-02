#include"DX12Resource.h"

D3D12_RESOURCE_BARRIER DX12Resource::TransitionResState(D3D12_RESOURCE_STATES targetstate)
{
	D3D12_RESOURCE_BARRIER transitionbarrier = {};
	transitionbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	transitionbarrier.Transition.StateBefore = m_currentresstate;
	transitionbarrier.Transition.StateAfter = targetstate;
	transitionbarrier.Transition.pResource = m_resource.Get();
	m_currentresstate = targetstate;

	return transitionbarrier;
}
