#define AORAY 3
struct Simpleraypayload
{

	float3 outcol;
};
struct AOraypayload
{

	float occlussionresult;
};
struct RTConstantSimple
{
//matrix is alias for float4x4
	matrix mat;
};
RaytracingAccelerationStructure basicas:register(t0);
ConstantBuffer<RTConstantSimple> rtconstmatrix:register(b1);
RWTexture2D<float4> gbuffertex[3]: register(u1);
[shader("closesthit")]
void closesthitmain(inout Simpleraypayload payload,in BuiltInTriangleIntersectionAttributes intersectionattribs)
{
	uint3 raydims=DispatchRaysDimensions();
	float3 raystart=WorldRayOrigin();
	float3 raydir=WorldRayDirection();
	float t=RayTCurrent();
	float3 rayend=raystart+(raydir*t);
	float4 worldhitpoint=float4(rayend,1.0f);
	//convert world space point(ray hit) to clip space by using viewprojection matrix
	float4 clippoint=mul(rtconstmatrix.mat,worldhitpoint);
	clippoint=clippoint/clippoint.w;
	//convert range for x & y from -1:1 to 0:1
	clippoint.xy=(clippoint.xy)+float2(1.0f,1.0f);
	clippoint.xy=clippoint.xy/2.0f;
	
	uint2 uv=raydims.xy*clippoint.xy;
	uv.y=raydims.y-uv.y;
	
	
	
	
	
	//fetch normal from gbuffer
	float4 normal=gbuffertex[1][uv];
	//fetch world pos
	float4 worldpos=gbuffertex[2][uv];
	float3 finalcol=float3(1.0f,1.0f,1.0f);
	//launch ao rays to find total occusion
	AOraypayload aopayload;
	//initialize no occlussion
	aopayload.occlussionresult=1.0f;
	
	{
		RayDesc ray;
		ray.Origin=worldpos.xyz;
		ray.Direction=normal.xyz;
		ray.TMin=0.001f;
ray.TMax=100.0f;
		TraceRay(basicas,

RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH
,0xFF,AORAY,1,0,ray,aopayload);
	}
	
	payload.outcol = aopayload.occlussionresult*finalcol;
	
}