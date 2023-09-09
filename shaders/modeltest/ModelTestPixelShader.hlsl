
Texture2D<float4> textures[]: register(t0);
struct MatConstants
{
uint texidx;
};
ConstantBuffer<MatConstants> matconsts:register(b1);
SamplerState simplesampler:register(s0);
struct VSOut
{
    float4 pos : SV_Position;
	float3 normal: NORM;
	float2 uv:UV;
};

float4 main(VSOut psin) : SV_TARGET0
{
	//return float4(psin.normal,1.0f);
	float2 uvnew=float2(psin.uv.x,1.0f-psin.uv.y);
	return textures[matconsts.texidx].Sample(simplesampler,uvnew);
	
}