#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// Sky Dome Pixel Shader
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // ワールド座標のY値でグラデーションを作成（より確実な方法）
    float3 worldPos = normalize(In.WorldPosition.xyz);
    float height = worldPos.y; // -1.0 (下) から 1.0 (上) の範囲
    
    // 0.0から1.0にマッピング
    float gradient = (height + 1.0f) * 0.5f;
    gradient = saturate(gradient); // 0.0-1.0の範囲にクランプ
    
    // 上部は明るい青、下部は薄いオレンジ色のグラデーション
    float4 skyColor = float4(0.5f, 0.8f, 1.0f, 1.0f);      // 明るい青空色（上部）
    float4 horizonColor = float4(1.0f, 0.7f, 0.4f, 1.0f);  // オレンジ色（下部）
    
    // 上下でスムーズにブレンド
    float4 color = lerp(horizonColor, skyColor, gradient);
    
    // アルファは完全不透明
    color.a = 1.0f;
    
    outDiffuse = color;
}