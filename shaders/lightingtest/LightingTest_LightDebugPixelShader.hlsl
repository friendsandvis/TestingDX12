
struct DebugRenderData
{
	float4 DebugCol;
};
ConstantBuffer<DebugRenderData> renderData:register(b2,space0);
struct VSOut
{
    float4 pos : SV_Position;
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
float4 main(VSOut psin) : SV_TARGET0
{

	return  float4(renderData.DebugCol.xyz,1.0f);
	
}