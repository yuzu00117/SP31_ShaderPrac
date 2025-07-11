#include "common.hlsl"

Texture2D       g_Texture : register(t0);
SamplerState    g_SamplerState : register(s0);

float CalucateGeometricDamping(float nh, float nv, float nl, float vh);
float CalucateDiffuseFromFresnel(float N, float L, float V);
float CalucateCookTorranceSpecular(float L, float V, float N, float smooth, float metallic);
float CalucateBeckmann(float m, float t);
float CalucateFresnel(float f0, float u);

static const float PI = 3.1415926f;