struct BuiltInTriangleIntersectionAttributes
{
    float2 barycentrics;
};
struct BasicPayLoad
{
	float4 colour;
};

["closesthit"]
void closesthitmain(inout BasicPayLoad payload,in BuiltInTriangleIntersectionAttributes intersectionattribs)
{
	payload.colour=float4(0.0f,0.0f,1.0f,1.0f);
}