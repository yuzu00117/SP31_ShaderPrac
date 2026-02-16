// =============================================================================
// パーティクル ピクセルシェーダー [particlePS.hlsl]
// 丸いグラデーション＋フェードアウトで描画する
// =============================================================================

// Texture2D g_Texture : register(t0); // テクスチャ版（将来拡張用）
// SamplerState g_Sampler : register(s0);

struct PS_IN
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float  Alpha    : TEXCOORD1;
};

float4 main(PS_IN input) : SV_Target
{
    // UV中心からの距離で丸い形を作る（テクスチャ不要のプロシージャル生成）
    float2 center = input.TexCoord - float2(0.5, 0.5);
    float dist = length(center) * 2.0; // 0?1の範囲
    
    // 円の外は描画しない
    if (dist > 1.0)
        discard;
    
    // ソフトエッジ（中心が明るく、外側に向かってフェードアウト）
    float softEdge = 1.0 - smoothstep(0.0, 1.0, dist);
    
    // パーティクルの色（暖色系グラデーション: 寿命に応じて変化）
    float3 colorHot  = float3(1.0, 0.8, 0.2);  // 黄色?オレンジ（新しいパーティクル）
    float3 colorCold = float3(1.0, 0.2, 0.05);  // 赤?暗め（古いパーティクル）
    float3 color = lerp(colorCold, colorHot, input.Alpha);
    
    // 最終アルファ
    float finalAlpha = softEdge * input.Alpha * 0.8;
    
    return float4(color * finalAlpha, finalAlpha);
}
