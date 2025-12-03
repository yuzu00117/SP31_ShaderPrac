#include "common.hlsl"

// t0 : 通常テクスチャ（床など）
// t1 : 1パス目で作ったシャドウマップ（不確実：実際のバインド位置に合わせて修正してください）
Texture2D g_Texture    : register(t0);
Texture2D g_ShadowMap  : register(t1);
SamplerState g_Sampler : register(s0);

// 2パス目：シャドウマップを見て影を落とす PS
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // ベースの色（BlinnPhong の diffuse 部分だけ真似している簡易版）
    float4 baseColor = g_Texture.Sample(g_Sampler, In.TexCoord);

    // ライト空間座標 → デバイス座標 → [0,1] UV
    float3 proj = In.LightPosition.xyz / In.LightPosition.w;
    float2 shadowUV = proj.xy * 0.5f + 0.5f;

    // 範囲外は「影なし」
    bool outside =
        (shadowUV.x < 0.0f || shadowUV.x > 1.0f ||
         shadowUV.y < 0.0f || shadowUV.y > 1.0f);

    float shadowFactor = 1.0f;

    if (!outside)
    {
        // シャドウマップのサンプル値
        float shadowSample = g_ShadowMap.Sample(g_Sampler, shadowUV).r;

        // ★ PDF で指定されている式があればそちらを優先してください。
        // ここでは「0=真っ黒影, 1=影なし」とみなして
        // 影部分を 0.3 倍くらい暗くする例を示しています。
        shadowFactor = lerp(0.3f, 1.0f, shadowSample);
    }

    outDiffuse = baseColor * shadowFactor;
}
