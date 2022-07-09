
//struct [raypayload]Simpleraypayload
struct Simpleraypayload
{
	float3 outcol;
};
[shader("miss")]
void missmain(inout Simpleraypayload payload)
{
	payload.outcol=float3(0.0f,0.0f,1.0f);
}