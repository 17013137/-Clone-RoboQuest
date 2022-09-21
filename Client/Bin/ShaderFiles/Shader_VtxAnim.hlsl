
#include "Shader_Defines.hpp"

struct BoneMatrix
{
	float4x4		BoneMatrices[128];
};

cbuffer Matrices
{
	float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
	float4x4		g_SocketMatrix;
	BoneMatrix		g_BoneMatrices;
	float4			g_vCamPosition;
	float4x4		g_WeaponWorld;
}

texture2D			g_DisolveTexture;
float Disolve;
cbuffer Temp {
	float fAlpha;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3      vPurePos : TEXCOORD2;
};

texture2D			g_DiffuseTexture;

sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out;

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	float4x4	BoneMatrix =
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;


	float4x4	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

	Out.vPurePos = mul(vPosition , g_WorldMatrix);

	Out.vPosition = mul(vPosition, matWVP);

	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));

	Out.vTexUV = In.vTexUV;

	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	vector			vPosition : SV_POSITION;
	float4			vNormal : NORMAL;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	float3			vPurePos : TEXCOORD2;
};

struct PS_OUT
{
	vector			vDiffuse : SV_TARGET0;
	vector			vNormal : SV_TARGET1;
	vector			vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vDiffuse.a < 0.5f)
		discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.0f, 0.f);

	return Out;
}

PS_OUT PS_WEAPON(PS_IN In)
{
	PS_OUT			Out;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse.x += fAlpha;
	Out.vDiffuse.y += fAlpha;
	Out.vDiffuse.z += fAlpha;

	if (Out.vDiffuse.a < 0.5f)
		discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.0f, 0.f);

	return Out;
}


PS_OUT PS_RIM(PS_IN In)
{
	PS_OUT			Out;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vDiffuse.a < 0.5f)
		discard;

	float3 fDir = normalize(g_vCamPosition - In.vPurePos);
	float rimWidth = 0.5f;

	float RimLightColor = smoothstep(1.0f - rimWidth, 1.0f, 1 - max(0, dot(In.vNormal, fDir)));

	if (dot(fDir, In.vNormal) < 0.5f) {
		Out.vDiffuse.x += 0.f;
		Out.vDiffuse.y += RimLightColor * fAlpha;
		Out.vDiffuse.z += 0.f;

	}

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.0f, 0.f);

	return Out;
}

PS_OUT PS_BLACKWHITE(PS_IN In)
{
	PS_OUT			Out;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	float Avg = Out.vDiffuse.x + Out.vDiffuse.y + Out.vDiffuse.z / 3.f;

	Out.vDiffuse.x = Avg;
	Out.vDiffuse.y = Avg;
	Out.vDiffuse.z = Avg;

	if (Out.vDiffuse.a < 0.5f)
		discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.0f, 0.f);

	return Out;
}

PS_OUT PS_BOSS(PS_IN In)
{
	PS_OUT			Out;
	vector		vDisolve = g_DisolveTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	float DisolveAvg = (vDisolve.x + vDisolve.y + vDisolve.z) / 3.f;

	Out.vDiffuse.x += fAlpha;
	Out.vDiffuse.y += fAlpha;
	Out.vDiffuse.z += fAlpha;

	if (DisolveAvg + Disolve < 1.f)
		discard;
	if (Out.vDiffuse.a < 0.5f)
		discard;
	
	
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.0f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	/* Pass를 나누는 기준 == 그릴려고하는 리소스에게 어떤 셰이더 세트 효과를 줄것인지?! */

	// 0
	/* 명암 + 그림자 + 노멀 */
	pass DefaultRendering
	{		
		SetRasterizerState(RS_Default);		
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
	/*	SetDepthStencilState();
			*/			

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1
	pass RimLight
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_RIM();
	}

	// 2
	pass BlackWhite
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKWHITE();
	}

	// 3
	pass Weapon
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_WEAPON();
	}

	//4
	pass Boss
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BOSS();
	}
}


