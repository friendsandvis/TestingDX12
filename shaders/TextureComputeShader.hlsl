
RWTexture2D<float4> tex: register(u0);
[numthreads(1,1,1)]
void main(
uint3 dispatchThreadId: SV_DispatchThreadID
)
{
	
	float2 uv=(dispatchThreadId.xy/float2(128.0f,128.0f));
	float delta=distance(uv,float2(0.5f,0.5f));
	tex[dispatchThreadId.xy]=float4(delta,0.0f,0.0f,1.0f);
}