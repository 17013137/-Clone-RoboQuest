#include "..\Public\Bullet.h"
#include "GameInstance.h"
#include "Navigation.h"

CBullet::CBullet(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
	
}

CBullet::CBullet(const CBullet & rhs)
	: CGameObject(rhs.m_pDevice, rhs.m_pDeviceContext)
{
	
}

HRESULT CBullet::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBullet::NativeConstruct(void * pArg, CTransform::TRANSFORMDESC * pTransformDesc)
{
	if (FAILED(__super::NativeConstruct(pArg, pTransformDesc)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;


	BULLETDESC Data;
	memcpy(&Data, pArg, sizeof(BULLETDESC));
	m_Damage = Data.Damage;
	m_CriticalPer = Data.CriticalPer;
	m_Speed = Data.Speed;
	m_Size = Data.Size;

	m_vDir = XMVector3Normalize(Data.vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Data.vPos);

	m_PrevPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	return S_OK;
}

_int CBullet::Tick(_double TimeDelta)
{
	if (__super::Tick(TimeDelta) == 1)
		return 1;

	if (CNavigation::GetInstance()->Sky_Move_OnNavi(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &m_iCurrentCellIndex) == false)
		return 1;

	return 0;
}

void CBullet::LateTick(_double TimeDelta)
{
	if (m_BoundCnt < 0) {
		m_Dead = true;
		return;
	}
	m_PrevPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

HRESULT CBullet::Render()
{
	return S_OK;
}

_bool CBullet::Collide_Object(const _tchar* LayerTag)
{
	_bool flag = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameInstance);
	CGameObject* CollisonObj = pGameInstance->Collison_Sphere_Layer(m_iLevelIndex, this, LayerTag);

	Safe_Release(pGameInstance);
	if (CollisonObj != nullptr) {
		CStatus* EnenmyStatus = (CStatus*)CollisonObj->Get_Component(L"Com_Status");
		EnenmyStatus->Damaged(m_Damage, CStatus::HIT_STATE_DEFAULT, m_Damage);
		m_Dead = true;
		return true;
	}

	return false;
}

_bool CBullet::AABBCollide_Object(const _tchar * LayerTag)
{
	_bool flag = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameInstance);
	CGameObject* CollisonObj = pGameInstance->Collison_AABBLayer(m_iLevelIndex, this, LayerTag);

	Safe_Release(pGameInstance);
	if (CollisonObj != nullptr) {
		CStatus* EnenmyStatus = (CStatus*)CollisonObj->Get_Component(L"Com_Status");
		EnenmyStatus->Damaged(m_Damage, CStatus::HIT_STATE_DEFAULT, m_Damage);
		m_Dead = true;
		return true;
	}

	return false;
}


void CBullet::Free()
{
	__super::Free();
}
