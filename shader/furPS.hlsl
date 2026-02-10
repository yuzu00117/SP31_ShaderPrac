#include "common.hlsl"

// テクスチャ
Texture2D g_Texture      : register(t0); // モデルテクスチャ
Texture2D g_NoiseTexture : register(t1); // ノイズテクスチャ（毛の密度パターン）
SamplerState g_SamplerState : register(s0);

// ファーシェーダー専用定数バッファ (b7)
cbuffer FurBuffer : register(b7)
{
    float  FurLength;
    float  ShellIndex;
    float  TotalShells;
    float  FurDensity;

    float4 FurColor;
    float4 GravityDir;
};

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // シェル層の正規化比率 (0.0 = 根元, 1.0 = 毛先)
    float shellRatio = ShellIndex / max(TotalShells, 1.0);

    // ノイズテクスチャからサンプル（UV をタイリングして毛の細かさを表現）
    float2 furUV = In.TexCoord * 20.0; // タイリング倍率（大きいほど毛が細かい）
    float noiseVal = g_NoiseTexture.Sample(g_SamplerState, furUV).r;

    // αカット閾値：毛先ほど閾値が高く、毛がまばらになる
    float threshold = shellRatio * (1.0 - FurDensity) + (1.0 - noiseVal);
    if (threshold > 1.0)
    {
        discard; // 毛の隙間 → ピクセルを捨てる
    }

    // ベースカラー（モデルテクスチャ）を取得
    float4 texColor = g_Texture.Sample(g_SamplerState, In.TexCoord);

    // ライティング（Lambert ディフューズ）
    float3 normal = normalize(In.Normal.xyz);
    float3 lightDir = normalize(-Light.Direction.xyz);
    float NdotL = saturate(dot(normal, lightDir));
    float lighting = NdotL * 0.7 + 0.3; // 最低30%のアンビエント保証

    // 毛の色をブレンド（根元はテクスチャ色、毛先はFurColorに近づく）
    float3 baseColor = texColor.rgb * In.Diffuse.rgb;
    float3 furTint = lerp(baseColor, FurColor.rgb, shellRatio * 0.5);

    // 層が上がるほど暗くする（自己遮蔽シミュレーション）
    float occlusion = lerp(0.4, 1.0, shellRatio);

    // 最終カラー
    outDiffuse.rgb = furTint * lighting * occlusion * Light.Diffuse.rgb;
    outDiffuse.rgb += baseColor * Light.Ambient.rgb * 0.3;

    // α値：毛先ほど透明になる
    float alpha = (1.0 - shellRatio) * FurColor.a;
    alpha = saturate(alpha);
    outDiffuse.a = alpha;
}
