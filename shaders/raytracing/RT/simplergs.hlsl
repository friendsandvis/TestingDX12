
RWTexture2D<float4> outtex: register(u0);
RaytracingAccelerationStructure basicas:register(t0);
[shader("raygeneration")]
void rgsmain()
{
	uint3 rayidx=DispatchRaysIndex();
	outtex[rayidx.xy]=float4(1.0f,0.0f,0.0f,1.0f);
}