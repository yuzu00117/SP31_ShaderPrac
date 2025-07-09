#include "common.hlsl"
Texture2D g_Texture : register(t0); // テクスチャ０番
SamplerState g_SamplerState : register(s0); // サンプラー０番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // 光源からピクセルへのベクトル
    float4 lv = In.WorldPosition - Light.Position;

    // 物体と光源の距離
    float4 ld = length(lv);

    // ベクトルの正規化
    lv = normalize(lv);

    // 減衰の計算
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld; // 減衰の計算
    // 減衰を未満にしない
    ofs = max(0, ofs);

    // ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);

    // 光量計算
    float light = dot(normal.xyz, lv.xyz);
    light = saturate(light);
    light *= ofs; // 明るさを減衰させる

    // テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light + Light.Ambient.rgb; // 明るさを乗算
    outDiffuse.a *= In.Diffuse.a; // α値に明るさは関係ない

    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

    // ハーフベクトルを計算
    float3 halfv = eyev + lv.xyz; // 視線ベクトル＋ライトベクトル
    halfv = normalize(halfv);

    // スペキュラーの計算
    float specular = dot(halfv, normal.xyz); // ハーフベクトルと法線の内積
    specular = saturate(specular);
    specular = pow(specular, 30);

    outDiffuse.rgb += (specular * ofs); // スペキュラも減衰させてから加算して出力
}
