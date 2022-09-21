#include "..\Public\Layer.h"
#include "GameObject.h"
#include "Picking.h"
#include "Transform.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	if (iIndex >= m_ObjectList.size())
		return nullptr;

	auto	iter = m_ObjectList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_Component(pComponentTag);
}

CTransform * CLayer::Get_Transform(_uint iIndex)
{
	auto iter = m_ObjectList.begin();

	for (_uint i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Get_Transform();
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return S_OK;
}

CGameObject * CLayer::Add_GameObj(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return nullptr;

	m_ObjectList.push_back(pGameObject);

	return pGameObject;
}

void CLayer::Tick(_double TimeDelta)
{
	_int Evnet = 0;

	auto& iter = m_ObjectList.begin();

	for (; iter != m_ObjectList.end(); )
	{
		
		Evnet = (*iter)->Tick(TimeDelta);

		if (1 == Evnet)
		{
			Safe_Release(*iter);
			iter = m_ObjectList.erase(iter);
		}
		else
			++iter;
	}
}

void CLayer::LateTick(_double TimeDelta)
{
	for (auto& pGameObject : m_ObjectList)
		pGameObject->LateTick(TimeDelta);
}

CGameObject* CLayer::Get_PickingObject()
{
	vector<CGameObject*> temp;

	for (CGameObject* iter : m_ObjectList) {
		if (iter->Get_isColideBoxPicking() == true)
			temp.push_back(iter);
	}

	if (temp.size() == 0) {
		return nullptr;
	}

	_float3 RayPos = CPicking::GetInstance()->Get_WorldRayPos();

	CTransform* trans = temp.front()->Get_Transform();
	_float minDist = XMVectorGetX(XMVector3Length(trans->Get_State(CTransform::STATE_POSITION) - XMLoadFloat3(&RayPos)));
	CGameObject* BestObj = temp.front();

	for (auto iter : temp) {
		CTransform* temp_trans = temp.front()->Get_Transform();
		_float Dist = XMVectorGetX(XMVector3Length(temp_trans->Get_State(CTransform::STATE_POSITION) - XMLoadFloat3(&RayPos)));
		if (Dist < minDist) {
			minDist = Dist;
			BestObj = iter;
		}
	}


	return BestObj;
}

CGameObject * CLayer::Get_CrossHair_Near_Object(_float3 RayPos, _float3 RayDir)
{
	vector<CGameObject*> temp;

	for (CGameObject* iter : m_ObjectList) {
		if (iter->Get_isCollideSphere(RayPos, RayDir) == true) {
			if (iter->Get_isDead() == true)
				continue;
			temp.push_back(iter);
		}
	}

	if (temp.size() == 0) {
		return nullptr;
	}
	CTransform* trans = temp.front()->Get_Transform();
	_float minDist = XMVectorGetX(XMVector3Length(trans->Get_State(CTransform::STATE_POSITION) - XMLoadFloat3(&RayPos)));
	CGameObject* BestObj = temp.front();

	for (auto iter : temp) {
		CTransform* temp_trans = iter->Get_Transform();
		_float Dist = XMVectorGetX(XMVector3Length(temp_trans->Get_State(CTransform::STATE_POSITION) - XMLoadFloat3(&RayPos)));
		if (Dist < minDist) {
			minDist = Dist;
			BestObj = iter;
		}
	}

	return BestObj;
}

_uint CLayer::Get_LayerIndex(CGameObject * Object)
{
	_int flag = 0;
	for (auto& iter : m_ObjectList) {
		if (iter == Object) {
			return flag;
		}
		flag++;
	}

	return 99;
}

CLayer * CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_ObjectList)
		Safe_Release(pGameObject);

	m_ObjectList.clear();

}
