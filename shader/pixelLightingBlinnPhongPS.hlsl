#include "common.hlsl"

// ピクセルライティングのピクセルシェーダー

Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    
      // ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);
    float light = -dot(normal.xyz, Light.Direction.xyz); // 光源計算をする
    light = saturate(light); // 明るさを0-1の範囲に収める

    // テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light; // 明るさを乗算
    outDiffuse.a *= In.Diffuse.a; // αに明るさは関係ないので別計算

    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev); // 正規化する

  
    
    //ハーフベクトルの作成
    float3 halfv = normalize(eyev + Light.Direction.xyz); //カメラと光源の中間ベクトルを作成
    eyev= normalize(eyev); //カメラベクトルを正規化

    //スペキュラの計算
    float specular = -dot(halfv, normal.xyz);//ハーフベクトルと法線の内積を計算
    specular = saturate(specular); // 値をサチュレート
    specular = pow(specular, 30); // ここでは30を乗してみる
    
    outDiffuse.rgb += specular; //スペキュラ値をでひゅーずとして足しこむ
}