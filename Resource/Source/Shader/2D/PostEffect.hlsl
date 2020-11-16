#include "2DStanderd.hlsli"

float3 Trans(float3 p)
{
	float range = 10.0f;
	return (fmod(abs(p), range) - range / 2.0f) * sign(p);
}

float SphreDistance(float3 p, float r)
{
	return length(p) - r;
}

float BoxDistnace(float3 p, float3 size)
{
	return length(max(abs(p) - float3(size), 0.0f)) - 0.1f;
}

float PlaneDistance(float3 p, float3 c)
{
	return abs(p.y - c.y);
}

float GetDistnace(float3 p)
{
	float r = 2.0f;
	float sphereA = SphreDistance(Trans(p), r);
	float boxB = BoxDistnace(Trans(p), sqrt(r));
    
	//return max(-sphereA, boxB);
    
	float3 size = float3(2.0f, 2.0f, 2.0f);
	float edge = 0.4f;
    
	float box2 = BoxDistnace(Trans(p), size * float3(edge, edge, 2.0f));
	float box3 = BoxDistnace(Trans(p), size * float3(edge, 2.0f, edge));
	float box4 = BoxDistnace(Trans(p), size * float3(2.0f, edge, edge));
	return max(boxB, -max(sphereA, -min(min(box2, box3), box4)));
    
}

float3 GetNormal(float3 p)
{
	float d = 0.0001;
	return normalize(float3(
    GetDistnace(p + float3(d, 0.0f, 0.0f)) - GetDistnace(p + float3(-d, 0.0f, 0.0f)),
	GetDistnace(p + float3(0.0f, d, 0.0f)) - GetDistnace(p + float3(0.0f, -d, 0.0f)),
	GetDistnace(p + float3(0.0f, 0.0f, d)) - GetDistnace(p + float3(0.0f, 0.0f, -d))));
}

float4 GetMosaicColor(Texture2D tex, float2 uv, float div, float2 aspect)
{
	float fmodNum = 1.0f / div;
	float2 mosaicUV = uv - fmod(uv, float2(fmodNum * aspect.y, fmodNum * aspect.x));
	return tex.Sample(smp, mosaicUV);
}

[RootSignature(RS)]
float4 PS(Output input):SV_TARGET
{
	PixcelInf pinf = pixcelInf[input.instanceID];
	float w, h, level;
	tex[pinf.texIndex].GetDimensions(0, w, h, level);
	float2 aspect = float2(w / h, 1.0f);
	
	float time = utility[0].time;
    
	float max = 6.0f;
	float min = 1.0f;
	float divide = 1.0f;
	float2 uv = fmod(input.uv, 1.0f / divide) * divide;
	float4 texColor = tex[pinf.texIndex].Sample(smp, uv);
    
	float scale = 0.2f;
	float2 debugTexUV = input.uv * (1.0f / scale);
	if (input.uv.x < scale && input.uv.y < scale)
	{
		float d = 1.0f - pow(depthTex[0].Sample(smp, debugTexUV), 100.0f);
		return float4(d, d, d, 1.0f);
	}
	if (input.uv.x < scale && input.uv.y < scale * 2.0f)
	{
		float d = 1.0f - depthTex[1].Sample(smp, debugTexUV);
		return float4(d, d, d, 1.0f);
	}
    
	if (texColor.a <= 0.0)
	{
		float move = time * 10.0f;
		float cMove = 10.0f * time * 3.1415926535f / 180.0f;
		float2 offset = float2(cos(cMove), sin(cMove));
        
		float2 uvpos = uv * float2(2.0f, -2.0f) - float2(1.0f, -1.0f);
	
		float3 light = normalize(float3(1, -1.0f, -1.0f));
	
		
		//move = 0;
		float3 eye = float3(offset, -3 + move);
		float3 tpos = float3(uvpos * aspect, 0 + move);
	
		float3 pos = eye;
		float3 ray = normalize(tpos - eye);
		int tryCnt = 128;
		for (int j = 0; j < tryCnt; j++)
		{
			float len = GetDistnace(pos);
			pos += ray * len;
			if (len < 0.001f)
			{
				float3 normal = GetNormal(pos);
				float lightBright = dot(normal, -light);
				float lim = 1.0f - dot(-ray, normal);
				float3 color = float3(uv, 1);
				float fog = j / float(tryCnt);
                
				float3 refLight = reflect(light, normal);
				float spec = dot(refLight, -ray);
                
				color = saturate(color * lightBright + fog + pow(spec, 5.0f));
				return float4(color, 1);
			}
		}
        
		float b = 1.0f - saturate(length(uvpos - offset) / 2.0f);
		return float4(b / 3.0f, b * 2.0f / 3.0f, b, 1);
	}
	
	float4 color = GetMosaicColor(tex[pinf.texIndex], uv, 100.0f, aspect);
	return float4(color.rgb * pinf.bright, color.a * pinf.alpha);
}