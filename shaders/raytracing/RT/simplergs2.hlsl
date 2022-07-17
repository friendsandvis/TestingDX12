/*
Notes:
ray origin is calculated based on the dispatch index and a center(this center value is  what test geometry's verticies  refrence to as center)
*/
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
float2 center=float2(-0.5f,-0.5f);
//ray.Origin=float3(0.0f,0.0,1.0f);
ray.TMin=0.001f;
ray.TMax=10.0f;
ray.Direction=float3(0.0f,0.0f,-1.0f);
	uint3 rayidx=DispatchRaysIndex();
	uint3 raydims=DispatchRaysDimensions();
	float2 offset=rayidx.xy/float2(raydims.xy);
	//offset.y*=-1.0f;
	//offset-=float2(0.5f,0.5f);
	//offset*=2.0f;
	ray.Origin=float3(center+offset,1.0f);
	TraceRay(basicas,RAY_FLAG_NONE,0xFF,0,0,0,ray,payload);
	//flipping the output.
	uint2 outindex=uint2(rayidx.x,raydims.y-rayidx.y);
	
	outtex[outindex]=float4(payload.outcol,1.0f);
}