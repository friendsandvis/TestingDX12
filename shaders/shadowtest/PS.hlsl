
Texture2D<float4> image:register(t0);
SamplerState simplesampler:register(s0);
struct SimpleMaterial
{
	float4 colour;
};
ConstantBuffer<SimpleMaterial> mat:register(b1);

struct Vout
{
	float4 opos:SV_Position;
	float4 vcol : COLOUR0;
	float2 uv:UV;
};

float4 main(Vout psin) : SV_TARGET0
{
	return mat.colour;
	//return float4(psin.uv,0.0f,1.0f);
	return image.Sample(simplesampler,psin.uv);
}