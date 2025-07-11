#include "common.hlsl"
static const float PI = 3.1415926f;

texture2D       g_Texture : register(t0); // アルベドカラー（拡散反射光）テクスチャ
SamplerState    g_SamplerState : register(s0); // テクスチャサンプラ

float CalculateGeometricDamping(float nh, float nv, float nl, float vh);
float CaluculateDiffuseFromFresnel(float3 N, float3 L, float3 V);
float CaluculateCookTorranceSpecular(float3 L, float3 V, float3 N, float smooth, float metallic);
float CaluculateBeckmann(float m, float t);
float CaluculateFresnel(float f0, float u);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // 法線を計算
    float4 normal = normalize(In.Normal);

    // 各種マップをサンプリングする
    // アルベドカラー（拡散反射光）
    float4 albedoColor = g_Texture.Sample(g_SamplerState, In.TexCoord);

    // 各種マップをサンプリングする
    // スペキュラカラーはアルベドカラーと同じにする
    float3 specColor = albedoColor;

    // 各種マップをサンプリングする
    // 金属度　今回は直接数値で
    float metallic = 0.5f;

    // 各種マップをサンプリングする
    // 滑らかさ　今回は直接数値で
    float smooth = 0.8f;

    // ピクセルからカメラへ向かうベクトル いつも逆
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);

    // ライト 3 つマシマシ
    float3 lig = 0;
    for (int ligNo = 0; ligNo < 3; ligNo++) //ライトが複数あると仮定
    {
        // 光のベクトル
        float3 lv = In.WorldPosition - Light.Position;

        // フレネル反射を考慮した拡散反射率を計算
        float diffuseFromFresnel = CaluculateDiffuseFromFresnel(
                                       normal.xyz, -lv.xyz, eyev);

        // 正規化Lambert拡散反射光を求める
        float nl = saturate(dot(normal.xyz, -lv.xyz));
        float3 light = nl * Light.Diffuse.rgb / PI;

        // 最終的な拡散反射光を計算する
        float3 diffuse = albedoColor.xyz * diffuseFromFresnel * light;

        // 鏡面反射率を計算する
        float3 spec = CaluculateCookTorranceSpecular(
                          -lv.xyz, eyev, normal.xyz, smooth, metallic) * Light.Diffuse;

        // 金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白になるように補間
        // スペキュラカラーの強さを鏡面反射率として扱う
        spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

        // 滑らかさが高ければ、拡散反射は弱くなる
        lig += diffuse * (1.0f - smooth) + spec;
    }

    // 環境光による底上げ
    lig += Light.Ambient.rgb * albedoColor.rgb;

    // テクスチャ取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse *= In.Diffuse;

    // 明るさを合成して出力
    outDiffuse.rgb += lig;
    outDiffuse.a *= albedoColor.a * In.Diffuse.a;
}

// ベックマン分布を計算する
float CaluculateBeckmann(float smooth, float nh)
{
    float D;
    D = (
        exp(
            -(1 - (nh * nh))
            / (smooth * smooth * nh * nh)
        )
        / (4 * smooth * smooth + nh * nh * nh * nh)
    );

    return D;
}

/// フレネルを計算。Schlick近似を使用
float CaluculateFresnel(float f0, float u)
{
    return f0 + (1 - f0) * pow(1 - u, 5);
}

float CaluculateCookTorranceSpecular(float3 L, float3 V, float3 N, float smooth, float metallic)
{
    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L + V);

    // 各種ベクトルがどれぐらい立っているかを内積を利用して求める
    float nh = saturate(dot(N, H));
    float vh = saturate(dot(V, H));
    float nl = saturate(dot(N, L));
    float nv = saturate(dot(N, V));

    // D項をベックマン分布を用いて計算する
    float D = CaluculateBeckmann(metallic, nh);

    // F項をSchlick近似を用いて計算する
    float F = CaluculateFresnel(smooth, vh);

    // G項を求める
    float G = CalculateGeometricDamping(nh, nv, nl, nh);

    // m項を求める
    float m = PI * nv * nh;

    // ここまで求めた値を利用して、Cook-Torranceモデルの鏡面反射を求める
    return max(F * D * G / m, 0.0);
}

float CaluculateDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    // 法線と光源に向かうベクトルがどれだけ似ているかを内積で求める
    float nl = saturate(dot(N, L));

    // 法線と視線に向かうベクトルがどれだけ似ているかを内積で求める
    float nv = saturate(dot(N, V));

    // 法線と光源への方向に依存する拡散反射率と、法線と視線ベクトルに依存する拡散反射率を
    // 乗算して最終的な拡散反射率を求めている。
    return (nl * nv);
}

// 幾何減衰率
float CalculateGeometricDamping(float nh, float nv, float nl, float vh)
{
    return min(1, min(2 * nh * nv / vh, 2 * nh * nl / vh));
}
