
struct SimpleAOpayload
{
	float aoresult;//0 means hit & 1 means miss
};
/*struct BuiltInTriangleIntersectionAttributes
{
    float2 barycentrics;
};*/
[shader("anyhit")]
void anyhitmain(inout SimpleAOpayload payload, BuiltInTriangleIntersectionAttributes attribs)
{
	payload.aoresult=0.0f;
}