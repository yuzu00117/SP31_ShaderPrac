//// DisneyCommon.hlsli
//#ifndef _DISNEY_COMMON_
//#define _DISNEY_COMMON_

//#define PI 3.14159265359

//cbuffer CameraCB : register(b0)
//{
//    float4x4 gViewProj;
//    float3 gCamPos;
//    float _pad0;
//}
//cbuffer ObjectCB : register(b1)
//{
//    float4x4 gWorld;
//}
//cbuffer DisneyMatCB : register(b3) // ← ステップ3で定義した 11パラメータ
//{
//    float3 baseColor;
//    float subsurface;
//    float metallic;
//    float specular;
//    float specularTint;
//    float roughness;
//    float anisotropic;
//    float sheen;
//    float sheenTint;
//    float clearcoat;
//    float clearcoatGloss;
//    float _pad1;
//}

//struct VS_INPUT
//{
//    float3 Pos : POSITION;
//    float3 Normal : NORMAL;
//    float2 Tex : TEXCOORD0;
//    float4 Tangent : TANGENT; // xyz: tangent, w: handedness
//};

//struct VS_OUTPUT
//{
//    float4 Pos : SV_POSITION;
//    float3 WorldPos : TEXCOORD0;
//    float3 Normal : TEXCOORD1;
//    float2 Tex : TEXCOORD2;
//    float3 Tangent : TEXCOORD3;
//    float3 Bitan : TEXCOORD4;
//};

//#endif
