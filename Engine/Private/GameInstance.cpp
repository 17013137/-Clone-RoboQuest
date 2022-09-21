
#include "../Public/GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pImgui_Manager(CImgui_Manager::GetInstance())
	, m_pPicking(CPicking::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pNavigation(CNavigation::GetInstance())
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pSound_Manager(CSound_Manager::GetInstance())

{
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pNavigation);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pImgui_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, CGraphic_Device::GRAPHICDESC GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pInput_Device ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppDeviceOut, ppDeviceContextOut)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->InitDevice(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pLight_Manager->NativeConstruct(*ppDeviceOut, *ppDeviceContextOut)))
		return E_FAIL;

	if (FAILED(m_pFrustum->NativeConstruct()))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pPicking->NativeConstruct(GraphicDesc.hWnd, *ppDeviceOut, *ppDeviceContextOut)))
		return E_FAIL;

	//m_pImgui_Manager->InitWin32(GraphicDesc.hWnd);
	//if (FAILED(m_pImgui_Manager->InitImGui(ppDeviceOut, ppDeviceContextOut)))
	//	return E_FAIL;
	//
	//m_pImgui_Manager->OnOffImgui();

	return S_OK;
}

void CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pInput_Device)
		return;

	// 키 입력 관련 처리
	m_pInput_Device->SetUp_InputDeviceState();

	// ObjectMgr Tick and LateTick
	m_pObject_Manager->Tick(TimeDelta);

	m_pPipeLine->Tick();

	m_pPicking->Update_ToTransform();

	m_pFrustum->Transform_ToWorldSpace();

	m_pObject_Manager->LateTick(TimeDelta);

	// LevelMgr Tick
	m_pLevel_Manager->Tick(TimeDelta);
}

HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pLevel_Manager->Render();

	return S_OK;
}

HRESULT CGameInstance::Render_Begin(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::Render_End()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Clear_LevelResource(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Clear(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

_char CGameInstance::Get_DIKeyState(_uchar byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_char CGameInstance::Get_DIMButtonState(CInput_Device::MOUSEBUTTONSTATE eDIMBState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMButtonState(eDIMBState);
}

_long CGameInstance::Get_DIMMoveState(CInput_Device::MOUSEMOVESTATE eDIMMState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eDIMMState);
}

HRESULT CGameInstance::Add_Timers(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timers(pTimerTag);
}

_double CGameInstance::Compute_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Compute_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->OpenLevel(iLevelIndex, pLevel);
}

void CGameInstance::Set_LevelIndex(_uint iLevelIndex)
{
	if (nullptr == m_pLevel_Manager)
		return;

	m_pLevel_Manager->Set_CurrentLevel(iLevelIndex);
}

_uint CGameInstance::Get_LevelIndex()
{
	if (nullptr == m_pLevel_Manager)
		return 0;

	return m_pLevel_Manager->Get_CurrentLevelIndex();
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

CGameObject * CGameInstance::Add_GameObjToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Add_GameObjToLayer(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

list<class CGameObject*>* CGameInstance::Get_GameObjectList(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObjectList(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Get_PickingObject(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_PickingObject(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Get_CenterRay_CollideObject(_uint iLevelIndex, const _tchar * pLayerTag, _float3 RayPos, _float3 RayDir)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_CenterRay_CollideObject(iLevelIndex, pLayerTag, RayPos, RayDir);
}

_bool CGameInstance::Collison_Sphere_Layer(_uint iLevelIndex, const _tchar * pSour, const _tchar * pDest)
{
	if (nullptr == m_pObject_Manager)
		return false;

	return m_pObject_Manager->Collison_Sphere_Layer(iLevelIndex, pSour, pDest);
}

CGameObject* CGameInstance::Collison_Sphere_Layer(_uint iLevelIndex, CGameObject * Sour, const _tchar * pDest)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Collison_Sphere_Layer(iLevelIndex, Sour, pDest);
}

CGameObject * CGameInstance::Collison_OBBLayer(_uint iLevelIndex, CGameObject * Sour, const _tchar * pDest)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Collison_OBB_Layer(iLevelIndex, Sour, pDest);
}

CGameObject * CGameInstance::Collison_AABBLayer(_uint iLevelIndex, CGameObject * Sour, const _tchar * pDest)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Collison_AABB_Layer(iLevelIndex, Sour, pDest);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

CTransform * CGameInstance::Get_Transform(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Transform(iLevelIndex, pLayerTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eState);
}

_vector CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return XMVectorSet(0.0f, 0.f, 0.f, 1.f);

	return m_pPipeLine->Get_CamPosition();
}

_float4 CGameInstance::Get_CamPositionFloat4()
{
	if (nullptr == m_pPipeLine)
		return _float4(0.0f, 0.f, 0.f, 1.f);

	return m_pPipeLine->Get_CamPositionFloat4();
}


const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Lights(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Lights(pDevice, pDeviceContext, LightDesc);
}

const _float3& CGameInstance::Get_WorldRay()
{
	if (m_pPicking == nullptr)
		return _float3(0.f, 0.f, 0.f);

	return m_pPicking->Get_WorldRay();
}

const _float3& CGameInstance::Get_WorldRayPos()
{
	if (m_pPicking == nullptr)
		return _float3(0.f, 0.f, 0.f);

	return m_pPicking->Get_WorldRayPos();
}

const _float3 & CGameInstance::Get_WorldCenterRay()
{
	if (m_pPicking == nullptr)
		return _float3(0.f, 0.f, 0.f);

	return m_pPicking->Get_WorldCenterRay();
}

HRESULT CGameInstance::Initialize_Navigation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pNavigationData, CTransform * pTransform)
{
	if (nullptr == m_pNavigation)
		return E_FAIL;

	return m_pNavigation->Initialize(pDevice, pDeviceContext, pNavigationData, pTransform);
}

HRESULT CGameInstance::Render_Navigation()
{
	if (nullptr == m_pNavigation)
		return E_FAIL;

#ifdef _DEBUG
	m_pNavigation->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CGameInstance::Render_NavigationCell(_uint iIndex)
{
	if (nullptr == m_pNavigation)
		return E_FAIL;

#ifdef _DEBUG
	m_pNavigation->Render_Cell(iIndex);
#endif // _DEBUG

	return S_OK;
}

_bool CGameInstance::CollideWithNavi(CCollider* pCollider, _fvector vDir, _float3 * Normal)
{
	if (nullptr == m_pNavigation)
		return false;

	return m_pNavigation->CollideWithNavi(pCollider, vDir, Normal);;
}

_bool CGameInstance::isInFrustum_WorldSpace(_vector vWorldPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRange);
}

void CGameInstance::OnOffgui()
{
	if (m_pImgui_Manager == nullptr)
		return;

	m_pImgui_Manager->OnOffImgui();
}


HRESULT CGameInstance::RenderImgui()
{
	if (m_pImgui_Manager == nullptr)
		return E_FAIL;

	return m_pImgui_Manager->Render();
}

ID3D11ShaderResourceView * CGameInstance::Get_RenderTargetSRV(const _tchar * pTargetTag) const
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_SRV(pTargetTag);
}
HRESULT CGameInstance::Add_Sounds()
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Initialize();
}

HRESULT CGameInstance::VolumeUp(_uint iSoundIndex, _float _vol)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->VolumeUp(iSoundIndex, _vol);
}

