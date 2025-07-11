#include "common.hlsl"

// -----------------------------------------------------------------------------
//  Cook‑Torrance Pixel Shader (fixed version)
// -----------------------------------------------------------------------------
//  * 2025‑07‑11 修正点
//    ‑ ベクトルの向きを LightPos – P / CameraPos – P に統一
//    ‑ N·L, N·V, H·V の取り違えを修正
//    ‑ Cook‑Torrance 分母 4 * (N·L)(N·V) を採用
//    ‑ Fresnel へ H·V を渡す
//    ‑ スペキュラ色にライト色・材質色を乗算
//    ‑ 距離減衰を saturate 付き線形モデルに整理
// -----------------------------------------------------------------------------

// ---- サブ関数宣言 ------------------------------------------------------------
float CalculateBeckmann(float roughness, float nh); //   D term
float CalculateFresnel(float F0, float hv); //   F term
float CalculateGeometricDamping(float nh, float nv,
                                float nl, float hv); //   G term

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// -----------------------------------------------------------------------------
//  Beckmann distribution  D( m = roughness, nh = cosβ )
// -----------------------------------------------------------------------------
float CalculateBeckmann(float roughness, float nh)
{
    float m2 = roughness * roughness;
    float nh2 = nh * nh;
    // exp(‑tan^2 / m^2) / (π m^2 (N·H)^4) を簡易化
    return exp(-(1.0f - nh2) / (m2 * nh2)) / (4.0f * m2 * nh2 * nh2);
}

// -----------------------------------------------------------------------------
//  Fresnel (Schlick approximation)  F0 = base reflectance, hv = H·V
// -----------------------------------------------------------------------------
float CalculateFresnel(float F0, float hv)
{
    return F0 + (1.0f - F0) * pow(1.0f - hv, 5.0f);
}

// -----------------------------------------------------------------------------
//  Geometry (Smith with Schlick‑GGX like min expression)
// -----------------------------------------------------------------------------
float CalculateGeometricDamping(float nh, float nv, float nl, float hv)
{
    float g1 = 2.0f * nh * nv / hv;
    float g2 = 2.0f * nh * nl / hv;
    return min(1.0f, min(g1, g2));
}

// -----------------------------------------------------------------------------
//  Main Pixel Shader
// -----------------------------------------------------------------------------
void main(in PS_IN In,
          out float4 outColor : SV_Target)
{
    // --- ベクトル計算 --------------------------------------------------------
    float3 P = In.WorldPosition.xyz;

    float3 L = normalize(Light.Position.xyz - P); // 光 → ピクセル
    float3 V = normalize(CameraPosition.xyz - P); // カメラ → ピクセル
    float3 N = normalize(In.Normal.xyz); // 法線 (ワールド)
    float3 H = normalize(L + V); // ハーフベクトル

    // --- 距離減衰 ------------------------------------------------------------
    float dist = length(Light.Position.xyz - P);
    float att = saturate(1.0f - dist / Light.PointLightParam.x); // 0‑1 線形減衰

    // --- 各コサイン ----------------------------------------------------------
    float nl = saturate(dot(N, L));
    float nv = saturate(dot(N, V));
    float nh = saturate(dot(N, H));
    float hv = saturate(dot(H, V));

    // --- Cook‑Torrance D / F / G -------------------------------------------
    const float roughness = 0.15f; // 仮パラメータ – 定数バッファ化推奨
    const float F0 = 1.0f; // 金属基準反射率 – 金属度に応じて変化させる

    float D = CalculateBeckmann(roughness, nh);
    float G = CalculateGeometricDamping(nh, nv, nl, hv);
    float F = CalculateFresnel(F0, hv);

    float spec = D * G * F / max(0.001f, 4.0f * nl * nv);

    // --- 基本色 & ライト色 ----------------------------------------------------
    float4 baseTex = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float3 albedo = baseTex.rgb * In.Diffuse.rgb; // アルベド
    float3 metalClr = float3(1.0f, 0.88f, 0.58f); // 金属色 (仮)

    // --- 出力計算 ------------------------------------------------------------
    float3 diffuse = albedo * nl; // Lambert
    float3 specCol = spec * metalClr; // Specular

    float3 finalRGB = (diffuse + specCol) * Light.Diffuse.rgb * att
                    + Light.Ambient.rgb; // 環境光を加算

    outColor = float4(finalRGB, baseTex.a * In.Diffuse.a);
}
