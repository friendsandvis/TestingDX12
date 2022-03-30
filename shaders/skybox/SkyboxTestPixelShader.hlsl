
SamplerState simplesampler:register(s0);
TextureCube skybox:register(t0);
struct VSOut
{
    float4 pos : SV_Position;
	float3 cubesamplecoord: UV;
};

float4 main(VSOut psin) : SV_TARGET0
{
	return skybox.Sample(simplesampler,psin.cubesamplecoord);
	//return float4(psin.cubesamplecoord,1.0f);
}