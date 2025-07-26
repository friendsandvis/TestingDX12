
Texture2D<float4> textures[]: register(t1);
struct MaterialDataGPU
{
uint diffusetexidx;
uint normaltexidx;
uint roughnesstexidx;
//uint padding;
};
struct CustomMaterial
{
	float useCustomMaterial; 
	float ambientfactor;
	float specularValue;
	float padding0;
	float4 viewPos;
	float4 albedo;
};
struct TestLight
{
	float3 lightCol;
	float data1;
	float3 lightPos;
	float data2;
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
ConstantBuffer<CustomMaterial> customMatconsts:register(b2,space0);
ConstantBuffer<TestLight> testLightconsts:register(b3,space0);
SamplerState simplesampler:register(s0);
/*struct GeneralConstants
{
uint supportmaterialtextures;
};*/
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
	bool useCustomMaterial = (customMatconsts.useCustomMaterial == 1.0f);
	if(useCustomMaterial)
	{
		//lighting calc
		float3 viewPos = customMatconsts.viewPos.xyz;
		float3 ambientlit = customMatconsts.albedo.xyz * customMatconsts.ambientfactor;
		float3 lightDir = normalize(testLightconsts.lightPos -psin.pixelpos.xyz);
		float3 viewDir = normalize(viewPos -psin.pixelpos.xyz);
		float3 lightreflectDir = normalize(reflect(lightDir,psin.normal));
		const float specFactintensity =1.0f;
		float specfact = specFactintensity * pow(max(dot(viewDir,lightreflectDir),0.0f),customMatconsts.specularValue);
		float3 speclit = specfact * testLightconsts.lightCol;
		float diffusefact = dot(lightDir,psin.normal);
		diffusefact = max(diffusefact,0.0f);
		float3 diffuselit = testLightconsts.lightCol * diffusefact;
		float3 finallit = ambientlit + (diffuselit * customMatconsts.albedo.xyz) + (speclit* customMatconsts.albedo.xyz);
		float4 finalOut = float4(finallit,1.0f);
		return finalOut;
	}
	
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