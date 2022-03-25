
Texture2D<float4> image:register(t0);
SamplerState simplesampler:register(s0);
struct VOut
{
    float4 pos : SV_Position;
	float2 uv:UV;
};

float4 main(VOut psin) : SV_TARGET0
{
	float4 texcol=image.Sample(simplesampler,psin.uv);
	return texcol;
	return float4(psin.uv,0.0f,1.0f);
}