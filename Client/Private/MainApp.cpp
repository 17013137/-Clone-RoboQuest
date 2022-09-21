#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player_Manager.h"
#include "Event_Manager.h"
#include "Weapon_Effect_Manager.h"

#include "BackGround.h"
#include "LoadingLogo.h"
#include "LoadingBar.h"

#include "Level_Logo.h"
#include "Level_Loading.h"
#include "GameStartBU.h"
#include "ExitBU.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);


	// D3D11_DEPTH_STENCIL_DESC
	// D3D11_BLEND_DESC



	/*D3D11_RASTERIZER_DESC
	D3D11_DEPTH_STENCIL_DESC
	D3D11_BLEND_DESC*/

	//m_pDeviceContext->OMSetBlendState(0xffffffff);


	/*
	m_pDeviceContext->OMSetDepthStencilState();
	m_pDeviceContext->RasterizerSTate();*/
}

HRESULT CMainApp::NativeConstruct()
{
	/*D3D11_SAMPLER_DESC*/

	CGraphic_Device::GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(CGraphic_Device::GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinCX = g_iWinCX;
	GraphicDesc.iWinCY = g_iWinCY;
	GraphicDesc.eWinMode = CGraphic_Device::MODE_WIN;

	/* 장치초기화 등. 엔진의 기능을 활용하기위한 기초 초기화 작업을 마친다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	m_pGameInstance->Add_Sounds();

	

	GetWindowRect(g_hWnd, &m_Rect);
	m_Rect.left += 10.f;
	m_Rect.right -= 10.f;
	m_Rect.top += 10.f;
	m_Rect.bottom -= 10.f;
	return S_OK;
}

void CMainApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	if (m_Clip == true) {
		ClipCursor(&m_Rect);
	}
	else
		ClipCursor(NULL);

	if (CKeyMgr::Get_Instance()->Key_Down(VK_END)) {
		m_Clip = m_Clip == true ? false : true;
		if (m_Clip == true) {
			ShowCursor(false);
		}
		else
			ShowCursor(true);
	}


	m_pGameInstance->Tick_Engine(TimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Render_Begin(_float4(0.0f, 0.f, 0.f, 1.f));

	if (FAILED(m_pRenderer->Render_GameObjects()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Engine()))
		return E_FAIL;

	//m_pGameInstance->RenderImgui();
	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CLevel*			pLevel = nullptr;

	switch (eLevelIndex)
	{
	case LEVEL_LOGO:
		pLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
		break;

	default:
		pLevel = CLevel_Loading::Create(m_pDevice, m_pDeviceContext, (LEVEL)eLevelIndex);
		break;
	}

	if (FAILED(m_pGameInstance->Open_Level(eLevelIndex, pLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingLogo"),
		CLoadingLogo::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingBar"),
		CLoadingBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GameStartBU"),
		CGameStartBU::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExitBU"),
		CExitBU::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"), CMonster::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), CVIBuffer_Trail::Create(20, m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail50"), CVIBuffer_Trail::Create(50, m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	/* 각종 컴포넌트들 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Status"), CStatus::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Weapon"),
		CWeapons::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxAnim*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIMMODEL_DECLARATION::Elements, VTXNONANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE_DECLARATION::Elements, VTXCUBE_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance.hlsl"), VTXINSTANCE_DECLARATION::Elements, VTXINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), CVIBuffer_Rect2::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Spray"),
		CVIBuffer_Point_Spray::Create(m_pDevice, m_pDeviceContext, 200))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Combine"),
		CVIBuffer_Point_Combine::Create(m_pDevice, m_pDeviceContext, 50))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainMenu"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Menu/Menu.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingLogo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Loading/Logo(%d).dds"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GameStart"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Menu/GameStart/GameStart(%d).dds"), 2))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Exit"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Menu/Exit/Exit(%d).dds"), 2))))
		return E_FAIL;

	return S_OK;
}

//HRESULT CMainApp::Ready_Gara()
//{
//	_ulong		dwByte = 0;
//
//	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
//	if (0 == hFile)
//		return E_FAIL;
//
//	_float3		vPoint[3];
//
//	vPoint[0] = _float3(0.f, 0.f, 5.f);
//	vPoint[1] = _float3(5.f, 0.f, 0.f);
//	vPoint[2] = _float3(0.f, 0.f, 0.f);
//	WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	vPoint[0] = _float3(0.f, 0.f, 5.f);
//	vPoint[1] = _float3(5.f, 0.f, 5.f);
//	vPoint[2] = _float3(5.f, 0.f, 0.f);
//	WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	vPoint[0] = _float3(0.f, 0.f, 10.f);
//	vPoint[1] = _float3(5.f, 0.f, 5.f);
//	vPoint[2] = _float3(0.f, 0.f, 5.f);
//	WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	vPoint[0] = _float3(5.f, 0.f, 5.f);
//	vPoint[1] = _float3(10.f, 0.f, 0.f);
//	vPoint[2] = _float3(5.f, 0.f, 0.f);
//	WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	CloseHandle(hFile);
//
//
//
//	return S_OK;
//}


CMainApp * Client::CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX(TEXT("Failed to Created CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::CMainApp::Free()
{
	CWeapon_Effect_Manager::Get_Instance()->Destroy_Instance();
	CPlayer_Manager::Get_Instance()->Destroy_Instance();
	CKeyMgr::Get_Instance()->Destroy_Instance();
	CEvent_Manager::Get_Instance()->Destroy_Instance();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}

