#include "2DStanderd.hlsli"

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
	PixcelInf pinf = pixcelInf[input.instanceID];
	float scale = 0.2f;
	float2 uv = input.uv * 1.0f / scale;
	
	if (input.uv.x < scale && input.uv.y < scale)
	{
		float d = depthTex.Sample(smp, uv);
		return float4(d, d, d, 1.0f);
	}
	if (input.uv.x < scale && input.uv.y < scale * 2.0f)
	{
		float d = lightDepthTex.Sample(smp, uv);
		return float4(d, d, d, 1.0f);
	}
	
	float4 texColor = tex[pinf.texIndex].Sample(smp, input.uv);
	return float4(texColor.rgb * pixcelInf[input.instanceID].bright, texColor.a * pixcelInf[input.instanceID].alpha);
}