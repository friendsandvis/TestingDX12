

struct VSOut
{
    float4 pos : SV_Position;
	float3 normal: NORM;
	float3 position: WPOS;
};

struct PSOut
{
	float4 maincolour: SV_TARGET0;
	float4 normals: SV_TARGET1;
	float4 pos: SV_TARGET2;
};

PSOut main(VSOut psin)
{
	PSOut psout;
	psout.maincolour=float4(0.0f,1.0f,0.0f,1.0f);
	psout.normals=float4(psin.normal,1.0f);
	psout.pos=float4(psin.position,1.0f);
	return psout;
}