
struct AOraypayload
{

	float occlussionresult;
};
struct RTConstantSimple
{
//matrix is alias for float4x4
	matrix mat;
};
ConstantBuffer<RTConstantSimple> rtconstmatrix:register(b1);
RWTexture2D<float4> gbuffertex[3]: register(u1);
[shader("closesthit")]
void closesthitmain(inout AOraypayload payload,in BuiltInTriangleIntersectionAttributes intersectionattribs)
{
	//mark occluded(occlusion result 0.0f)
	payload.occlussionresult = 0.0f;
	
}