#include "common.hlsl"
Texture2D g_Texture : register(t0); // テクスチャ０番
SamplerState g_SamplerState : register(s0); // サンプラー０番

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
    
    // Toon Shader の段階的な陰影
    // 3段階の明度に分割
    float toonFactor;
    if (dotNL > 0.8)
        toonFactor = 1.0; // 最も明るい部分
    else if (dotNL > 0.4)
        toonFactor = 0.6; // 中間の明るさ
    else if (dotNL > 0.1)
        toonFactor = 0.3; // 暗い部分
    else
        toonFactor = 0.1; // 最も暗い部分（完全に黒ではない）
    
    // 減衰も適用
    toonFactor *= ofs;
    
    // テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    // Toonシェーディングを適用
    outDiffuse.rgb *= In.Diffuse.rgb * toonFactor + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;
    
    // リム照明効果（輪郭の明るさ）
    float3 eyev = normalize(CameraPosition.xyz - In.WorldPosition.xyz);
    float rim = 1.0 - saturate(dot(normal.xyz, eyev));
    rim = pow(rim, 2.0);
    
    // リム照明の強度を調整
    float rimIntensity = 0.3;
    outDiffuse.rgb += rim * rimIntensity * Light.Diffuse.rgb;
    
    // スペキュラーハイライト（アニメ風の鋭いハイライト）
    float3 halfv = normalize(eyev - lv.xyz);
    float specular = dot(halfv, normal.xyz);
    specular = saturate(specular);
    
    // Toon風のスペキュラー（段階的）
    float toonSpecular;
    if (specular > 0.95)
        toonSpecular = 1.0;
    else if (specular > 0.8)
        toonSpecular = 0.5;
    else
        toonSpecular = 0.0;
    
    // スペキュラーを加算（アニメ風の鋭いハイライト）
    outDiffuse.rgb += toonSpecular * ofs * 0.8;
    
    // 色の彩度を少し上げてアニメ風に
    outDiffuse.rgb = saturate(outDiffuse.rgb * 1.1);
    
    // 輪郭線判定（エッジ検出）
    // 視線ベクトルと法線の内積で輪郭を判定
    float edgeDetection = dot(eyev, normal.xyz);
    edgeDetection = abs(edgeDetection); // 絶対値を取る
    
    // 輪郭線の閾値（この値より小さい場合は輪郭線とみなす）
    float edgeThreshold = 0.2;
    
    // 輪郭線の色（黒）
    float3 edgeColor = float3(0.0, 0.0, 0.0);
    
    // 輪郭線判定：閾値より小さい場合は輪郭線色に置き換え
    if (edgeDetection < edgeThreshold)
    {
        outDiffuse.rgb = edgeColor;
    }
}
