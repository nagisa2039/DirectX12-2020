#include "2DStanderd.hlsli"

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
    Texture2D skyTex = tex[pixcelInf[input.instanceID].texIndex];
    
    PixcelInf pinf = pixcelInf[input.instanceID];
    float2 size = float2(1280, 720);
    float2 aspect = float2(size.x / size.y, 1.0f);
    
    float PI = 3.14159265359;
    float t = scene.fov / 2.0f * PI / 180.0f;
    
    float3 tpos = float3(float2(input.pos.xy * size / 2), 0);
		
    float3 cPos = float3(0, 0, -size.y / 2.0f / tan(t));
		
    float3 crossPos = scene.eye/1000.0f + mul(scene.cameraRotate, float4(normalize(tpos - cPos), 0)).rgb;
    float phi = atan2(crossPos.z, crossPos.x);
    float theta = asin(crossPos.y);
		
    float2 sUV = float2(1.f - (phi + PI) / (2.f * PI), 1.0f - (theta + PI / 2.f) / PI);
		
    return skyTex.Sample(smp, sUV);
}