#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(pComponentTag, iIndex);
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (0 != m_iNumLevels)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype)
		return E_FAIL;

	if (nullptr != Find_Prototype(pPrototypeTag))
		return S_OK;

	m_Prototypes.insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void* pArg)
{
	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

CGameObject * CObject_Manager::Add_GameObjToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return nullptr;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return nullptr;

		m_pLayers[iLevelIndex].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return pGameObject;
}

void CObject_Manager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Tick(TimeDelta);
		}
	}
}

void CObject_Manager::LateTick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->LateTick(TimeDelta);
		}
	}
}

HRESULT CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

CTransform* CObject_Manager::Get_Transform(_uint iLevelIndex, const _tchar * pLayerTag, _uint iNumIndex)
{
	CLayer* temp = Find_Layer(iLevelIndex, pLayerTag);
	if (temp == nullptr)
		return nullptr;

	return temp->Get_Transform(iNumIndex);
}

CGameObject * CObject_Manager::Get_PickingObject(_uint iLevelIndex, const _tchar * pLayerTag)
{
	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;
	
	return pLayer->Get_PickingObject();
}

CGameObject * CObject_Manager::Get_CenterRay_CollideObject(_uint iLevelIndex, const _tchar * pLayerTag, _float3 RayPos, _float3 RayDir)
{
	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_CrossHair_Near_Object(RayPos, RayDir);
}

list<class CGameObject*>* CObject_Manager::Get_GameObjectList(_uint iLevelIndex, const _tchar * pLayerTag)
{
	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ObjectList();
}

_bool CObject_Manager::Collison_Sphere_Layer(_uint iLevelIndex, const _tchar * pSour, const _tchar * pDest)
{
	list<CGameObject*>* SourList = Get_GameObjectList(iLevelIndex, pSour);
	list<CGameObject*>* DestList = Get_GameObjectList(iLevelIndex, pDest);
	CCollider* SourCollider = nullptr;
	CCollider* DestCollider = nullptr;

	_bool flag = false;
	_float Dist = 0.f;

	if (SourList == nullptr || DestList == nullptr)
		return false;

	for (auto& Sour : *SourList) {
		if (Sour->Get_isCulling() == true || Sour->Get_isDead())
			continue;
		SourCollider = (CCollider*)Sour->Get_Component(L"Com_Sphere");
		for (auto& Dest : *DestList) {
			if (Sour == Dest || Dest->Get_isCulling() == true || Dest->Get_isDead())
				continue;
			DestCollider = (CCollider*)Dest->Get_Component(L"Com_Sphere");
			if (true == SourCollider->InterSectSphere(*DestCollider->Get_SphereWorld(), &Dist)) {
				Set_Position(Sour, Dest, Dist);
				flag = true;
			}
		}
	}

	return flag;
}

CGameObject* CObject_Manager::Collison_Sphere_Layer(_uint iLevelIndex, CGameObject* Sour, const _tchar * pDest)
{
	list<CGameObject*>* DestList = Get_GameObjectList(iLevelIndex, pDest);
	CCollider* SourCollider = (CCollider*)Sour->Get_Component(L"Com_Sphere");
	CCollider* DestCollider = nullptr;

	_bool flag = false;
	_float Dist = 0.f;

	if (DestList == nullptr)
		return nullptr;

	for (auto& Dest : *DestList) {
		if (Sour == Dest || Dest->Get_isCulling() == true || Dest->Get_isDead())
			continue;
		DestCollider = (CCollider*)Dest->Get_Component(L"Com_Sphere");
		if (true == SourCollider->InterSectSphere(*DestCollider->Get_SphereWorld(), &Dist)) {
			return Dest;
		}
	}

	return nullptr;
}

CGameObject * CObject_Manager::Collison_OBB_Layer(_uint iLevelIndex, CGameObject * Sour, const _tchar * pDest)
{
	list<CGameObject*>* DestList = Get_GameObjectList(iLevelIndex, pDest);
	CCollider* SourCollider = (CCollider*)Sour->Get_Component(L"Com_Sphere");
	CCollider* DestCollider = nullptr;

	_bool flag = false;
	_float Dist = 0.f;

	if (DestList == nullptr)
		return nullptr;

	for (auto& Dest : *DestList) {
		if (Sour == Dest || Dest->Get_isCulling() == true || Dest->Get_isDead())
			continue;
		DestCollider = (CCollider*)Dest->Get_Component(L"Com_OBB");
		if (true == SourCollider->InterSectOBB(*DestCollider->Get_OBBWorld(), &Dist)) {
			return Dest;
		}
	}

	return nullptr;
}

CGameObject * CObject_Manager::Collison_AABB_Layer(_uint iLevelIndex, CGameObject * Sour, const _tchar * pDest)
{
	list<CGameObject*>* DestList = Get_GameObjectList(iLevelIndex, pDest);
	CCollider* SourCollider = (CCollider*)Sour->Get_Component(L"Com_Sphere");
	CCollider* DestCollider = nullptr;

	_bool flag = false;
	_float Dist = 0.f;

	if (DestList == nullptr)
		return nullptr;

	for (auto& Dest : *DestList) {
		if (Sour == Dest || Dest->Get_isCulling() == true || Dest->Get_isDead())
			continue;
		DestCollider = (CCollider*)Dest->Get_Component(L"Com_AABB");
		if (true == SourCollider->InterSectAABB(*DestCollider->Get_AABBWorld(), &Dist)) {
			return Dest;
		}
	}
	return nullptr;
}

_uint CObject_Manager::Get_LayerIndex(_uint iLevelIndex, const _tchar * pLayerTag, CGameObject * Object)
{
	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return 99;

	return pLayer->Get_LayerIndex(Object);;
}

_uint CObject_Manager::Get_LayerSize(_uint iLevelIndex, const _tchar * pLayerTag)
{
	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return 0;

	return pLayer->Get_LayerSize();
}

void CObject_Manager::Set_Position(CGameObject* Sour, CGameObject* Dest, _float Dist)
{
	CTransform* SourTrans = Sour->Get_Transform();
	CTransform* DestTrans = Dest->Get_Transform();

	_vector vDir = XMVector3Normalize(DestTrans->Get_State(CTransform::STATE_POSITION) - SourTrans->Get_State(CTransform::STATE_POSITION));
	vDir *= Dist;

	SourTrans->Set_State(CTransform::STATE_POSITION, SourTrans->Get_State(CTransform::STATE_POSITION) - XMVectorSet(XMVectorGetX(vDir), 0.f, XMVectorGetX(vDir), 0.f));
}

CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTagFinder(pPrototypeTag));
	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
