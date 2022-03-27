
struct SFConstants
{
	uint lodclamp;
};
Texture2D<float4> image:register(t0);
Texture2D<float4> minlodmap:register(t1);
SamplerState simplesampler:register(s0);
FeedbackTexture2D<SAMPLER_FEEDBACK_MIN_MIP> imagefeedback:register(u0);
ConstantBuffer<SFConstants> sfsconstants:register(b0);


struct VSOut
{
    float4 pos : SV_Position;
	float2 uv:UV;
};

float4 main(VSOut psin) : SV_TARGET0
{
int2 offset=int2(0,0);
 float4 outcol=image.Sample(simplesampler,psin.uv,offset,sfsconstants.lodclamp);
 #ifdef SFSSUPPORTED
 imagefeedback.WriteSamplerFeedback(image,simplesampler,psin.uv);
 #endif//SFSSUPPORTED
	//return float4(psin.uv,0.0f,1.0f);
	return outcol;
}