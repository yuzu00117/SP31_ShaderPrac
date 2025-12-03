#include "common.hlsl"

// 1パス目：シルエットだけを書き出すピクセルシェーダ
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // ここで書き出した値が「シャドウマップの濃さ」になります。
    // PDF で指定されている値があればそれに合わせてください。
    // ひとまず「真っ黒」を出す例：
    outDiffuse = float4(0, 0, 0, 1);
}
