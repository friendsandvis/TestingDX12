
RWTexture2D<float4> tex: register(u0);
[numthreads(1,1,1)]
void main(
uint3 dispatchThreadId: SV_DispatchThreadID
)
{
	
	tex[dispatchThreadId.xy]=float4(1.0f,1.0f,1.0f,1.0f);
}