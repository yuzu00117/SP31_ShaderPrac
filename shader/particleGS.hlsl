// =============================================================================
// パーティクル ジオメトリシェーダー [particleGS.hlsl]
// 点プリミティブを受け取り、カメラに向いたビルボード（四角形）に展開する
// =============================================================================

cbuffer ViewBuffer : register(b1)
{
    matrix View;
};

cbuffer ProjectionBuffer : register(b2)
{
    matrix Projection;
};

struct GS_IN
{
    float3 Position : POSITION;
    float  Life     : TEXCOORD0;
    float  MaxLife  : TEXCOORD1;
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float  Alpha    : TEXCOORD1;
};

[maxvertexcount(4)]
void main(point GS_IN input[1], inout TriangleStream<PS_IN> stream)
{
    // 寿命切れのパーティクルは描画しない
    if (input[0].Life <= 0.0)
        return;

    // アルファ値（寿命に基づくフェードアウト）
    float alpha = saturate(input[0].Life / input[0].MaxLife);

    // View行列からカメラのRight/Upベクトルを抽出
    float3 right = float3(View[0][0], View[1][0], View[2][0]);
    float3 up    = float3(View[0][1], View[1][1], View[2][1]);

    // ビルボードサイズ（寿命に応じて少し縮小）
    float size = 0.05 * alpha + 0.01;

    // 中心位置
    float3 center = input[0].Position;

    // 4頂点のオフセット（左下、左上、右下、右上 - TriangleStrip用）
    float3 offsets[4] =
    {
        -right * size - up * size, // 左下
        -right * size + up * size, // 左上
         right * size - up * size, // 右下
         right * size + up * size  // 右上
    };

    float2 uvs[4] =
    {
        float2(0.0, 1.0),
        float2(0.0, 0.0),
        float2(1.0, 1.0),
        float2(1.0, 0.0)
    };

    // ViewProjection行列
    matrix vp = mul(View, Projection);

    [unroll]
    for (int i = 0; i < 4; i++)
    {
        PS_IN output;
        float4 worldPos = float4(center + offsets[i], 1.0);
        output.Position = mul(worldPos, vp);
        output.TexCoord = uvs[i];
        output.Alpha    = alpha;
        stream.Append(output);
    }
}
