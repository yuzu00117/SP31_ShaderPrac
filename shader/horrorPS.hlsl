Texture2D SceneTex : register(t0);
SamplerState samLinear : register(s0);

// Horror effect parameters. Note: cbuffer slot b7 is used to avoid conflicts with engine globals.
cbuffer HorrorParams : register(b7)
{
    float2 NoiseSeed;          // accumulate per frame, reset when large
    float  NoiseScale;         // controls noise frequency / intensity
    float  GrayScaleEnabled;   // 0.0/1.0
    float  ContrastPower;      // e.g., 5.0
    float2 NoiseAddPerFrame;   // delta added to NoiseSeed per frame (CPU updates)
    float  pad;                // padding to 16-byte boundary
}

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

// Pseudo-random using frac + dot + sin.
// The following magic constants are intentionally irregular; changing them too much
// can introduce visible patterns/bands.
float Rand(float2 coord)
{
    float n = dot(coord, float2(12.9898, 78.233));
    return frac(sin(n) * 43758.5453);
}

float3 toGray(float3 c)
{
    // Luma-weighted average (Rec. 709)
    float g = dot(c, float3(0.2126, 0.7152, 0.0722));
    return g.xxx;
}

// Pixel shader entry (used at runtime via D3DCompileFromFile)
float4 ps_main(PSInput input) : SV_Target
{
    float2 uv = input.uv;

    // 1) Sample original scene color
    float4 col = SceneTex.Sample(samLinear, uv);

    // 2) Optional grayscale
    if (GrayScaleEnabled > 0.5)
    {
        col.rgb = toGray(col.rgb);
    }

    // 3) Add noise on brightness using animated seed.
    //    To prevent striped artifacts during long runs, reset the seed on CPU when it becomes large.
    float2 ncoord = uv * max(1.0, NoiseScale) + NoiseSeed;
    float r = Rand(ncoord);          // [0,1)
    float noise = r - 0.5;           // [-0.5, +0.5)
    float noiseStrength = 0.15;      // tweakable; keep subtle for horror look
    col.rgb = saturate(col.rgb * (1.0 + noise * noiseStrength));

    // 4) Contrast enhancement
    col.rgb = pow(saturate(col.rgb), max(0.0001, ContrastPower));

    // 5) Output
    return col;
}

// Dummy vertex shader named 'main' to satisfy VS HLSL build systems that may try
// to compile this file as a vertex shader with an old profile (vs_2_0). This allows
// the project to build even if it mistakenly compiles this file as a VS.
struct VSOutDummy { float4 pos : POSITION; float2 uv : TEXCOORD0; };
VSOutDummy main(float4 pos : POSITION, float2 uv : TEXCOORD0)
{
    VSOutDummy o; o.pos = pos; o.uv = uv; return o;
}
