struct VSIn
{
    float3 vpos:POS;
	float2 uv:PUV;
};

struct VSConstants
{
//matrix is alias for float4x4
	matrix mvp;
	float4 campos;
};
ConstantBuffer<VSConstants> vertexconsts:register(b0);

struct VSOut
{
    float4 pos : SV_Position;
	float3 cubesamplecoord: UV;
};

VSOut main(VSIn input)
{
    VSOut output;

		
		float4 vposmod=float4(input.vpos,1.0f)+vertexconsts.campos;
		output.pos =mul(vertexconsts.mvp,vposmod);
		//output.pos=output.pos-vertexconsts.campos;
		
		output.pos.z=output.pos.w;
		output.cubesamplecoord=normalize(input.vpos);

    return output;
}