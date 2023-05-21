/*
Notes:
ray origin is calculated based on the dispatch index and a center(this center value is  what test geometry's verticies  refrence to as center)
*/
//ray types defines
#define FETCHGBUFFERRAY 0
#define SIMPLEHITRAY 1
//ao calc ray type triggers accumulation & calculation of ao 
#define AOCALCRAY 2
#define NUMRAYTTYPES 3
#define NUMGBUFFERTEXTURES 3
struct RTConstants
{
//matrix is alias for float4x4
	matrix mat1;
	matrix mat2;
};
struct AOConstants
{
	uint frameidx;
	float aoradius;
};

struct SimpleAOpayload
{
	float aoresult;//0 means hit & 1 means miss
};
RWTexture2D<float4> outtex: register(u0);
RWTexture2D<float4> gbuffertex[NUMGBUFFERTEXTURES]: register(u1);
RaytracingAccelerationStructure basicas:register(t0);
ConstantBuffer<AOConstants> aoconstants:register(b0);

//util functions for ao ray generation(taken from "http://cwyman.org/code/dxrTutors/tutors/Tutor5/tutorial05")
uint InitRand(uint val0, uint val1, uint backoff = 16)
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
float3 GetCosHemisphereSample(inout uint randSeed, float3 hitNorm)
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
[shader("raygeneration")]
void rgsmain()
{
uint3 rayidx=DispatchRaysIndex();
	uint3 raydims=DispatchRaysDimensions();
	uint2 outindex=uint2(rayidx.x,raydims.y-rayidx.y);

//fetch normal from gbuffer
	float4 worldnormal=gbuffertex[1][outindex];
	//fetch world pos
	float4 worldpos=gbuffertex[2][outindex];
	//fetch albedo
	float4 albedo=gbuffertex[0][outindex];

float2 center=float2(-0.5f,-0.5f);
//ray.Origin=float3(0.0f,0.0,1.0f);
	float2 clippointxy=rayidx.xy/float2(raydims.xy);
	clippointxy=(clippointxy*2.0f)-float2(1.0f,1.0f);


	//flipping the output.
	
	//calculate uv from rayindex for tex sampling(here asuming the dispatch ray dimension ==sampled texture size)
	float2 uv=rayidx.xy/float2(raydims.xy);
	//outtex[outindex]=float4(float3(uv,0.0f),1.0f);
	//outtex[outindex]=float4(payload.outcol,1.0f);
	//output gbuffer img data but assumed dispatch size is same as gbuffer tex size always.
	
	if(albedo.w==1.0f)//non-background( geometry in gbuffer mark white(ao calc)
	{
	uint randseed= InitRand((rayidx.x+ rayidx.y*raydims.x),aoconstants.frameidx);
	float3 dir = GetCosHemisphereSample(randseed,normalize(worldnormal.xyz));
	RayDesc ray;
	ray.Origin=worldpos.xyz;
	ray.Direction=normalize(dir);
	ray.TMin=0.001f;
	ray.TMax=aoconstants.aoradius;
	SimpleAOpayload payload;
payload.aoresult=1.0f;//initialize as hit.
		TraceRay(basicas,RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH|RAY_FLAG_SKIP_CLOSEST_HIT_SHADER,0xFF,AOCALCRAY,NUMRAYTTYPES,1,ray,payload);
		//outtex[outindex]=float4(normalize(worldnormal.xyz),1.0f);
		outtex[outindex]=float4(payload.aoresult,payload.aoresult,payload.aoresult,1.0f);
	}
	else//background(no geometry in gbuffer mark black(no ao calc)
	{
		outtex[outindex]=float4(0.0f,0.0f,0.0f,1.0f);
	}
	//outtex[outindex]=float4(payload.aoresult,payload.aoresult,payload.aoresult,1.0f);
}