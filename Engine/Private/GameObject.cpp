#include "..\Public\GameObject.h"

#include "GameInstance.h"
#include "Level_Manager.h"
#include "Collider.h"
#include "Frustum.h"
#include "Picking.h"
#include "PIpeLine.h"
#include "Navigation.h"

const _tchar*	CGameObject::m_pTransformTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice), m_pDeviceContext(rhs.m_pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void * pArg, CTransform::TRANSFORMDESC* pTransformDesc)
{
	if (nullptr != Find_Component(m_pTransformTag))
		return E_FAIL;

	m_pTransformCom = CTransform::Create(m_pDevice, m_pDeviceContext, pTransformDesc);

	m_Components.insert(COMPONENTS::value_type(m_pTransformTag, m_pTransformCom));

	m_iLevelIndex = CLevel_Manager::GetInstance()->Get_CurrentLevelIndex();

	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

_int CGameObject::Tick(_double TimeDelta)
{
	if (m_Dead == true)
		return 1;

	return 0;
}

void CGameObject::LateTick(_double TimeDelta)
{
	_bool isinCamera = CFrustum::GetInstance()->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f);

	if (isinCamera == false)
		m_Culling = true;
	else
		m_Culling = false;

	if (m_Culling == true)
		return;

}

HRESULT CGameObject::Render()
{
	return S_OK;
}

_bool CGameObject::SkyMoveOnNavi()
{
	_bool flag = false;

	CNavigation* Navi = GET_INSTANCE(CNavigation);

	flag = Navi->Sky_Move_OnNavi(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &m_iCurrentCellIndex);

	RELEASE_INSTANCE(CNavigation);

	return flag;
}

void CGameObject::Create_ExpBall()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"ExpBall", L"Prototype_GameObject_ExpBall", &m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameinstance);
}

void CGameObject::Create_Smoke(_fvector vPos)
{
	_vector Pos = vPos;

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Smoke", L"Prototype_Effect_SmokeSprite", &Pos);

	Safe_Release(pGameinstance);
}

_bool CGameObject::Collide_Object(const _tchar * LayerTag)
{
	_bool flag = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameInstance);
	CGameObject* CollisonObj = pGameInstance->Collison_Sphere_Layer(m_iLevelIndex, this, LayerTag);

	Safe_Release(pGameInstance);
	if (CollisonObj != nullptr) {
		return true;
	}

	return false;
}

_uint CGameObject::Get_LayerSize(const _tchar * pLayerTag)
{
	return CObject_Manager::GetInstance()->Get_LayerSize(m_iLevelIndex, pLayerTag);
}

void CGameObject::Set_LayerIndex(const _tchar* pLayerTag)
{
	m_LayerIndex = CObject_Manager::GetInstance()->Get_LayerIndex(m_iLevelIndex, pLayerTag, this);
}

void CGameObject::LookAtCameara()
{
	CPipeLine* pPipeline = GET_INSTANCE(CPipeLine);

	_vector CameraPos = pPipeline->Get_CamPosition();

	RELEASE_INSTANCE(CPipeLine);

	m_pTransformCom->LookAt(CameraPos);
}

HRESULT CGameObject::Search_MyNaviIndex()
{
	_int flag = 0;

	CNavigation* pNavi = GET_INSTANCE(CNavigation);

	flag = pNavi->Search_MyIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	RELEASE_INSTANCE(CNavigation);

	if (flag == -1)
		return E_FAIL;
	else
		m_iCurrentCellIndex = flag;

	return S_OK;
}

HRESULT CGameObject::SetUp_Components(const _tchar* pComponentTag, _uint iLevelIndex, const _tchar* pPrototypeTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	CComponent*	pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.insert(COMPONENTS::value_type(pComponentTag, pComponent));

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

_bool CGameObject::Get_isColideBoxPicking()
{
	CCollider* Collider = (CCollider*)Get_Component(L"Com_AABB");
	_vector vPos;

	if (Collider == nullptr)
		return false;

	vPos = Collider->InterSectsRay(CPicking::GetInstance()->Get_WorldRayPos(), CPicking::GetInstance()->Get_WorldRay());
	if (XMVectorGetX(XMVector3Length(vPos)) > 0.f)
		return true;

	return false;
}

_bool CGameObject::Get_isCollideSphere(_float3 RayPos, _float3 RayDir)
{
	CCollider* Collider = (CCollider*)Get_Component(L"Com_Sphere");
	_vector vPos;

	if (Collider == nullptr)
		return false;

	vPos = Collider->InterSectsRay(RayPos, RayDir);
	if (XMVectorGetX(XMVector3Length(vPos)) > 0.f)
		return true;

	return false;
}

void CGameObject::Set_CameraDistance()
{
	_vector CameraPos = CPipeLine::GetInstance()->Get_CamPosition();
	_vector MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_CameraDistance = XMVectorGetX(XMVector3Length(CameraPos - MyPos));
}

void CGameObject::ZBillBoard()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);

	_float3 fScail = m_pTransformCom->Get_Scale();

	_matrix CameraWorld = ((CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Camera", m_pTransformTag))->Get_WorldMatrix();
	_matrix MyMtx = _matrix();
	MyMtx = XMMatrixIdentity();

	MyMtx.r[0] = XMVectorSetX(MyMtx.r[0], XMVectorGetX(CameraWorld.r[0]));
	MyMtx.r[0] = XMVectorSetY(MyMtx.r[0], XMVectorGetY(CameraWorld.r[0]));
	MyMtx.r[1] = XMVectorSetX(MyMtx.r[1], XMVectorGetX(CameraWorld.r[1]));
	MyMtx.r[1] = XMVectorSetY(MyMtx.r[1], XMVectorGetY(CameraWorld.r[1]));
	MyMtx = XMMatrixInverse(nullptr, MyMtx);

	//_float4x4 fMyMtx;
	//XMStoreFloat4x4(&fMyMtx, MyMtx);
	//m_pTransformCom->Set_WorldMTX(fMyMtx);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(MyMtx.r[0]) * fScail.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(MyMtx.r[1]) * fScail.y);
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(MyMtx.r[2]) * fScail.z);

	RELEASE_INSTANCE(CGameInstance);
}

void CGameObject::Free()
{
	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
