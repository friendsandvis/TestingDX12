struct VSIn
{
    float3 vpos:POS;
	float2 uv:PUV;
};

struct VSConstants
{
//matrix is alias for float4x4
	matrix mvp;
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

		
		
		output.pos =mul(vertexconsts.mvp,float4(input.vpos,1.0f));
		output.cubesamplecoord=normalize(input.vpos);

    return output;
}