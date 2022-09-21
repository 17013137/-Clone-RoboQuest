
#include "Shader_Defines.hpp"


cbuffer Matrices
{
	float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
	float4			g_vCamPosition;
}

cbuffer Reflect {
	float3 g_TargetPos;
};

texture2D			g_DiffuseTexture;
texture2D			g_NormalTexture;
texture2D			g_PatternTex;
texture2D			g_DisolveTex;
float				Disolve;
float				AccTime;
float				Ambient;

sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state {
	filter = min_mag_mip_point;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD;
	float3		vTangent : TANGENT;
};


struct VS_OUT_MODEL
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
	float3      vPurePos : TEXCOORD2;
};

VS_OUT_MODEL VS_MAIN_MODEL(VS_IN In)
{
	VS_OUT_MODEL		Out;

	float4		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vPurePos = vPosition;
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = cross(Out.vNormal.xyz, Out.vTangent);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = vPosition;

	return Out;
}

struct PS_IN_MODEL
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
	float3		vPurePos : TEXCOORD2;
};

struct PS_OUT
{
	vector			vDiffuse : SV_TARGET0;
	vector			vNormal : SV_TARGET1;
	vector			vDepth : SV_TARGET2;	
};

PS_OUT PS_MAIN_MODEL(PS_IN_MODEL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3		vDir = normalize(g_vCamPosition - In.vPurePos);

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

	vNormal = normalize(mul(vNormal, WorldMatrix));
	
	Out.vDiffuse = vMtrlDiffuse;
	
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);


	if (vMtrlDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_WAVE(PS_IN_MODEL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vPattern = g_PatternTex.Sample(DefaultSampler, In.vTexUV + AccTime);
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	
	float		fintensity = vPattern.x + vPattern.y + vPattern.z / 3.f;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

	vNormal = normalize(mul(vNormal, WorldMatrix));

	
	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.x = 1.f;
	Out.vDiffuse.y = saturate(fintensity-0.7f);
	Out.vDiffuse.z = 0.f;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_FLAMECONE(PS_IN_MODEL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vPattern = g_PatternTex.Sample(DefaultSampler, In.vTexUV - AccTime);
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float		fintensity = vPattern.x + vPattern.y + vPattern.z / 3.f;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3		vDir = normalize(g_vCamPosition - In.vPurePos);

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

	vNormal = normalize(mul(vNormal, WorldMatrix));


	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.x = 1.f;
	Out.vDiffuse.y = saturate(fintensity - 0.7f);
	Out.vDiffuse.z = 0.f;
	Out.vDiffuse.a = 1 - vPattern.a;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	
	if (Out.vDiffuse.a <= 0.5f)
		discard;

	return Out;
}

PS_OUT PS_FLAMETRIANGLE(PS_IN_MODEL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vPattern = g_PatternTex.Sample(DefaultSampler, In.vTexUV - AccTime);
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDissolve = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);



	float		fintensity = vPattern.x + vPattern.y + vPattern.z / 3.f;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3		vColor1;
	float3		vColor2;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.x = vDissolve.x * vColor1 + (1 - vDissolve.x) * vColor2;
	Out.vDiffuse.y = d
	Out.vDiffuse.z = 0.f;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_REFLELCT(PS_IN_MODEL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3		vDir = normalize(g_vCamPosition - In.vPurePos);
	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	
	float		Height = (In.vPurePos.y - g_TargetPos.y)/2.f;
	float		Intencity = (1 - ((Height / 5.f)));
	float2 fSize = float2(48.5f, 70.f);
	float3 WantColor = float3(1.f, 0.0f, 0.f);
	Out.vDiffuse = vMtrlDiffuse;

	if (In.vPurePos.x > g_TargetPos.x - fSize.x && In.vPurePos.x < g_TargetPos.x + fSize.x) {
		if (In.vPurePos.z < g_TargetPos.z + fSize.y && In.vPurePos.z > g_TargetPos.z - fSize.y) {
			if (Height > 0 && Height < 5.f) {
				Out.vDiffuse.x = (Out.vDiffuse.x *  (1 - Intencity)) + (WantColor.x * Intencity);
				Out.vDiffuse.y = (Out.vDiffuse.y *  (1 - Intencity)) + (WantColor.y * Intencity);
				Out.vDiffuse.z = (Out.vDiffuse.z *  (1 - Intencity)) + (WantColor.z * Intencity);

			}
		}
	}
	

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_BOSSSIHLED(PS_IN_MODEL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDisolve = g_PatternTex.Sample(DefaultSampler, In.vTexUV);
	vector		vPattern = g_PatternTex.Sample(DefaultSampler, In.vTexUV);
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3		vDir = normalize(g_vCamPosition - In.vPurePos);

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vDiffuse = vMtrlDiffuse;
	float Avg = (Out.vDiffuse.x + Out.vDiffuse.y + Out.vDiffuse.z) / 3.f;
	float DisolveAvg = (vDisolve.x + vDisolve.y + vDisolve.z) / 3.f;

	float3 WantColor = float3(1.f, 0.4f, 0.f);
	
	if (DisolveAvg + Disolve < 1.f)
		discard;
	else {

		if (Avg < 0.3f) {
			Out.vDiffuse.x = saturate(vPattern.x * WantColor.x + Ambient);
			Out.vDiffuse.y = saturate(vPattern.y * WantColor.y + Ambient);
			Out.vDiffuse.z = saturate(vPattern.z * WantColor.z + Ambient);
		}

		float Avg2 = (Out.vDiffuse.x + Out.vDiffuse.y + Out.vDiffuse.z) / 3.f;

		if (Avg2 <= 0.1f) {
			Out.vDiffuse.xyz = WantColor;
			Out.vDiffuse.a = 0.2f;
		}
	}
		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	
	return Out;
}

PS_OUT PS_BOSSOVERHIT(PS_IN_MODEL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV+AccTime);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3		vDir = normalize(g_vCamPosition - In.vPurePos);

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3 WantColor = float3(1.f, 1.f, 0.f);

	Out.vDiffuse = vMtrlDiffuse;
	float Avg = (Out.vDiffuse.x + Out.vDiffuse.y + Out.vDiffuse.z) / 3.f;

	if (Avg <= 0.3f)
		discard;
	else {
		Out.vDiffuse.x = saturate(Out.vDiffuse.x + (WantColor.x));
		Out.vDiffuse.y = saturate(Out.vDiffuse.y + (WantColor.y));
		Out.vDiffuse.z = saturate(Out.vDiffuse.z + (WantColor.z));
	}

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);


	if (vMtrlDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_BOSSOVERHITLASER(PS_IN_MODEL In)
{
	PS_OUT			Out = (PS_OUT)0;
	In.vTexUV.y += AccTime;


	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3		vDir = normalize(g_vCamPosition - In.vPurePos);

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3 WantColor = float3(1.f, 0.8f, 0.1f);

	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.x = saturate(Out.vDiffuse.x * (WantColor.x));
	Out.vDiffuse.y = saturate(Out.vDiffuse.y * (WantColor.y));
	Out.vDiffuse.z = saturate(Out.vDiffuse.z * (WantColor.z));
	Out.vDiffuse.w = 0.8f;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);


	if (vMtrlDiffuse.a < 0.1f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	// 0
	pass Model
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MODEL();
	}

	// 1
	pass Wave
	{
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_WAVE();
	}

	// 2
	pass TargetReflect {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_REFLELCT();
	}

	// 3
	pass FlameCone
	{
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FLAMECONE();
	}

	//4
	pass FlameTriangle
	{
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FLAMETRIANGLE();
	}

	//5
	pass BossShield
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BOSSSIHLED();
	}

	//6
	pass BossOverHit
	{
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BOSSOVERHIT();
	}

	//7
	pass BossOverHitLaser
	{
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BOSSOVERHITLASER();
	}
}


