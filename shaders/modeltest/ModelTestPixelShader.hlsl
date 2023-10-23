
Texture2D<float4> textures[]: register(t0);
struct MatConstants
{
uint texidx;
};
struct MaterialDataGPU
{
uint diffusetexidx;
uint normaltexidx;
uint roughnesstexidx;
//uint padding;
};
ConstantBuffer<MaterialDataGPU> matconsts:register(b1,space0);
SamplerState simplesampler:register(s0);
struct GeneralConstants
{
uint supportmaterialtextures;
};
ConstantBuffer<GeneralConstants> generalconsts:register(b1,space1);
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
		float4 texoutput=textures[matconsts.roughnesstexidx].Sample(simplesampler,uvnew);
		float4 normaloutput = float4(psin.normal,1.0f);
		return lerp(normaloutput,texoutput,(float)generalconsts.supportmaterialtextures);
	
}