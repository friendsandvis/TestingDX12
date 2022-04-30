

struct VSOut
{
    float4 pos : SV_Position;
	float3 normal: NORM;
};

struct PSOut
{
	float4 maincolour: SV_TARGET0;
	float4 normals: SV_TARGET1;
};

PSOut main(VSOut psin)
{
	PSOut psout;
	psout.maincolour=float4(psin.normal,1.0f);
	psout.normals=float4(1.0f,1.0f,1.0f,1.0f);
	return psout;
}