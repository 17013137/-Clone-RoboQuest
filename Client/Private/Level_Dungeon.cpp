#include "stdafx.h"
#include "..\Public\Level_Dungeon.h"
#include "GameInstance.h"
#include "Camera_Default.h"
#include "BundleRifle.h"
#include "Level_Loading.h"

CLevel_Dungeon::CLevel_Dungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{

}

HRESULT CLevel_Dungeon::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;	

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Enemy(TEXT("Layer_Enemy"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Weapon(TEXT("Layer_Weapon"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Dungeon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
		
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);
	CSound_Manager::GetInstance()->SoundPlay(L"Enviment2.ogg", 10, 0.3f);

	if (GetKeyState('G') & 0x8000)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_BOSSTOWER))))
			return;
	}

	Safe_Release(pGameInstance);

}

HRESULT CLevel_Dungeon::Render()
{
	if (FAILED(__super::Render()))
		return S_OK;

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Lights()
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

HRESULT CLevel_Dungeon::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 10.f, -15.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinCX / g_iWinCY;
	CameraDesc.pPlayerTrans = (CTransform*)pGameInstance->Get_Component(LEVEL_DUNGEON, L"Layer_Player", L"Com_Transform");

	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Camera_Default"), &CameraDesc)))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Camera_First"), &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, L"Layer_HealBot", TEXT("Prototype_GameObject_HealBot"), &XMVectorSet(205.f, 3.92f, 249.0f, XMConvertToRadians(111.f)))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, L"Layer_HealBot", TEXT("Prototype_GameObject_HealBot"), &XMVectorSet(151, 3.92f, 354, XMConvertToRadians(208.5f)))))
		return E_FAIL;

	/*
	
	245.71f, 3.92f, 243.83f, XMConvertToRadians(62.f));
	posInfo[2] = XMVectorSet(194.15f, 3.92f, 246.92f
	
	*/

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Enemy(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_SmallBot"))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_RocketPawn"))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MortarPod"))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MecanoBot"))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MinePod"))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_LaserFly"))))
	//	return E_FAIL;

	//Stage2
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MecanoBot"), &_float3(152.91f, 0.f, 157.8f))))
		return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_SmallBot"), &_float3(144.f, 0.f, 171.4f))))
	//	return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MecanoBot"), &_float3(131.538f, 0.f, 171.434f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MecanoBot"), &_float3(120.574f, 0.f, 171.434f))))
		return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_SmallBot"), &_float3(114.247f, 0.f, 160.053f))))
	//	return E_FAIL;
	
	//Stgae3
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_RocketPawn"), &_float3(212.247f, 4.32738f, 195.65f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_RocketPawn"), &_float3(229.801f, 4.32738f, 202.192f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_RocketPawn"), &_float3(221.271f, 4.32738f, 215.282f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_RocketPawn"), &_float3(233.269f, 4.32738f, 226.194f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_RocketPawn"), &_float3(216.226f, 4.32738f, 234.06f))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MinePod"), &_float3(118.f, 5.f, 281.f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MinePod"), &_float3(113.5f, 5.f, 300.f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MinePod"), &_float3(158.4f, 3.88f, 288.5f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_MinePod"), &_float3(131.f, 3.88f, 245.7f))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_JetFly"), &_float3(131.f, 0.f, 321.75f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_JetFly"), &_float3(126.f, 0.f, 311.f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_JetFly"), &_float3(145.f, 0.f, 314.f))))
		return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_JetFly"), &_float3(134.f, 0.f, 307.f))))
	//	return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_JetFly"), &_float3(146.f, 0.f, 303.f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_JetFly"), &_float3(134.f, 0.f, 295.f))))
		return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_JetFly"), &_float3(150.f, 0.f, 328.f))))
	//	return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_JetFly"), &_float3(139.f, 0.f, 284.f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_JetFly"), &_float3(143.f, 0.f, 273.f))))
		return E_FAIL;





	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Weapon(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CBundleRifle::SOCKETDESC			SocketDesc;
	ZeroMemory(&SocketDesc, sizeof(CBundleRifle::SOCKETDESC));

	SocketDesc.pModelCom = (CModel*)pGameInstance->Get_Component(LEVEL_DUNGEON, L"Layer_Player", TEXT("Com_Model"));
	SocketDesc.pBoneName = "Weapon_Attachement";
	SocketDesc.pBoneName2 = "Weapon_AttachementDual";
	SocketDesc.pPlayerTransCom = (CTransform*)pGameInstance->Get_Component(LEVEL_DUNGEON, L"Layer_Player", TEXT("Com_Transform"));
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_AssultM4"), &SocketDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_HuntingShotgun"), &SocketDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_LongBow"), &SocketDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, L"Stage", TEXT("Prototype_GameObject_Stage1"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, L"Stage", TEXT("Prototype_GameObject_Stage2"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, L"Stage", TEXT("Prototype_GameObject_Stage3"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, L"Stage", TEXT("Prototype_GameObject_Stage4"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, L"Stage", TEXT("Prototype_GameObject_Stage5"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, L"MagmaTile", TEXT("Prototype_GameObject_MagmaTile"))))
		return E_FAIL;

	_float4 BoardPos[6];
	BoardPos[0] = _float4(166.104f, -6.13915f, 253.956f, 22.5f);
	BoardPos[1] = _float4(152.269f, -6.07484f, 260.603f, 41.6864f);
	BoardPos[2] = _float4(141.273f, -6.21946f, 273.613f, 79.152f);
	BoardPos[3] = _float4(136.911f, -3.19163f, 291.116f, 98.4846f);
	BoardPos[4] = _float4(137.066f, -6.13915f, 308.438f, 109.248f);
	BoardPos[5] = _float4(144.334f, -6.21946f, 322.492f, 113.812f);

	for (_int i = 0; i < 6; i++) {
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, L"SpringBoard", TEXT("Prototype_GameObject_SpringBoard"), &BoardPos[i])))
			return E_FAIL;
	}

	_vector posInfo[5];
	posInfo[0] = XMVectorSet(139.82f, 0.F, 133.07f, 0.f);
	posInfo[1] = XMVectorSet(245.71f, 3.92f, 243.83f, XMConvertToRadians(62.f));
	posInfo[2] = XMVectorSet(194.15f, 3.92f, 246.92f, XMConvertToRadians(291.f));
	posInfo[3] = XMVectorSet(160.93f, 4.f, 358.9f, XMConvertToRadians(208.5f));
	posInfo[4] = XMVectorSet(378.76f, 0.f, 269.25f, XMConvertToRadians(260.f));


	for (int i = 0; i < 3; i++) {
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_DoorStage"), &posInfo[i])))
			return E_FAIL;
	}
	
	for (int i = 3; i < 5; i++) {
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_DoorBoss"), &posInfo[i])))
			return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_Effect_Muzzle"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;


	
}

