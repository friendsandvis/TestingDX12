
Texture2D<float4> textures[]: register(t1);
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
struct MaterialConstants
{
uint diffusetexidx;
};
StructuredBuffer<MaterialDataGPU> mattable:register(t0,space1);
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
	
	
	
	float2 uvnew=float2(psin.uv.x,1.0f-psin.uv.y);
	/*MaterialDataGPU mattmp=mattable[0];
	float4 diffusecol=textures[mattmp.diffusetexidx].Sample(simplesampler,uvnew);
	float4 normalcol=textures[mattmp.normaltexidx].Sample(simplesampler,uvnew);
	float4 roughnesscol=textures[mattmp.roughnesstexidx].Sample(simplesampler,uvnew);
	float4 finalc=(diffusecol*500.0f)+(normalcol*50.0f)+(roughnesscol*0.5f);
	return finalc;*/
		float4 texoutput=textures[matconsts.diffusetexidx].Sample(simplesampler,uvnew);
		float4 normaloutput = float4(psin.normal,1.0f);
		return texoutput;
	
}