
struct Simpleraypayload
{
	float3 outcol;
};

[shader("closesthit")]
void closesthitmain(inout Simpleraypayload payload,in BuiltInTriangleIntersectionAttributes intersectionattribs)
{
	payload.outcol=float3(1.0f,0.0f,0.0f);
}