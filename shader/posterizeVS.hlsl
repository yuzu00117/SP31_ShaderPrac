struct VSInput
{
    float3 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

// Fullscreen quad passthrough VS (same as horror/mosaic)
VSOutput main(VSInput input)
{
    VSOutput o;
    o.pos = float4(input.pos, 1.0);
    o.uv  = input.uv;
    return o;
}
