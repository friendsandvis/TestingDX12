
struct Simpleraypayload
{
	float3 outcol;
};
struct Vertex
{
	float3 pos;
	float2 uv;
};
StructuredBuffer<Vertex> triangleverticies:register(t1);
[shader("closesthit")]
void closesthitmain(inout Simpleraypayload payload,in BuiltInTriangleIntersectionAttributes intersectionattribs)
{
	uint vertId = 3 * PrimitiveIndex();
	float2 a0=triangleverticies[vertId].uv;
	float2 a1=triangleverticies[vertId+1].uv;
	float2 a2=triangleverticies[vertId+2].uv;
	float barycentricz=1.0f-(intersectionattribs.barycentrics.x+intersectionattribs.barycentrics.y);
	//float2 uvres=(triangleverticies[vertId].uv*intersectionattribs.barycentrics.x)+(triangleverticies[vertId+1].uv*intersectionattribs.barycentrics.y)+(triangleverticies[vertId+2].uv*barycentricz);
	float2 uvres=a0 + intersectionattribs.barycentrics.x *(a1-a0) + intersectionattribs.barycentrics.y *(a2-a0);
	//payload.outcol=float3(1.0f,0.0f,0.0f);
	payload.outcol=float3(uvres,0.0f);
}