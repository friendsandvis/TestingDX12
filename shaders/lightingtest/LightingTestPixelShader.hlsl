//#define TESTLIGHTTYPE_POINT
#define TESTLIGHTTYPE_DIRECTION
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
	float useMaterialTextures;
	float specularValue;
	uint  lightingMode;
	float4 viewPos;
	float4 ambient;
	float4 diffuse;
	float4 specular;
};
struct LocalLight
{
	float4 lightCol;
	float4 lightPos;
};
#ifdef TESTLIGHTTYPE_POINT
struct TestLight
{
	float3 lightCol;
	float data1;
	float3 lightPos;
	float data2;
};
#elif defined(TESTLIGHTTYPE_DIRECTION)
	struct LightingPeroperties
{
	float3 lightCol;
	float numlocallights;
	float3 lightDir;
	float usedirectionallight;
};
#endif //TESTLIGHTTYPE_POINT
StructuredBuffer<MaterialDataGPU> mattable:register(t0,space1);
StructuredBuffer<LocalLight> locallights:register(t0,space0);
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
ConstantBuffer<LightingPeroperties> testLightconsts:register(b3,space0);
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
	bool useMaterialTextures = (customMatconsts.useMaterialTextures == 1.0f);
		float2 uvnew=float2(psin.uv.x,1.0f-psin.uv.y);
	MaterialDataGPU mattmp=mattable[matgeneralconsts.matidx];
	float3 finallit = float3(0.0f,0.0f,0.0f);
	if(useCustomMaterial)
	{
		float4 diffusecol = float4(1.0f,1.0f,1.0f,1.0f);
		float4 roughnesscol = float4(1.0f,1.0f,1.0f,1.0f);
		if(useMaterialTextures)
		{
			diffusecol = textures[mattmp.diffusetexidx].Sample(simplesampler,uvnew);
			roughnesscol = textures[mattmp.roughnesstexidx].Sample(simplesampler,uvnew);
		}
		//lighting calc
		bool usedirectionallight = (testLightconsts.usedirectionallight == 1.0f);
		float numlocallight = testLightconsts.numlocallights;
		LocalLight locallight = locallights[0];
		float3 viewPos = customMatconsts.viewPos.xyz;
		float3 ambientlit = customMatconsts.ambient.xyz * diffusecol.xyz;
		//for lighting lightdirection is in point of view of pixel aka direction towards the light
		float3 lightDir = normalize(locallight.lightPos.xyz -psin.pixelpos.xyz);
		float3 lightCol = locallight.lightCol.xyz;
		
		if(usedirectionallight)
		{
			lightDir = normalize(testLightconsts.lightDir.xyz);
			lightCol = testLightconsts.lightCol;
		}

		float3 viewDir = normalize(viewPos -psin.pixelpos.xyz);
		viewDir = -viewDir;
		//reflight lights actual direction for reflected direction vector calc
		float3 lightreflectDir = normalize(reflect(lightDir,psin.normal));
		const float specFactintensity =50.0f;
		float specfact = specFactintensity * pow(max(dot(viewDir,lightreflectDir),0.0f),customMatconsts.specularValue);
		float3 speclit = specfact * lightCol * customMatconsts.specular.xyz * roughnesscol.xyz;
		float diffusefact = dot(psin.normal,lightDir);
		diffusefact = max(diffusefact,0.0f);
		
		float3 diffuselit = lightCol * diffusefact*customMatconsts.diffuse.xyz* diffusecol.xyz;
		
		if(customMatconsts.lightingMode == 0)
		{
			finallit = ambientlit + diffuselit  + speclit;
		}
		else if(customMatconsts.lightingMode == 1)
		{
			finallit = customMatconsts.ambient.xyz;
		}
		float4 finalOut = float4(finallit,1.0f);
		return finalOut;
	}

	//float4 diffusecol=textures[mattmp.diffusetexidx].Sample(simplesampler,uvnew);
	//float4 normalcol=textures[mattmp.normaltexidx].Sample(simplesampler,uvnew);
	//float4 roughnesscol=textures[mattmp.roughnesstexidx].Sample(simplesampler,uvnew);
	//float4 finalc=(diffusecol*500.0f)+(normalcol*50.0f)+(roughnesscol*0.5f);
	//return finalc;
		float4 texoutput=textures[mattmp.diffusetexidx].Sample(simplesampler,uvnew);
		float4 normaloutput = float4(psin.normal,1.0f);
		return texoutput;
	
}