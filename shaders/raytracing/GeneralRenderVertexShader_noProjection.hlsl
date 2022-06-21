struct VSIn
{
    float3 vpos:POS;
	float2 uv:VUV;
};

struct VSOut
{
    float4 pos : SV_Position;
	float2 uv: UV;
	float3 position: WPOS;
};

VSOut main(VSIn input)
{
    VSOut output;

		
		
		output.pos =float4(input.vpos,1.0f);
		output.uv=input.uv;
		output.position=input.vpos;

    return output;
}