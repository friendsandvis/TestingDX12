
Texture2D<float4> textures[]: register(t1);
struct MaterialDataGPU
{
uint diffusetexidx;
uint normaltexidx;
uint roughnesstexidx;
//uint padding;
};
StructuredBuffer<MaterialDataGPU> mattable:register(t0,space1);
struct MatConstants
{
bool useCustomMaterial;
uint texidx;
};

struct MaterialConstants
{
	uint matidx;
};

ConstantBuffer<MaterialDataGPU> matconsts:register(b1,space0);
SamplerState simplesampler:register(s0);
ConstantBuffer<MaterialConstants> matgeneralconsts:register(b1,space1);
struct VSOut
{
    float4 pos : SV_Position;
	float3 normal: NORM;
	float4 pixelpos : PIXPOS;
	float2 uv:UV;
};
float4 main(VSOut psin) : SV_TARGET0
{
	float3 finallit = float3(0.0f,0.0f,0.0f);
	float2 uvnew=float2(psin.uv.x,1.0f-psin.uv.y);
	MaterialDataGPU mattmp=mattable[matgeneralconsts.matidx];
	//float4 diffusecol=textures[mattmp.diffusetexidx].Sample(simplesampler,uvnew);
	//float4 normalcol=textures[mattmp.normaltexidx].Sample(simplesampler,uvnew);
	//float4 roughnesscol=textures[mattmp.roughnesstexidx].Sample(simplesampler,uvnew);
	//float4 finalc=(diffusecol*500.0f)+(normalcol*50.0f)+(roughnesscol*0.5f);
	//return finalc;
		float4 texoutput=textures[mattmp.diffusetexidx].Sample(simplesampler,uvnew);
		float4 normaloutput = float4(psin.normal,1.0f);
		return texoutput;
	
}