


//Texture2D<float4> image:register(t0);
RWTexture2D<uint> overlaytex:register(u0);
SamplerState simplesampler:register(s0);

struct VSOut
{
    float4 pos : SV_Position;
	float2 uv:UV;
};

float4 main(VSOut psin) : SV_TARGET0
{
uint2 imgsize;
overlaytex.GetDimensions(imgsize.x,imgsize.y);
uint2 fetchlocation=floor(imgsize*psin.uv);
uint texvalue=overlaytex[fetchlocation];
float4 imgdata=float4(texvalue,0.0f,0.0f,1.0f);
	return imgdata;
	//return float4(psin.uv,0.0f,1.0f);
}