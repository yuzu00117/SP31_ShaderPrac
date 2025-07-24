#include "common.hlsl"

Texture2D g_Texture : register(t0);        // カラーテクスチャ
Texture2D g_TextureToon : register(t1);    // ランプテクスチャ
SamplerState g_SamplerState : register(s0); // サンプラー

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);
    
    // 光源からピクセルへのベクトル
    float4 lv = In.WorldPosition - Light.Position;
    float ld = length(lv); // 物体と光源の距離
    lv = normalize(lv); // ベクトルの正規化
    
    // 減衰の計算
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    ofs = max(0, ofs);
    
    // 光量計算（ランバート拡散照明）
    float dotNL = dot(normal.xyz, -lv.xyz);
    dotNL = saturate(dotNL);
    
    // 照明強度を0+ε ～ 1-ε の範囲にクランプ
    // エッジ付近で反対側の色が補間で混ざるのを防ぐため
    float light = clamp(dotNL, 0.001f, 0.999f);
    
    // 減衰も適用
    light *= ofs;
    
    // ベースカラーを取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    // ランプテクスチャからトゥーン照明を取得
    // lightを X 座標として使用、Y は 0.5 固定（1×N テクスチャ対応）
    float2 rampUV = float2(light, 0.5f);
    float4 rampColor = g_TextureToon.Sample(g_SamplerState, rampUV);
    
    // 最終色の計算
    // ベースカラーとランプカラーを乗算
    outDiffuse.rgb *= In.Diffuse.rgb * rampColor.rgb;
    
    // アンビエント光を加算
    outDiffuse.rgb += outDiffuse.rgb * Light.Ambient.rgb;
    
    // アルファ値を設定
    outDiffuse.a *= In.Diffuse.a;
    
    // 視線ベクトルと法線の内積で輪郭を判定（エッジライン）
    float3 eyev = normalize(CameraPosition.xyz - In.WorldPosition.xyz);
    float edgeDetection = dot(eyev, normal.xyz);
    edgeDetection = abs(edgeDetection);
    
    // 輪郭線の閾値（この値より小さい場合は輪郭線とみなす）
    float edgeThreshold = 0.2f;
    
    // 輪郭線判定：閾値より小さい場合は輪郭線色に置き換え
    if (edgeDetection < edgeThreshold)
    {
        outDiffuse.rgb = float3(0.0f, 0.0f, 0.0f); // 黒い輪郭線
    }
}