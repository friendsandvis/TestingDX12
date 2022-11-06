
struct Simpleraypayload
{

	float3 outcol;
};
struct RTConstantSimple
{
//matrix is alias for float4x4
	matrix mat;
};
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
	float4 clippoint=mul(rtconstmatrix.mat,worldhitpoint);
	clippoint=clippoint/clippoint.w;
	//payload.outcol=float3(1.0f,0.0f,0.0f);
	uint2 uv=raydims.xy*clippoint.xy;
	
	float4 col=gbuffertex[0][uv];
	//payload.outcol=col.xyz;
	clippoint.xy=(clippoint.xy)+float2(1.0f,1.0f);
	clippoint.xy=clippoint.xy/2.0f;
	uv=raydims.xy*clippoint.xy;
	uv.y=raydims.y-uv.y;
	col=gbuffertex[2][uv];
	payload.outcol=col.xyz;
	
}