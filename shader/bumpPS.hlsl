#include "common.hlsl"

Texture2D g_Texture : register(t0); // テクスチャ０番
Texture2D g_TextureNormal : register(t1); // テクスチャ１番（法線マップ）
SamplerState g_SamplerState : register(s0); // サンプラー０番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // 物体から光源へのベクトル
    float4 lv = In.WorldPosition - Light.Position;
    
    // 物体と光源の距離
    float4 ld = length(lv);
    
    // ベクトルの正規化
    lv = normalize(lv);
    
    // 減衰の計算
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    // 減衰率の負値は0にする
    ofs = max(0, ofs);
    
    // 法線マップ取得
    float4 normalMap = g_TextureNormal.Sample(g_SamplerState, In.TexCoord);
    
    // 色データをベクトルに展開 (0-1の範囲を-1～1の範囲に変換)
    normalMap = (normalMap * 2.0f) - 1.0f;
    
    // 法線として格納しなおす
    float4 normal;
    normal.x = normalMap.x;
    normal.y = normalMap.y;
    normal.z = normalMap.z;
    normal.w = 0.0f;
    
    // ピクセルの法線を正規化
    normal = normalize(normal);
    
    // 光源計算（内積の負号を削除）
    float light = dot(normal.xyz, lv.xyz);
    //light = saturate(light);
    light *= ofs; // 明るさを減衰
    
    // テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light; // 明るさを乗算
    outDiffuse.a *= In.Diffuse.a; // α値に明るさは関係ない
    outDiffuse.rgb += Light.Ambient.rgb; // 環境光の加算
    
    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    // ハーフベクトルを計算
    float3 halfv = eyev + lv.xyz;
    halfv = normalize(halfv);
    
    // スペキュラーの計算
    float specular = dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 30);
    
    outDiffuse.rgb += (specular * ofs);
}