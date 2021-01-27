#include "2DStanderd.hlsli"

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
    Texture2D skyTex = tex[pixcelInf[input.instanceID].texIndex];
    
    float2 size = float2(1280, 720);
    float3 tpos = float3(float2(input.pos.rg * size / 2), 0);
		
    float3 cPos = float3(0, 0, -size.x / 2);
		
    float3 crossPos = mul(scene.cameraRotate, float4(normalize(tpos - cPos), 0)).rgb;
    float phi = atan2(crossPos.x, crossPos.z);
    float theta = asin(crossPos.y);
		
    float PI = 3.14159265359;
    float2 sUV = float2(1.f - (phi + PI) / (2.f * PI), 1.0f - (theta + PI / 2.f) / PI);
		
    return skyTex.Sample(smp, sUV);
}