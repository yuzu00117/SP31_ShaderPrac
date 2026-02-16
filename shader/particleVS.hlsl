// =============================================================================
// パーティクル 頂点シェーダー [particleVS.hlsl]
// StructuredBufferからSV_VertexIDでパーティクルデータを取得し、
// ジオメトリシェーダーにパススルーする
// =============================================================================

struct Particle
{
    float3 Position;
    float  Life;
    float3 Velocity;
    float  MaxLife;
};

StructuredBuffer<Particle> Particles : register(t0);

struct VS_OUT
{
    float3 Position : POSITION;
    float  Life     : TEXCOORD0;
    float  MaxLife  : TEXCOORD1;
};

VS_OUT main(uint vertexID : SV_VertexID)
{
    VS_OUT output;

    Particle p = Particles[vertexID];
    output.Position = p.Position;
    output.Life     = p.Life;
    output.MaxLife  = p.MaxLife;

    return output;
}
