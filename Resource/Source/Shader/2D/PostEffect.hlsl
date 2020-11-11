#include "2DStanderd.hlsli"

float3 Trans(float3 p)
{
	float range = 3.0f;
    return fmod(abs(p), range * 2.0f) - range;
}

float SphreDistance(float3 p, float3 c, float r)
{
	return length(Trans(p) - c) - r;
}

float PlaneDistance(float3 p, float3 c)
{
	return abs(p.y - c.y);
}

[RootSignature(RS)]
float4 PS(Output input):SV_TARGET
{
    PixcelInf pinf = pixcelInf[input.instanceID];
	float w, h, level;
    tex[pinf.texIndex].GetDimensions(0, w, h, level);
	
	float divide = 3.0f;
	float2 uv = fmod(input.uv, 1.0f / divide) * divide;
    float4 texColor = tex[pinf.texIndex].Sample(smp, uv);
    
    float scale = 0.2f;
    float2 debugTexUV = input.uv * (1.0f / scale);
    if (input.uv.x < scale && input.uv.y < scale)
    {
        float d = 1.0f - depthTex.Sample(smp, debugTexUV);
        return float4(d, d, d, 1.0f);
    }
    if (input.uv.x < scale && input.uv.y < scale * 2.0f)
    {
        float d = 1.0f - lightDepthTex.Sample(smp, debugTexUV);
        return float4(d, d, d, 1.0f);
    }
    
    if (texColor.a <= 0.0)
    {
        float2 uvpos = uv * float2(2.0f, -2.0f) - float2(1.0f, -1.0f);
	
        float3 light = normalize(float3(1.0f, -1.0f, 1.0f));
	
        float2 aspect = float2(w / h, 1.0f);
        float3 eye = float3(0, 0, -2);
        float3 tpos = float3(uvpos * aspect, 0);
	
        float3 sphCenter = float3(0, 0, 0);
        float rsph = 0.5f;
	
        float3 planeCenter = float3(0, 0, 0);
	
        float3 ray = normalize(tpos - eye);
        int tryCnt = 128;
        for (int j = 0; j < tryCnt; j++)
        {
            float len = 100000.0f;
            len = min(SphreDistance(eye, sphCenter, rsph), len);
		    //len = min(PlaneDistance(eye, planeCenter), len);
		
            eye += ray * len;
            if (len < 0.001f)
            {
			//float lightBright = dot(normalize(eye - sphCenter), -light);
                float3 color = float3(uv, 1);
                float bright = (float(tryCnt - j) / float(tryCnt));
                color = color * bright;
                return float4(color, 1);
            }
        }
    }
	
    return float4(texColor.rgb * pinf.bright, texColor.a * pinf.alpha);
}