HRESULT CGameInstance::VolumeDown(_uint iSoundIndex, _float _vol)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->VolumeDown(iSoundIndex, _vol);
}

HRESULT CGameInstance::BGMVolumeUp(_float _vol)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->BGMVolumeUp(_vol);
}

HRESULT CGameInstance::BGMVolumeDown(_float _vol)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->BGMVolumeDown(_vol);
}

HRESULT CGameInstance::Pause(_uint iSoundIndex)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Pause(iSoundIndex);
}

HRESULT CGameInstance::SoundPlay(TCHAR * pSoundKey, _uint iSoundIndex, _float _vol)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->SoundPlay(pSoundKey, iSoundIndex, _vol);
}

HRESULT CGameInstance::PlayBGM(TCHAR * pSoundKey)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->PlayBGM(pSoundKey);
}

HRESULT CGameInstance::StopSound(_uint iSoundIndex)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->StopSound(iSoundIndex);
}

HRESULT CGameInstance::StopAll()
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->StopAll();
}

_uint CGameInstance::Get_ChannelSize()
{
	if (nullptr == m_pSound_Manager)
		return 99;

	return m_pSound_Manager->Get_ChannelSize();
}

void CGameInstance::ReSet_Channel()
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->ReSet_Channel();
}

LRESULT CGameInstance::WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM IParam)
{
	if (m_pImgui_Manager == nullptr)
		return E_FAIL;

	return m_pImgui_Manager->WndProcHandler(hWnd, message, wParam, IParam);
}

void CGameInstance::Release_Engine()
{
	CImgui_Manager::GetInstance()->DestroyInstance();

	CGameInstance::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CTarget_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CTimer_Manager::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CLight_Manager::GetInstance()->DestroyInstance();

	CPicking::GetInstance()->DestroyInstance();

	CFrustum::GetInstance()->DestroyInstance();

	CNavigation::GetInstance()->DestroyInstance();

	CSound_Manager::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();

}

void CGameInstance::Free()
{
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pPicking);
	Safe_Release(m_pImgui_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}
