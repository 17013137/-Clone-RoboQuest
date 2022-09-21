#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{

}

HRESULT CLevel_Logo::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;	

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CSound_Manager::GetInstance()->SoundPlay(L"Radio.ogg", 10, 0.3f);
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	
	POINT m_pt = POINT();
	GetCursorPos(&m_pt);
	ScreenToClient(g_hWnd, &m_pt);

	_bool flag = false;

	if (m_pt.x <= 170.f + 175.f / 2.f && m_pt.x >= 170.f - 175.f / 2.f) {
		if (m_pt.y <= 440.f + 37.5f / 2.f && m_pt.y >= 440.f - 37.5f / 2.f) {
			if (m_Button == false) {
				CSound_Manager::GetInstance()->SoundPlay(L"ButtonIn.ogg", 1, 1.0f);
				m_Button = true;
			}
			if (GetAsyncKeyState(VK_LBUTTON) & 0X8000)
			{
				CSound_Manager::GetInstance()->StopAll();
				CSound_Manager::GetInstance()->SoundPlay(L"Click.ogg", 2, 1.0f);
				if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_GAMEPLAY))))
					return;
			}
		}
		else
			m_Button = false;
	}
	else
		m_Button = false;

	Safe_Release(pGameInstance);
		
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return S_OK;

	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_GameStartBU"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_ExitBU"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX(TEXT("Failed to Created CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
