#include "2DStanderd.hlsli"

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
    float4 texColor = tex[pixcelInf[input.instanceID].texIndex].Sample(smp, input.uv);
    return float4(texColor.rgb * pixcelInf[input.instanceID].bright, texColor.a * pixcelInf[input.instanceID].alpha);
}