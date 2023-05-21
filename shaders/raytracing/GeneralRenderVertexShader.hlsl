struct VSIn
{
    float3 vpos:POS;
	float3 normal:NORMAL;
};

struct VSConstants
{
//matrix is alias for float4x4
	matrix mvp;
};
ConstantBuffer<VSConstants> vertexconsts:register(b0);
ConstantBuffer<VSConstants> vertexconsts2:register(b1);

struct VSOut
{
    float4 pos : SV_Position;
	float3 normal: NORM;
	float3 position: WPOS;
};

VSOut main(VSIn input)
{
    VSOut output;

		
		
		output.pos =mul(vertexconsts.mvp,float4(input.vpos,1.0f));
		output.normal=input.normal;
		output.position=(mul(vertexconsts2.mvp,float4(input.vpos,1.0f))).xyz;

    return output;
}