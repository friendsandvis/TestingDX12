

struct Vout
{
	float4 opos:SV_Position;
};

float4 main(Vout psin) : SV_TARGET0
{
	return float4(1.0f,0.0f,0.0f,1.0f);
}