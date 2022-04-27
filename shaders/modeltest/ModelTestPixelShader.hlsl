

struct VSOut
{
    float4 pos : SV_Position;
	float3 normal: NORM;
};

float4 main(VSOut psin) : SV_TARGET0
{
	return float4(psin.normal,1.0f);
}