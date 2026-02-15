struct VSIn
{
    float3 vpos:POS;
};

struct VSConstants
{
//matrix is alias for float4x4
	matrix mvp;
	matrix model;
	matrix view;
	matrix projection;
};
ConstantBuffer<VSConstants> vertexconsts:register(b0);

struct VSOut
{
    float4 pos : SV_Position;
};

VSOut main(VSIn input)
{
    VSOut output;
		//mvp is to be recalculated as PVM,row/column ordering or matrix in hlsl
		matrix mvpRecal = mul(vertexconsts.projection,vertexconsts.view);
		matrix modelMat = vertexconsts.model;
		mvpRecal = mul(mvpRecal,modelMat);
		output.pos =mul(vertexconsts.mvp,float4(input.vpos,1.0f));

    return output;
}