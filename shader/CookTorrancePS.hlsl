#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

//パラメータ計算用ローカル関数のプロトタイプ宣言
float CalculateBeckmann(float m, float alpha);
float CalculateFresnel(float n, float c);
float CalculateGeometricDamping(float nh, float nv, float nl, float vh);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //物体から光源へのベクトル
    float3 lv = Light.Position.xyz - In.WorldPosition.xyz;
    //物体から光源の距離
    float ld = length(lv);
    //ベクトルの正規化
    lv = normalize(lv);
    
    //距離の計算
    float att = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    //距離減衰を適用する
    att = max(0.0f, att);
    
    //ピクセルの法線を正規化
    float3 normal = normalize(In.Normal.xyz);
    float light = 0.5f + 0.5f * dot(normal.xyz, lv.xyz);
    light *= att;   //距離による減衰
    
    //テクスチャのピクセル色を取得
    float4 metalcolor = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse = metalcolor;
    outDiffuse.rgb *= In.Diffuse.rgb;
    outDiffuse.rgb *= light;    //距離の減衰
    
    //カメラから物体のピクセルへの方向ベクトル
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);
    
    //ハーフベクトルを計算
    float3 halfv = eyev + lv.xyz;
    halfv = normalize(halfv);
    
    
    //---------------------------------------------------------------------
    //スペキュラー成分
    float nv = max(0.0f, dot(normal.xyz, eyev)); 
    float m = max(0.0f, dot(normal.xyz, halfv)); 
    float vh = max(0.0f, dot(eyev, halfv)); 
    float nl = max(0.0f, dot(normal.xyz, lv.xyz)); 
    
    float D = CalculateBeckmann(0.3f, m); //表面の細かい凹凸
    float G = CalculateGeometricDamping(m, nv, nl, vh);
    float F = CalculateFresnel(0.8f, dot(lv.xyz, halfv)); //表面反射率はDf
    
    float3 specular = metalcolor.rgb * (F * D * G / max(0.001f, nv));
    
    //スペキュラー色 = max(0, cl * cs * n * a / nv / ndotv.rgb);
    
    outDiffuse.rgb += (specular * att);
}

//マイクロファセットの分散
//パラメータ分布関数
// α:物体のかさかさ感
//平滑な面で光を反射する面積の割合
float CalculateBeckmann(float smooth, float nh)
{
    float D;
    D = (
         exp(
             -((1 - (nh * nh))
               / (smooth * smooth * nh * nh))
         ) / (4 * smooth * smooth * nh * nh * nh * nh)
    );
    
    return D;
}

//フレネル項
//ん:ほ.？
float CalculateFresnel(float metal, float hv)
{
    //Schlick'sFresnelの式で近似する
    return metal + (1 - metal) * pow(1 - hv, 5);
    
    //簡単近似版
    //float F;
    //float f = sqrt(metal * metal + hv * hv - 1);
    //float T1 = ((F - hv) / (F + hv));
    //float T2 = (1 + (hv * (F - hv) / F) / (hv * (F + hv) - 1));
    
    //F = 0.5 * T1 * T1 * T2;
    
    //return F;
}

//幾何減衰項
float CalculateGeometricDamping(float nh, float nv, float nl, float vh)
{
    float G;
    G = min(1.0f, min(2 * nh * nv / max(0.001f, vh), 2 * nh * nl / max(0.001f, vh)));
    
    return G;
}
