#include "common.hlsl"

// 1パス目：ライト視点でシャドウマップを作る頂点シェーダ
void main(in VS_IN In, out PS_IN Out)
{
    // ★ ここでは View / Projection にライトカメラの行列が
    //    CPU 側からセットされている前提（PDFの手順想定）

    matrix wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);

    // 以下は PS 側で特に使わないが、構造体を埋めておく
    float4 normal = float4(In.Normal.xyz, 0.0f);
    float4 worldNormal = mul(normal, World);
    Out.Normal = normalize(worldNormal);

    Out.Diffuse       = In.Diffuse;
    Out.TexCoord      = In.TexCoord;
    Out.WorldPosition = mul(In.Position, World);

    // 1パス目ではライト座標は使わないので 0 で埋める
    Out.LightPosition = float4(0, 0, 0, 1);
}
