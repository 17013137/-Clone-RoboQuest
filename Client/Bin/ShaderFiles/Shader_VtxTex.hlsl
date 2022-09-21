
#include "Shader_Defines.hpp"

cbuffer Matrices
{
	float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
}

cbuffer Sprite {
	float2 XYIndex;  //사진의 x, y갯수
	float Frame;  
	float fAlpha;
	bool OnOff;
	float ImgIndex;
};

cbuffer Gage {
	float Hp;
	float PrevHp;
	float Exp;
	float HitAlpha;
	float AccTime;
};

cbuffer Sprint {
	float red;
	float blue;
	float green;
};

cbuffer Trail{
	float3 MyColor;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD;
	float4		vLocalPos : TEXCOORD1;
};

texture2D			g_Texture;

sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out;

	float4		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV ;
	Out.vLocalPos = vector(In.vPosition, 1.f);

	return Out;
}

VS_OUT VS_SPRITE(VS_IN In)
{
	VS_OUT		Out;

	float4		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	//Out.vTexUV = (In.vTexUV / XYIndex.x) + (Frame / XYIndex.x);
	Out.vTexUV.x = (In.vTexUV.x / XYIndex.x) + (Frame / XYIndex.x);
	Out.vTexUV.y = (In.vTexUV.y / XYIndex.y) + ImgIndex / XYIndex.y;
	
	
	Out.vLocalPos = vector(In.vPosition, 1.f);

	return Out;
}


struct PS_IN
{
	vector			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vLocalPos : TEXCOORD1;
};

struct PS_OUT
{
	vector			vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a = 0.5f;

	return Out;
}

PS_OUT PS_UI(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a == 0.5f) {
		discard;
	}

	return Out;
}

PS_OUT PS_INTRO(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a -= fAlpha;

	if (Out.vColor.a <= 0.5f) {
		Out.vColor.x = 0.0f;
		Out.vColor.y = 0.0f;
		Out.vColor.z = 0.0f;
		Out.vColor.a = 0.8f;
	}

	return Out;
}

PS_OUT PS_FADEINOUT(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);


	if (Out.vColor.a <= 0.3f)
		discard;
	else
		Out.vColor.a = fAlpha;

	return Out;
}

PS_OUT PS_HP(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	if (In.vTexUV.x <= Hp)
		Out.vColor.a = 1.f;
	else if (In.vTexUV.x > Hp && In.vTexUV.x <= PrevHp) {
		Out.vColor.a = 1.f;
		Out.vColor.r = 1.f;
		Out.vColor.g = 0.f;
		Out.vColor.b = 0.f;
	}
	else
		Out.vColor.a = 0.f;

	return Out;
}

PS_OUT PS_EXP(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	
	Out.vColor.x = 0.2f;
	Out.vColor.y = 1.f;
	Out.vColor.z = 1.f;

	if (In.vTexUV.x <= Exp)
		Out.vColor.a = 1.f;
	else {
		Out.vColor.a = 0.f;
		//180 255 255 하늘색

	}
	return Out;
}

PS_OUT PS_EXPBALL(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	float3 WantColor = float3(180.f/255.f, 1.f, 1.f);

	Out.vColor.x *= WantColor.x;
	Out.vColor.y *= WantColor.y;
	Out.vColor.z *= WantColor.z;


	if (Out.vColor.a <= 0.5f) {
		discard;
	}
	else
		Out.vColor.a = 1.0f;

	return Out;
}

PS_OUT PS_MUZZLE_EFFECT(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a = Out.vColor.r;
	
	if (OnOff == false)
		discard;
	else {
		if (Out.vColor.g < 0.45) {
			Out.vColor.r = 1.f;
			Out.vColor.g = 0.498f;
			Out.vColor.b = 0.f;
		}
		else {
			Out.vColor.r = 0.88f;
			Out.vColor.g = 0.88f;
			Out.vColor.b = 0.745f;
		}
	}

	if (Out.vColor.a <= 0.5f)
		discard;

	return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a = Out.vColor.x + Out.vColor.y + Out.vColor.z / 3.f;
	//if (Out.vColor.a <= 0.5f)
	//	discard;
	//else {
	Out.vColor.x = MyColor.x;
	Out.vColor.y = MyColor.y;
	Out.vColor.z = MyColor.z;
		//Out.vColor.a = fAlpha;
	//}

	return Out;
}

PS_OUT PS_BULLET(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.3f)
		discard;
	else {
		float Avg = (Out.vColor.x + Out.vColor.y + Out.vColor.z) / 3.f;

		Out.vColor.x = MyColor.x * Avg;
		Out.vColor.y = MyColor.y * Avg;
		Out.vColor.z = MyColor.z * Avg;
	}

	return Out;
}

PS_OUT PS_PLAYERBULLET(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	float Avg = (Out.vColor.x + Out.vColor.y + Out.vColor.z) / 3.f;

	Out.vColor.a = Avg - 0.5f;

	if (Out.vColor.a <= 0.3f)
		discard;

	Out.vColor.a = fAlpha;

	return Out;
}

