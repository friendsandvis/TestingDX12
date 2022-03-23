

struct Vout
{
	float4 opos:SV_Position;
	float3 col: COL;
};

float4 main(Vout psin) : SV_TARGET0
{
	return float4(psin.col,1.0f);
}