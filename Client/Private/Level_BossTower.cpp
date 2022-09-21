#include "stdafx.h"
#include "..\Public\Level_BossTower.h"

#include "GameInstance.h"
#include "Camera_Default.h"
#include "Sword.h"
#include "BundleRifle.h"
#include "Level_Loading.h"
#include "Event_Manager.h"

CLevel_BossTower::CLevel_BossTower(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{

}

HRESULT CLevel_BossTower::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;	

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Enemy(TEXT("Layer_Enemy"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Weapon(TEXT("Layer_Weapon"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	CSound_Manager::GetInstance()->StopAll();

	CEvent_Manager::Get_Instance()->Set_EventType(0);
	CEvent_Manager::Get_Instance()->Set_Air(true);

	return S_OK;
}

void CLevel_BossTower::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
		
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	CSound_Manager::GetInstance()->SoundPlay(L"BossBGM.MP3", 10, 0.3f);

	//if (GetKeyState('G') & 0x8000)
	//{
	//	if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_BOSSDISCO))))
	//		return;
	//}

	Safe_Release(pGameInstance);

}

HRESULT CLevel_BossTower::Render()
{
	if (FAILED(__super::Render()))
		return S_OK;


	SetWindowText(g_hWnd, TEXT("BossStage1."));

	return S_OK;
}

HRESULT CLevel_BossTower::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 0.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_BossTower::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 10.f, -15.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.0f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinCX / g_iWinCY;
	CameraDesc.pPlayerTrans = (CTransform*)pGameInstance->Get_Component(LEVEL_BOSSTOWER, L"Layer_Player", L"Com_Transform");

	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_Camera_Default"), &CameraDesc)))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_Camera_First"), &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_BossTower::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_BossTower::Ready_Layer_Enemy(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_BossTower"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, L"BossTowerSocle", TEXT("Prototype_GameObject_TowerSocle"))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_BossTower::Ready_Layer_Weapon(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CBundleRifle::SOCKETDESC			SocketDesc;
	ZeroMemory(&SocketDesc, sizeof(CBundleRifle::SOCKETDESC));

	SocketDesc.pModelCom = (CModel*)pGameInstance->Get_Component(LEVEL_BOSSTOWER, L"Layer_Player", TEXT("Com_Model"));
	SocketDesc.pBoneName = "Weapon_Attachement";
	SocketDesc.pBoneName2 = "Weapon_AttachementDual";
	SocketDesc.pPlayerTransCom = (CTransform*)pGameInstance->Get_Component(LEVEL_BOSSTOWER, L"Layer_Player", TEXT("Com_Transform"));
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_AssultM4"), &SocketDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_HuntingShotgun"), &SocketDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_LongBow"), &SocketDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_BossTower::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, L"BaseCamp", TEXT("Prototype_GameObject_BaseCamp"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_BossTower::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_Effect_Muzzle"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;

	return S_OK;


	
}

HRESULT CLevel_BossTower::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* CrossHair */
	_float4 UISize = _float4(g_iWinCX*0.5f, g_iWinCY*0.5f + 10.f, 3.f, 10.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_CrossHairRect"), &UISize)))
		return E_FAIL;
	UISize = _float4(g_iWinCX*0.5f, g_iWinCY*0.5f - 10.f, 3.f, 10.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_CrossHairRect"), &UISize)))
		return E_FAIL;
	UISize = _float4(g_iWinCX*0.5f + 10.f, g_iWinCY*0.5f, 3.f, 10.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_CrossHairRect"), &UISize)))
		return E_FAIL;
	UISize = _float4(g_iWinCX*0.5f - 10.f, g_iWinCY*0.5f, 3.f, 10.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_CrossHairRect"), &UISize)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_PlayerIcon"), &UISize)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_PlayerBackGround"), &UISize)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_ExpBarBackGround"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_ExpBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_HealthBarBackGround"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_HealthBar"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_Hit"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_BossHpBarBackground"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_BossHpBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_GameObject_FadeOut"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_Spirnt"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_Shield"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_RifleIcon"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_ShotgunIcon"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_BowIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_BulletBackground"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_BulletCount"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_BOSSTOWER, pLayerTag, TEXT("Prototype_UI_BulletTenCount"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_BossTower * CLevel_BossTower::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CLevel_BossTower*	pInstance = new CLevel_BossTower(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX(TEXT("Failed to Created CLevel_BossTower"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_BossTower::Free()
{
	__super::Free();


}
