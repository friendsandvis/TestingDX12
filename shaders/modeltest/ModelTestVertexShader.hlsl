struct VSIn
{
    float3 vpos:POS;
	float3 normal:NORMAL;
	float2 vuv:VUV;
};

struct VSConstants
{
//matrix is alias for float4x4
	matrix mvp;
	matrix model;
};
ConstantBuffer<VSConstants> vertexconsts:register(b0);

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

		
		
		output.pos =mul(vertexconsts.mvp,float4(input.vpos,1.0f));
		output.normal=normalize(input.normal);
		output.pixelpos = mul(vertexconsts.model,float4(input.vpos,1.0f));
		output.uv=input.vuv;

    return output;
}