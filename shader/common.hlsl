
cbuffer WorldBuffer : register(b0)
{
    matrix World;
}
cbuffer ViewBuffer : register(b1)
{
    matrix View;
}
cbuffer ProjectionBuffer : register(b2)
{
    matrix Projection;
}

struct VS_IN
{
    float4 Position : POSITION0;
    float4 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION0;
    float4 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    
};

struct LIGHT
{
    bool Enable;
    bool3 Dummy;
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
    
    float4 Position;
    float4 PointLightParam;
    
    float4 Angle;//スポットライトの角度（コーンの角度）
    
    float4 SkyColor; //天球色
    float4 GroundColor;//地面色
    float4 GroundNormal;//地面法線
   
};

cbuffer LightBuffer : register(b4)
{
    LIGHT Light;
}

cbuffer CameraBuffer : register(b5)
{
    float4 CameraPosition;
}

cbuffer ParameterBuffer : register(b6)
{
    float4 Parameter;
}

