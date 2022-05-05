




struct VSOut
{
    float4 pos : SV_Position;
	float2 uv:UV;
};

float4 main(VSOut psin) : SV_TARGET0
{
	return float4(psin.uv,0.0f,1.0f);
}