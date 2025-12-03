#include "common.hlsl"

// 2パス目：通常カメラ＋ライト空間座標出力用 VS
void main(in VS_IN In, out PS_IN Out)
{
    // 通常カメラ用の頂点変換
    matrix wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);

    // 法線のワールド変換（BlinnPhong と同じ）
    float4 normal      = float4(In.Normal.xyz, 0.0f);
    float4 worldNormal = mul(normal, World);
    worldNormal        = normalize(worldNormal);
    Out.Normal         = worldNormal;

    Out.Diffuse       = In.Diffuse;
    Out.TexCoord      = In.TexCoord;
    Out.WorldPosition = mul(In.Position, World);

    // ★ ライト空間での座標（World * Light.ViewMatrix * Light.ProjectionMatrix）
    matrix lwvp = mul(World, Light.ViewMatrix);
    lwvp        = mul(lwvp, Light.ProjectionMatrix);
    Out.LightPosition = mul(In.Position, lwvp);
}
