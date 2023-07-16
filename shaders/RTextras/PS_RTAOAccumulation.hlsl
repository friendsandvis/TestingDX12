
Texture2D<float4> imagelastframe:register(t0);
Texture2D<float4> imagecurrentframe:register(t1);
SamplerState simplesampler:register(s0);
struct AOAccumConstants
{
	uint framecount;
};
ConstantBuffer<AOAccumConstants> aoaccumconstants:register(b0);
struct Vout
{
	float4 opos:SV_Position;
	float4 vcol : COLOUR0;
	float2 uv:UV;
};

float4 main(Vout psin) : SV_TARGET0
{
	float4  currentframedata = imagecurrentframe.Sample(simplesampler,psin.uv);
	float4  lastframedata = imagelastframe.Sample(simplesampler,psin.uv);
	float4 result = (aoaccumconstants.framecount*lastframedata + currentframedata)/(aoaccumconstants.framecount+1);
	//return float4(psin.uv,0.0f,1.0f);
	return result;
	//return currentframedata;
	return float4(1.0f,0.0f,0.0f,1.0f);
}