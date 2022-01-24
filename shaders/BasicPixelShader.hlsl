struct Vout
{
	float4 opos:SV_Position;
	float4 vcol : COLOUR0;
};

float4 main(Vout psin) : SV_TARGET0
{
	return psin.vcol;//float4(0.0f,1.0f,0.0f,1.0f);
}