HRESULT CLevel_Dungeon::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* CrossHair */
	_float4 UISize = _float4(g_iWinCX*0.5f, g_iWinCY*0.5f + 10.f, 3.f, 10.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_CrossHairRect"), &UISize)))
		return E_FAIL;
	UISize = _float4(g_iWinCX*0.5f, g_iWinCY*0.5f - 10.f, 3.f, 10.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_CrossHairRect"), &UISize)))
		return E_FAIL;
	UISize = _float4(g_iWinCX*0.5f + 10.f, g_iWinCY*0.5f, 3.f, 10.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_CrossHairRect"), &UISize)))
		return E_FAIL;
	UISize = _float4(g_iWinCX*0.5f - 10.f, g_iWinCY*0.5f, 3.f, 10.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_CrossHairRect"), &UISize)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_PlayerIcon"), &UISize)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_PlayerBackGround"), &UISize)))
		return E_FAIL;



	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_ExpBarBackGround"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_ExpBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_HealthBarBackGround"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_HealthBar"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_Hit"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_FadeOut"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_Spirnt"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_Shield"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_RifleIcon"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_ShotgunIcon"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_BowIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_BulletBackground"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_BulletCount"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_UI_BulletTenCount"))))
		return E_FAIL;
	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Dungeon * CLevel_Dungeon::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CLevel_Dungeon*	pInstance = new CLevel_Dungeon(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX(TEXT("Failed to Created CLevel_Dungeon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Dungeon::Free()
{
	__super::Free();


}
