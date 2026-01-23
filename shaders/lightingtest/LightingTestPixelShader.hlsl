
Texture2D<float4> textures[]: register(t1,space0);
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
struct PointLight
{
	float4 lightCol;
	float4 lightPos;
	float4 attenuationConsts;//x=constant,y=linear,z=quadratic w is unused
};
struct SpotLight
{
	float4 lightCol;
	float4 lightPos;
	float4 lightDir;//xyz direction and w as cutoff
};
struct LocalLight
{
	float4 lightCol;
	float4 lightPos;//w tells type of light
	float4 lightExtras;//can hold attenuation for pointlight or direction with cutoff for spot
};
PointLight GetPointLight(const LocalLight locallight)
{
	PointLight pointLight;
	pointLight.lightCol = locallight.lightCol;
	pointLight.lightPos = locallight.lightPos;
	pointLight.attenuationConsts = locallight.lightExtras;
	return pointLight;
}

struct GeneralData
{
	float3 lightCol;
	float numlocallights;
	float3 lightDir;
	float usedirectionallight;
	float useInstanceData;
	float padding1;
	float padding2;
	
};
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
ConstantBuffer<GeneralData> testLightconsts:register(b3,space0);
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
struct LightingResult
{
	float3 diffuseRes;
	float3 specRes;
	float attenuationFact;
};
struct LightingInputsCommon
{
	float3 pixelPos;
	float3 normal;
	float specularValue;
	float3 viewPos;
	float specFactintensity;
	
	
};
LightingResult GetDirectionalLightRes(float3 lightDir,float3 lightCol,LightingInputsCommon lightingInputsCommon)
{
	//for lighting lightdirection is in point of view of pixel aka direction towards the light
	LightingResult resOut;
	float3 viewDir = normalize(lightingInputsCommon.viewPos -lightingInputsCommon.pixelPos);
		viewDir = -viewDir;
		//reflight lights actual direction for reflected direction vector calc
		float3 lightreflectDir = normalize(reflect(lightDir,lightingInputsCommon.normal));
		float specfact = lightingInputsCommon.specFactintensity * pow(max(dot(viewDir,lightreflectDir),0.0f),lightingInputsCommon.specularValue);
		float diffusefact = dot(lightingInputsCommon.normal,lightDir);
		resOut.diffuseRes = diffusefact * lightCol;
		resOut.specRes = specfact * lightCol;
	return resOut;
}
LightingResult GetPointLightRes(PointLight locallight,LightingInputsCommon lightingInputsCommon)
{
	LightingResult resOut;
	float3 lightCol = locallight.lightCol.xyz;
	//for lighting lightdirection is in point of view of pixel aka direction towards the light
	float3 lightDir = normalize(locallight.lightPos.xyz -lightingInputsCommon.pixelPos);
	float3 viewDir = normalize(lightingInputsCommon.viewPos -lightingInputsCommon.pixelPos);
		viewDir = -viewDir;
		//reflight lights actual direction for reflected direction vector calc
		float3 lightreflectDir = normalize(reflect(lightDir,lightingInputsCommon.normal));
		float specfact = lightingInputsCommon.specFactintensity * pow(max(dot(viewDir,lightreflectDir),0.0f),lightingInputsCommon.specularValue);
		//attenuation calc
		float dL = length(locallight.lightPos.xyz -lightingInputsCommon.pixelPos);
		resOut.attenuationFact = 1.0f /(locallight.attenuationConsts.x + locallight.attenuationConsts.y * dL + locallight.attenuationConsts.z * dL * dL);
		float diffusefact = dot(lightingInputsCommon.normal,lightDir);
		resOut.diffuseRes = diffusefact * lightCol * resOut.attenuationFact;
		resOut.specRes = specfact * lightCol * resOut.attenuationFact;
	
	return resOut;
}
float4 main(VSOut psin) : SV_TARGET0
{

	bool useCustomMaterial = (customMatconsts.useCustomMaterial == 1.0f);
	bool useMaterialTextures = (customMatconsts.useMaterialTextures == 1.0f);
		float2 uvnew=float2(psin.uv.x,1.0f-psin.uv.y);
	MaterialDataGPU mattmp=mattable[matgeneralconsts.matidx];
	float3 finallit = float3(0.0f,0.0f,0.0f);
	LightingInputsCommon lightingInputsCommon;
		lightingInputsCommon.normal = psin.normal;
		lightingInputsCommon.pixelPos = psin.pixelpos.xyz;
		lightingInputsCommon.specularValue = customMatconsts.specularValue;
		lightingInputsCommon.viewPos = customMatconsts.viewPos.xyz;
		lightingInputsCommon.specFactintensity =50.0f;
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
		PointLight locallight = GetPointLight(locallights[0]);
		float3 viewPos = customMatconsts.viewPos.xyz;
		float3 ambientlit = customMatconsts.ambient.xyz * diffusecol.xyz;
		LightingResult directionalLightingRes = GetDirectionalLightRes(normalize(testLightconsts.lightDir.xyz),testLightconsts.lightCol,lightingInputsCommon);
		directionalLightingRes.diffuseRes *= (customMatconsts.diffuse.xyz* diffusecol.xyz);
		directionalLightingRes.specRes *= (customMatconsts.specular.xyz * roughnesscol.xyz);
		LightingResult pointLightingRes = GetPointLightRes(locallight,lightingInputsCommon);
		pointLightingRes.diffuseRes *= (customMatconsts.diffuse.xyz* diffusecol.xyz);
		pointLightingRes.specRes *= (customMatconsts.specular.xyz * roughnesscol.xyz);
		LightingResult finalLightingRes = pointLightingRes;
		if(usedirectionallight)
		{
			finalLightingRes = directionalLightingRes;
		}
		if(customMatconsts.lightingMode == 0)
		{
			finallit = ambientlit + finalLightingRes.diffuseRes  + finalLightingRes.specRes;
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