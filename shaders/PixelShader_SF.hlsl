
Texture2D<float4> image:register(t0);
Texture2D<float4> minlodmap:register(t1);
SamplerState simplesampler:register(s0);
FeedbackTexture2D<SAMPLER_FEEDBACK_MIN_MIP> imagefeedback:register(u0);



struct Vout
{
	float4 opos:SV_Position;
	float4 vcol : COLOUR0;
	float2 uv:UV;
};

float4 main(Vout psin) : SV_TARGET0
{
 float4 outcol=image.Sample(simplesampler,psin.uv);
 imagefeedback.WriteSamplerFeedback(image,simplesampler,psin.uv);
	//return float4(psin.uv,0.0f,1.0f);
	return outcol;
}