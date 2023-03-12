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
//util functions for ao ray generation(taken from "http://cwyman.org/code/dxrTutors/tutors/Tutor5/tutorial05")
uint initRand(uint val0, uint val1, uint backoff = 16)
{
	uint v0 = val0, v1 = val1, s0 = 0;

	[unroll]
	for (uint n = 0; n < backoff; n++)
	{
		s0 += 0x9e3779b9;
		v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
		v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
	}
	return v0;
}
float3 getPerpendicularVector(float3 u)
{
	float3 a = abs(u);
	uint xm = ((a.x - a.y)<0 && (a.x - a.z)<0) ? 1 : 0;
	uint ym = (a.y - a.z)<0 ? (1 ^ xm) : 0;
	uint zm = 1 ^ (xm | ym);
	return cross(u, float3(xm, ym, zm));
}
// Takes our seed, updates it, and returns a pseudorandom float in [0..1]
float nextRand(inout uint s)
{
	s = (1664525u * s + 1013904223u);
	return float(s & 0x00FFFFFF) / float(0x01000000);
}
float3 getCosHemisphereSample(inout uint randSeed, float3 hitNorm)
{
	// Get 2 random numbers to select our sample with
	float2 randVal = float2(nextRand(randSeed), nextRand(randSeed));

	// Cosine weighted hemisphere sample from RNG
	float3 bitangent = getPerpendicularVector(hitNorm);
	float3 tangent = cross(bitangent, hitNorm);
	float r = sqrt(randVal.x);
	float phi = 2.0f * 3.14159265f * randVal.y;

	// Get our cosine-weighted hemisphere lobe sample direction
	return tangent * (r * cos(phi).x) + bitangent * (r * sin(phi)) + hitNorm.xyz * sqrt(1 - randVal.x);
}
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