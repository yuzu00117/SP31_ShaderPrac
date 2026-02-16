// =============================================================================
// パーティクル コンピュートシェーダー [particleCS.hlsl]
// パーティクルの位置・速度・寿命をGPU上で更新する
// =============================================================================

struct Particle
{
    float3 Position;
    float  Life;
    float3 Velocity;
    float  MaxLife;
};

// パーティクルバッファ（読み書き）
RWStructuredBuffer<Particle> Particles : register(u0);

cbuffer ParticleParams : register(b11)
{
    float3 EmitterPosition;
    float  DeltaTime;
    float4 Seed; // 乱数シード（フレームごとに変化）
};

// 簡易ハッシュ乱数（0?1）
float Hash(float n)
{
    return frac(sin(n) * 43758.5453123);
}

// 3次元ランダムベクトル生成
float3 RandomVelocity(uint id, float4 seed)
{
    float x = Hash(id * 1.23 + seed.x) * 2.0 - 1.0;
    float y = Hash(id * 2.34 + seed.y) * 1.5 + 0.5; // 上向きバイアス
    float z = Hash(id * 3.45 + seed.z) * 2.0 - 1.0;
    return float3(x, y, z);
}

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint id = dtid.x;

    Particle p = Particles[id];

    // 寿命を減らす
    p.Life -= DeltaTime;

    if (p.Life <= 0.0)
    {
        // リスポーン
        p.Position = EmitterPosition;
        p.Velocity = RandomVelocity(id, Seed);
        p.MaxLife = 1.5 + Hash(id * 4.56 + Seed.w) * 1.5; // 1.5?3.0秒
        p.Life = p.MaxLife;
    }
    else
    {
        // 重力
        p.Velocity.y -= 1.5 * DeltaTime;

        // 位置更新
        p.Position += p.Velocity * DeltaTime;
    }

    Particles[id] = p;
}
