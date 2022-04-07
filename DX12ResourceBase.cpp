#include"DX12ResourceBase.h"

D3D12_RESOURCE_BARRIER DX12ResourceBase::TransitionResState(D3D12_RESOURCE_STATES targetstate)
{
	assert(targetstate != m_currentresstate);
	D3D12_RESOURCE_BARRIER transitionbarrier = {};
	//return a default barrier struct if target state is same as current state
	if (targetstate != m_currentresstate)
	{
		transitionbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		transitionbarrier.Transition.StateBefore = m_currentresstate;
		transitionbarrier.Transition.StateAfter = targetstate;
		transitionbarrier.Transition.pResource = m_resource.Get();
		m_currentresstate = targetstate;
	}
	return transitionbarrier;
}