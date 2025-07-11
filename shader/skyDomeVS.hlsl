#include "common.hlsl"

// Sky Dome Vertex Shader
void main(in VS_IN In, out PS_IN Out)
{
    // カメラ位置を中心にスカイドームを配置
    float4 worldPos = In.Position + float4(CameraPosition.xyz, 0.0f);
    
    Out.WorldPosition = mul(worldPos, World);
    Out.Position = mul(Out.WorldPosition, View);
    Out.Position = mul(Out.Position, Projection);
    
    // 深度を最大値にする（スカイドームは最も遠い）
    // z/w = 1.0 になるようにzをwに設定
    Out.Position.z = Out.Position.w * 0.9999f; // 完全に1.0だと問題が起きる場合があるので少し小さく
    
    Out.Normal = In.Normal;
    Out.Diffuse = In.Diffuse;
    Out.TexCoord = In.TexCoord;
}