PS_OUT PS_SMOKESPRITE(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	float Avg = (Out.vColor.x + Out.vColor.y + Out.vColor.z) / 3.f;

	Out.vColor.a = Avg;

	if (Out.vColor.a <= 0.3f)
		discard;

	return Out;
}

PS_OUT PS_BossHP(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.x = 1.f;
	Out.vColor.y = 0.f;
	Out.vColor.z = 0.f;

	if (Hp < In.vTexUV.x)
		discard;
	
	return Out;
}

PS_OUT PS_SPRINT(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a <= 0.5f) {
		discard;
	}
	
	if (Out.vColor.x >= 0.5f) {
		Out.vColor.a = saturate(red * fAlpha - 0.3f);
	}
	else if (Out.vColor.y >= 0.5f) {
		Out.vColor.a = saturate(blue * fAlpha - 0.3f);
	}
	else if (Out.vColor.z >= 0.5f) {
		Out.vColor.a = saturate(green * fAlpha - 0.3f);
	}
	
	Out.vColor.x = 1.f;
	Out.vColor.y = 1.f;
	Out.vColor.z = 1.f;

	return Out;
}

PS_OUT PS_RECOVERING(PS_IN In)
{
	PS_OUT			Out;

	In.vTexUV.y += AccTime;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	// 0, 137, 255
	float3 WantColor = float3(0.f, 1.f, 130.f / 255.f);

	if (Out.vColor.a <= 0.5f) {
		discard;
	}

	Out.vColor.x = WantColor.x * Out.vColor.a;
	Out.vColor.y = WantColor.y * Out.vColor.a;
	Out.vColor.z = WantColor.z * Out.vColor.a;
	Out.vColor.a = saturate(Out.vColor.a - 0.3f) * fAlpha;

	return Out;
}

PS_OUT PS_SHIELD(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	// 0, 137, 255
	float3 WantColor = float3(1.f, 0.f, 0.f);
	float Avg = Out.vColor.x + Out.vColor.y + Out.vColor.z / 3.f;
	
	Out.vColor.x = WantColor.x;
	Out.vColor.y = WantColor.y;
	Out.vColor.z = WantColor.z;

	Out.vColor.a = Avg * fAlpha;
	
	return Out;
}

PS_OUT PS_BLACK_REMOVE_UI(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	// 0, 137, 255
	float Avg = Out.vColor.x + Out.vColor.y + Out.vColor.z / 3.f;

	Out.vColor.a = Avg;

	return Out;
}

PS_OUT PS_WEAPONICON(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a <= 0.5f) {
		discard;
	}
	
	Out.vColor.a = fAlpha;

	return Out;
}

PS_OUT PS_BOSSLASER(PS_IN In)
{
	PS_OUT			Out;

	In.vTexUV.x -= AccTime;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	float	Avg = (Out.vColor.x + Out.vColor.y + Out.vColor.z)/ 3.f;
	float3 WantColor = float3(1.f, 157.f/255.f, 0.f);

	Out.vColor.x += WantColor.x;
	Out.vColor.y += WantColor.y;
	Out.vColor.z += WantColor.z;


	if (Out.vColor.a <= 0.5f) {
		discard;
	}


	return Out;
}

PS_OUT PS_BULLETCOUNT(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.x <= 0.5f) {
		discard;
	}

	return Out;
}


//BOOL BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;

technique11 DefaultTechnique
{
	/* Pass를 나누는 기준 == 그릴려고하는 리소스에게 어떤 셰이더 세트 효과를 줄것인지?! */

	/* 명암 + 그림자 + 노멀 */
	// 0
	pass DefaultRendering
	{
		SetRasterizerState(RS_Default);		
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
	/*	SetDepthStencilState();
			*/			

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	// 1
	pass UIRendering {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_UI();
	}

	// 2
	pass MuzzleSprite{
		SetRasterizerState(RS_Cull_CW);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_SPRITE();
		PixelShader = compile ps_5_0 PS_MUZZLE_EFFECT();
	}
	// 3
	pass IntroUI {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_INTRO();
	}
	// 4
	pass FadeInOut {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_FADEINOUT();
	}
	// 5
	pass ExpBall {
		SetRasterizerState(RS_Cull_CW);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_EXPBALL();

	}
	// 6
	pass HPRender{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_HP();
	}

	//7
	pass EXPRender {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_EXP();
	}

	//8
	pass Trail {
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_TRAIL();
	}

	//9
	pass Bullet{
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_BULLET();
	}

	//10
	pass PlayerBullet {
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_PLAYERBULLET();
	}

	//11
	pass SmokeSprite{
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_SMOKESPRITE();
	}

	//12
	pass BossHPRender {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_BossHP();
	}

	//13
	pass Sprint{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_SPRINT();
	}

	//14
	pass Recovering{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_RECOVERING();
	}


	//15
	pass Shield{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_SHIELD();
	}
	

	//16
	pass UI_BlackRemove{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_BLACK_REMOVE_UI();
	}

	// 17
	pass Weapon_Icon{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_WEAPONICON();
	}

	// 18
	pass Boss_Laser{
		SetRasterizerState(RS_Cull_NON);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_BOSSLASER();
	}

	//19
	pass BulletCount {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		/*	SetDepthStencilState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_BULLETCOUNT();
	}
}


