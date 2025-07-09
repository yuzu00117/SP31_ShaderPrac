#include"common.hlsl"

Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
 // 光源からピクセルへのベクトル
    float3 lv = In.WorldPosition.xyz - Light.Position.xyz;

    // ベクトルの正規化
    lv = normalize(lv);

    // ピクセルの法線を正規化
    float3 normal = normalize(In.Normal.xyz);

    // 光源計算をする（ディフューズ光）
    float light = -dot(normal, Light.Direction.xyz);
    light = saturate(light); // 明るさを 0 - 1 に収める

    // テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);

    // ディフューズカラーに光を乗算
    outDiffuse.rgb *= In.Diffuse.rgb * light;

    // アルファは別途乗算
    outDiffuse.a *= In.Diffuse.a;

    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

    // ハーフベクトルの作成
    float3 halfv = normalize(eyev + Light.Direction.xyz);

    // スペキュラーの計算
    float specular = -dot(halfv, normal);
    specular = saturate(specular);
    specular = pow(specular, 30);

    float ofs = 1.0f; // スペキュラー強度係数
    outDiffuse.rgb += (specular * ofs);

    // --- リムライティング ---

    // 光の方向と視線ベクトルの考慮（逆光ほど明るい）
    float lit = 1.0f - max(0, dot(lv, eyev));

    // 輪郭部分ほど明るくする
    float lim = 1.0f - max(0, dot(normal, -eyev));

    // lit と lim の明るさを合成
    lim *= lit;
    lim = pow(lim, 3);

    // リムライトのカラーと強さを設定
    float3 rimColor = float3(1.0, 1.0, 1.0); // 白いリムライト
    float rimIntensity = 0.5f; // 強さ（0.0?1.0）

    // リムライティングの明るさをデフューズに加算
    outDiffuse.rgb += lim * rimColor * rimIntensity;
}