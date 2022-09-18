/*
Notes:
ray origin is calculated based on the dispatch index and a center(this center value is  what test geometry's verticies  refrence to as center)
*/
struct RTConstants
{
//matrix is alias for float4x4
	matrix mat;
};

struct Simpleraypayload
{
	float3 outcol;
};
RWTexture2D<float4> outtex: register(u0);
RaytracingAccelerationStructure basicas:register(t0);
ConstantBuffer<RTConstants> rtconstsprojection:register(b0);
ConstantBuffer<RTConstants> rtconstsview:register(b1);
[shader("raygeneration")]
void rgsmain()
{
Simpleraypayload payload;
payload.outcol=float3(0.0f,0.0f,0.0f);
RayDesc ray;
float2 center=float2(-0.5f,-0.5f);
//ray.Origin=float3(0.0f,0.0,1.0f);
ray.TMin=0.001f;
ray.TMax=200.0f;
ray.Direction=float3(0.0f,0.0f,-1.0f);
	uint3 rayidx=DispatchRaysIndex();
	uint3 raydims=DispatchRaysDimensions();
	float2 clippointxy=rayidx.xy/float2(raydims.xy);
	clippointxy=(clippointxy*2.0f)-float2(1.0f,1.0f);
	float4 clippointnear=float4(clippointxy,0.0f,1.0f);
	clippointnear=mul(rtconstsprojection.mat,clippointnear);
	float4 clippointfar=float4(clippointxy,1.0f,1.0f);
	clippointfar=mul(rtconstsprojection.mat,clippointfar);
	ray.Origin=clippointnear.xyz;
	ray.Direction=(clippointfar.xyz-clippointnear.xyz);
	ray.TMax=length(ray.Direction);
	ray.Direction=normalize(ray.Direction);
	TraceRay(basicas,RAY_FLAG_NONE,0xFF,0,0,0,ray,payload);
	//flipping the output.
	uint2 outindex=uint2(rayidx.x,raydims.y-rayidx.y);
	
	outtex[outindex]=float4(payload.outcol,1.0f);
}