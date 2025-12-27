struct VSIn
{
    float3 vpos:POS;
	float3 normal:NORMAL;
	float2 vuv:VUV;
	uint instanceID : SV_InstanceID;
};

struct VSConstants
{
//matrix is alias for float4x4
	matrix mvp;
	matrix model;
	matrix view;
	matrix projection;
};
struct InstanceData
{
	//matrix is alias for float4x4
	matrix model;
};
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
ConstantBuffer<VSConstants> vertexconsts:register(b0);
ConstantBuffer<GeneralData> testLightconsts:register(b3,space0);
StructuredBuffer<InstanceData> instanceData:register(t1,space1);

struct VSOut
{
    float4 pos : SV_Position;
	float3 normal: NORM;
	float4 pixelpos : PIXPOS;
	float2 uv:UV;
};

VSOut main(VSIn input)
{
    VSOut output;

		const bool useInstanceData = (testLightconsts.useInstanceData == 1.0f);
		//mvp is to be recalculated as PVM,row/column ordering or matrix in hlsl
		matrix mvpRecal = mul(vertexconsts.projection,vertexconsts.view);
		matrix modelMat = vertexconsts.model;
		if(useInstanceData)
		{
			modelMat = instanceData[input.instanceID].model;
		}
		mvpRecal = mul(mvpRecal,modelMat);
		output.pos =mul(mvpRecal,float4(input.vpos,1.0f));
		output.normal=normalize(input.normal);
		output.pixelpos = mul(modelMat,float4(input.vpos,1.0f));
		output.uv=input.vuv;

    return output;
}