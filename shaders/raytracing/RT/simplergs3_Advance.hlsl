/*
Notes:
ray origin is calculated based on the dispatch index and a center(this center value is  what test geometry's verticies  refrence to as center)
*/
#define NUMGBUFFERTEXTURES 3
struct RTConstants
{
//matrix is alias for float4x4
	matrix mat1;
	matrix mat2;
};

struct Simpleraypayload
{
	float3 outcol;
};
RWTexture2D<float4> outtex: register(u0);
RWTexture2D<float4> gbuffertex[NUMGBUFFERTEXTURES]: register(u1);
RaytracingAccelerationStructure basicas:register(t0);
ConstantBuffer<RTConstants> rtconstsinvmatricies:register(b0);//mat1 is invprojection & mat2 is invview

[shader("raygeneration")]
void rgsmain()
{
Simpleraypayload payload;
payload.outcol=float3(0.0f,0.0f,0.0f);
RayDesc ray;
float2 center=float2(-0.5f,-0.5f);
//ray.Origin=float3(0.0f,0.0,1.0f);
ray.TMin=0.001f;
ray.TMax=100.0f;
ray.Direction=float3(0.0f,0.0f,-1.0f);
	uint3 rayidx=DispatchRaysIndex();
	uint3 raydims=DispatchRaysDimensions();
	float2 clippointxy=rayidx.xy/float2(raydims.xy);
	clippointxy=(clippointxy*2.0f)-float2(1.0f,1.0f);
	float4 clippointnear=float4(clippointxy,0.0f,1.0f);
	clippointnear=mul(rtconstsinvmatricies.mat1,clippointnear);
	clippointnear=clippointnear/clippointnear.w;
	float4 clippointfar=float4(clippointxy,1.0f,1.0f);
	clippointfar=mul(rtconstsinvmatricies.mat1,clippointfar);
	clippointfar=clippointfar/clippointfar.w;
	ray.Origin=clippointnear.xyz;
	float4 originmod=float4(clippointnear.xyz,1.0f);
	originmod=mul(rtconstsinvmatricies.mat2,originmod);
	ray.Origin=originmod.xyz;
	
	ray.Direction=(clippointfar.xyz-clippointnear.xyz);
	
	ray.TMax=length(ray.Direction);
	ray.Direction=normalize(ray.Direction);
	float4 dirmod=float4(ray.Direction.xyz,0.0f);
	
	dirmod=mul(rtconstsinvmatricies.mat2,dirmod);
	ray.Direction=dirmod.xyz;
	TraceRay(basicas,RAY_FLAG_NONE,0xFF,0,0,0,ray,payload);
	//flipping the output.
	uint2 outindex=uint2(rayidx.x,raydims.y-rayidx.y);
	//calculate uv from rayindex for tex sampling(here asuming the dispatch ray dimension ==sampled texture size)
	float2 uv=rayidx.xy/float2(raydims.xy);
	//outtex[outindex]=float4(float3(uv,0.0f),1.0f);
	outtex[outindex]=float4(payload.outcol,1.0f);
	//output gbuffer img data but assumed dispatch size is same as gbuffer tex size always.
	//outtex[outindex]=(gbuffertex[2])[outindex];
}