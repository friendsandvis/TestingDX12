

struct AOraypayload
{

	float occlussionresult;
};
[shader("miss")]
void missmain(inout AOraypayload payload)
{
	payload.occlussionresult = 10.0f;
}