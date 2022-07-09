struct Simpleraypayload
{
	float3 outcol;
};
RWTexture2D<float4> outtex: register(u0);
RaytracingAccelerationStructure basicas:register(t0);
[shader("raygeneration")]
void rgsmain()
{
Simpleraypayload payload;
payload.outcol=float3(0.0f,0.0f,0.0f);
RayDesc ray;
ray.Origin=float3(0.0f,0.0f,0.0f);
ray.TMin=0.0f;
ray.TMax=0.0f;
ray.Direction=float3(0.0f,0.0f,-1.0f);
	uint3 rayidx=DispatchRaysIndex();
	TraceRay(basicas,RAY_FLAG_NONE,0xFF,0,0,0,ray,payload);
	outtex[rayidx.xy]=float4(payload.outcol,1.0f);
}