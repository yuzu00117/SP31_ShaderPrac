#include "common.hlsl"

// 半球ライティングのピクセルシェーダー

Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 normal = normalize(In.Normal); // ピクセルの法線を正規化
    float light = -dot(normal.xyz, normalize(Light.Direction.xyz)); // 光源計算をする

    // テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);

    outDiffuse.rgb *= (In.Diffuse.rgb * light + Light.Ambient.rgb); // 明るさを乗算
    outDiffuse.a *= In.Diffuse.a; // αに関係ないので別計算

    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev); // 正規化する

    // ハーフベクトルの作成
    float3 halfv = eyev + normalize(Light.Direction.xyz); // 視線とライトベクトルを加算
    halfv = normalize(halfv); // 正規化する
    float specular = -dot(halfv, normal.xyz); // ハーフベクトルと法線の内積を計算
    specular = saturate(specular); // サチュレートする
    specular = pow(specular, 30);

    // 半球ライティング
    float norm = dot(normal, normalize(Light.GroundNormal)); // 地面とピクセル法線の内積
    norm = (norm + 1.0f) / 2.0f; // 内積をスケーリング
    float3 hemiColor = lerp(Light.GroundColor, Light.SkyColor, norm); // 色の補間

    outDiffuse.rgb += specular + hemiColor; // まとめて足しこむ
}
