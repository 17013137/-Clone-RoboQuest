
cbuffer Matrices
{
	float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
	float4x4		g_ViewMatrixInv, g_ProjMatrixInv;
	float g_WinCX, g_WinCY;
}

cbuffer LightDesc
{
	float4		g_vLightDir;
	float4		g_vLightPos;
	float		g_fRange;
	float4		g_vLightDiffuse;
	float4		g_vLightAmbient;
	float4		g_vLightSpecular;
}

cbuffer MaterialDesc
{
	float4		g_vMtrlAmbient = (float4)1.f;
	float4		g_vMtrlSpecular = (float4)1.f;
	float		g_fPower = 30.0f;
}

cbuffer CameraDesc
{
	float4		g_vCamPosition;
}

texture2D		g_Texture;
texture2D		g_NormalTexture;
texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_DepthTexture;
texture2D		g_SpecularTexture;

sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD;
};

struct VS_OUT_LINE
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD;
	float2		vCenter : TEXCOORD1;
	float2		vLeft : TEXCOORD2;
	float2		vRight : TEXCOORD3;
	float2		vUp : TEXCOORD4;
	float2		vDown : TEXCOORD5;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out;

	float4		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_LINE VS_MAIN_LINE(VS_IN In)
{
	VS_OUT_LINE		Out;

	float4		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	float texSize = 1.0f / g_WinCX;

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;
	Out.vCenter = In.vTexUV;
	Out.vLeft = saturate(In.vTexUV + float2(texSize * -1.0f, 0.0f));
	Out.vRight = saturate(In.vTexUV + float2(texSize * 1.0f, 0.0f));
	Out.vUp = saturate(In.vTexUV + float2(0.0f, texSize * 1.0f));
	Out.vDown = saturate(In.vTexUV + float2(0.f, texSize * -1.0f));
	return Out;
}

struct PS_IN
{
	vector			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;	

};

struct PS_IN_LINE
{
	vector			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
	float2			vCenter : TEXCOORD1;
	float2			vLeft : TEXCOORD2;
	float2			vRight : TEXCOORD3;
	float2			vUp : TEXCOORD4;
	float2			vDown : TEXCOORD5;
};


struct PS_OUT
{
	vector			vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);	

	return Out;
}

struct PS_OUT_LIGHT
{
	vector			vShade : SV_TARGET0;
	vector			vSpecular : SV_TARGET1;
};

PS_OUT PS_MAIN_OUTLINE(PS_IN_LINE In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector			vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	vector			vNorCenter = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vNorLeft = g_NormalTexture.Sample(DefaultSampler, In.vLeft);
	vector			vNorRight = g_NormalTexture.Sample(DefaultSampler, In.vRight);
	vector			vNorTop = g_NormalTexture.Sample(DefaultSampler, In.vUp);
	vector			vNorBottom = g_NormalTexture.Sample(DefaultSampler, In.vDown);
	
	float3 dW = vNorLeft.rgb + vNorRight.rgb - 2 * vNorCenter.rgb;
	float3 dH = vNorTop.rgb + vNorBottom.rgb - 2 * vNorCenter.rgb;
	float lW = length(dW);
	float lH = length(dH);


	float fShade = (smoothstep(0.2, 0.8, vShade.x));
	fShade = smoothstep(0.4, 0.6, fShade);
	
	fShade = saturate(fShade+0.3f);

	vShade.x = fShade;
	vShade.y = fShade;
	vShade.z = fShade;

	Out.vColor = vDiffuse * (vShade);

	if (0.2 < sqrt(lW*lW + lH*lH)) {
		Out.vColor.x = 0.f;
		Out.vColor.y = 0.f;
		Out.vColor.z = 0.f;
	}

	if (0.0f == Out.vColor.a)
		discard;

	return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector			vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float			fViewZ = vDepthDesc.y * 1000.f;

	vector			vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal) + ((ceil(g_vLightAmbient * g_vMtrlAmbient)*3.f) / 3.f) - 0.3f);

	Out.vShade.a = 1.f;

	vector			vReflect = reflect(normalize(g_vLightDir), vNormal);
		
	vector			vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * fViewZ;

	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector			vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular);

	return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector			vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float			fViewZ = vDepthDesc.y * 1000.f;

	vector			vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector			vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * fViewZ;

	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector			vLightDir = vWorldPos - g_vLightPos;
	float			fDistance = length(vLightDir);

	float			fAtt = max((g_fRange - fDistance) / g_fRange, 0.f);

	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	Out.vShade.a = 1.f;

	vector			vReflect = reflect(normalize(vLightDir), vNormal);	

	vector			vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular) * fAtt;

	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector			vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	

	Out.vColor = vDiffuse * vShade + vSpecular;

	if (0.0f == Out.vColor.a)
		discard;

	return Out;
}





RasterizerState	RS_Default
{
	FillMode = Solid;
	FrontCounterClockwise = false;
	CullMode = back;
};


BlendState BS_NonBlend
{
	BlendEnable[0] = false;
	BlendEnable[1] = false;
};

BlendState BS_LightBlend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = one;
	DestBlend = one;
	Blendop = add;

	/*BlendEnable[0] = true;
	SrcBlend[0] = one;
	DestBlend[0] = one;
	Blendop[0] = add;*/

	//BlendEnable[1] = true;
	//SrcBlend[1] = one;
	//DestBlend[1] = one;
	//Blendop[1] = add;
	
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_NonZTestAndWrite
{
	DepthEnable = false;
	DepthWriteMask = zero;
};


technique11 DefaultTechnique
{
	//0
	pass Debug_RenderTarget
	{		
		SetRasterizerState(RS_Default);		
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	//1
	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
	}

	//2
	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_LightBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
	}

	//3
	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	//4
	pass OutLine
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_LINE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
	}

}


