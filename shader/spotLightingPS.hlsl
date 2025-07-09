#include "common.hlsl"
Texture2D g_Texture : register(t0); // テクスチャ０番
SamplerState g_SamplerState : register(s0); // サンプラー０番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
//光源からピクセルへのベクトル
    float3 lv = normalize(In.WorldPosition.xyz - Light.Position.xyz);

//光源計算
    float4 normal = normalize(In.Normal);
    float light = -dot(normal.xyz, lv);

//距離による減衰
    float3 ld = length(In.WorldPosition.xyz - Light.Position.xyz);
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    ofs = saturate(ofs);
    light *= ofs;

// 視線ベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

//反射ベクトル
    float3 refv = reflect(Light.Direction.xyz, normal.xyz);
    refv = normalize(refv);

//スペキュラの計算
    float specular = -dot(eyev, refv);
    specular = saturate(specular);
    specular = pow(specular, 30.0f);

//コーンの向きとlvの角度
    float angle = acos(dot(lv.xyz, normalize(Light.Direction.xyz)));

//angleがコーンの中にどのくらい入っているか？による明るさ値（スポットライトでの減衰値）
    float spot = 1.0f - 1.0f / Light.Angle.x * abs(angle);

// 明るさの値にテクスチャと頂点色とアンビエントを合成
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= Light.Diffuse.rgb * In.Diffuse.rgb * light * spot + Light.Ambient.rgb;
    outDiffuse.rgb += (specular * spot);

    outDiffuse.a *= In.Diffuse.a;

}
