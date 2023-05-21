

struct SimpleAOpayload
{
	float aoresult;//0 means hit & 1 means miss
};
[shader("miss")]
void missmain(inout SimpleAOpayload payload)
{
	payload.aoresult = 1.0f;
}