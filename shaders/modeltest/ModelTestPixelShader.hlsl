
Texture2D<float4> textures[]: register(t0);
struct VSOut
{
    float4 pos : SV_Position;
	float3 normal: NORM;
	float2 uv:UV;
};

float4 main(VSOut psin) : SV_TARGET0
{
	return float4(psin.normal,1.0f);
}