struct VSIn
{
    uint vertexId : SV_VertexID;
};

struct VSOut
{
    float4 pos : SV_Position;
    float4 color : COLOUR0;
};

VSOut main(VSIn input)
{
    VSOut output;

    if (input.vertexId == 0)
        output.pos = float4(0.0, 1.0F, 0.0, 1.0);
		else if (input.vertexId == 1)
        output.pos = float4(1.0F, -1.0F, 0.0, 1.0);
    else if (input.vertexId == 2)
        output.pos = float4(-1.0F, -1.0F, 0.0, 1.0);
    else if (input.vertexId == 3)
        output.pos = float4(1.0F,1.0F, 0.0, 1.0);

    output.color = float4(1.0f,1.0f,1.0,1.0f);

    return output;
}