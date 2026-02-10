#include "common.hlsl"

// ファーシェーダー専用定数バッファ (b7)
cbuffer FurBuffer : register(b7)
{
    float  FurLength;      // 毛の長さ
    float  ShellIndex;     // 現在のシェル層 (0 ? TotalShells-1)
    float  TotalShells;    // シェル総数
    float  FurDensity;     // 毛の密度

    float4 FurColor;       // 毛の色 (rgb) + α倍率 (a)
    float4 GravityDir;     // 重力方向 (xyz) + 強さ (w)
};

void main(in VS_IN In, out PS_IN Out)
{
    // シェル層の正規化比率 (0.0 = 根元, 1.0 = 毛先)
    float shellRatio = ShellIndex / max(TotalShells, 1.0);

    // 法線をワールド空間へ変換
    float4 normal = float4(In.Normal.xyz, 0.0);
    float4 worldNormal = normalize(mul(normal, World));

    // 法線方向への押し出し量
    float3 shellOffset = worldNormal.xyz * FurLength * shellRatio;

    // 重力の影響（毛先ほど強く受ける）
    float3 gravityOffset = GravityDir.xyz * GravityDir.w * shellRatio * shellRatio;

    // ワールド座標を計算（押し出し + 重力）
    float4 worldPos = mul(In.Position, World);
    worldPos.xyz += shellOffset + gravityOffset;

    // ビュー・プロジェクション変換
    float4 viewPos = mul(worldPos, View);
    Out.Position = mul(viewPos, Projection);

    // ワールド座標を出力（ライティング用）
    Out.WorldPosition = worldPos;

    // 法線を出力
    Out.Normal = worldNormal;

    // 頂点カラー・UV をそのまま出力
    Out.Diffuse = In.Diffuse;
    Out.TexCoord = In.TexCoord;
}
