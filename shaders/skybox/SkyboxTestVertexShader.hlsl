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

		
		float4 vposmod=float4(input.vpos,0.0f)+vertexconsts.campos;//possible fix added to set w to 0 to avoid cam movement but need to avoid this campos offset added.
		output.pos =mul(vertexconsts.mvp,vposmod);
		//output.pos=output.pos-vertexconsts.campos;
		
		output.pos.z=output.pos.w;
		output.cubesamplecoord=normalize(input.vpos);

    return output;
}