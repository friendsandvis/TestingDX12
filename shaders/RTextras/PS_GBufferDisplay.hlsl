#define NUMGBUFFERTEXTURES 3
Texture2D<float4> image[NUMGBUFFERTEXTURES]:register(t0);
SamplerState simplesampler:register(s0);

struct Vout
{
	float4 opos:SV_Position;
	float4 vcol : COLOUR0;
	float2 uv:UV;
};

float4 main(Vout psin) : SV_TARGET0
{
	//return float4(psin.uv,0.0f,1.0f);
	return image[2].Sample(simplesampler,psin.uv);//outputs worldpos only(index 2)
}