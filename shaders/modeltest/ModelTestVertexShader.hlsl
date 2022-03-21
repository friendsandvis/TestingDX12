struct VSIn
{
    float3 vpos:POS;
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
};

VSOut main(VSIn input)
{
    VSOut output;

		
		
		output.pos =mul(vertexconsts.mvp,float4(input.vpos,1.0f));

    return output;
}