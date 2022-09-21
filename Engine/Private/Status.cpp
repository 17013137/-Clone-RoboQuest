#include "..\Public\Status.h"
#include "Transform.h"




CStatus::CStatus(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CComponent(pDeviceOut, pDeviceContextOut)
	
{
}

const _float CStatus::Get_StatusInfo(STATUS_INFO iIndex)
{
	// enum STATUS_INFO {INFO_HP, INFO_SHILED, INFO_DAMAGE, INFO_SPEED, INFO_SIKLLCOOLTIME, INFO_STATE, INFO_END};

	_float temp = (float)INFO_END;

	switch (iIndex)
	{
	case Engine::CStatus::INFO_HP:
		temp = m_Hp;
		break;
	case Engine::CStatus::INFO_SHILED:
		temp = m_Shield;
		break;
	case Engine::CStatus::INFO_DAMAGE:
		temp = m_Damage;
		break;
	case Engine::CStatus::INFO_SPEED:
		temp = m_Speed;
		break;
	case Engine::CStatus::INFO_SKILLCOOLTIME:
		temp = m_SkillCooltime;
		break;
	default:
		break;
	}

	return temp;
}

void CStatus::Set_Status(STATUS_INFO iIndex, _float _value)
{
	switch (iIndex)
	{	
	case Engine::CStatus::INFO_HP:
		m_Hp = _value;
		break;
	case Engine::CStatus::INFO_SHILED:
		m_Shield = _value;
		break;
	case Engine::CStatus::INFO_DAMAGE:
		m_Damage = _value;
		break;
	case Engine::CStatus::INFO_SPEED:
		m_Speed = _value;
		break;
	case Engine::CStatus::INFO_SKILLCOOLTIME:
		m_SkillCooltime = _value;
		break;
	case Engine::CStatus::INFO_END:
		break;
	default:
		break;
	}
}

_int CStatus::Status_Tick(_double TimeDelta)
{
	STATUS_STATE flag = STATUS_STATE_DEFAULT;

	if (m_Hp <= 0)
	{
		m_Hp = -1;
		m_State = STATUS_STATE_DEAD;
		return 1;
	}

	if (m_NuckBackDesc.isStart == true) {
		if (m_NuckBackDesc.Duration < m_NuckBackDesc.AccTime) {
			m_NuckBackDesc.isStart = false;
			m_NuckBackDesc.AccTime = 0.0;
		}
		
		m_NuckBackDesc.AccTime += TimeDelta;
		m_DamagedPower /= 2.f;
		return 3;
	}

	if (m_State == STATUS_STATE_DAMAGED) {
		m_State = STATUS_STATE_DEFAULT;
		if (m_HitState == HIT_STATE_KNOCKBACK) {
			m_NuckBackDesc.isStart = true;
		}
		return 2;
	}

	return flag;
}

HRESULT CStatus::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CStatus::NativeConstruct(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	STATUSDESC _pStatus;
	memcpy(&_pStatus, pArg, sizeof(STATUSDESC));

	m_StatusDesc = _pStatus;
	m_Hp = _pStatus.fMaxHp;
	m_Shield = _pStatus.fMaxShield;
	m_Speed = _pStatus.fSpeed;
	return S_OK;
}


_int CStatus::Damaged(_float Damaged, HIT_STATE HitState, _float Power, _fvector* Dir)
{
	if (m_Super == true)
		return 0;

	m_Hp -= Damaged;
	m_DamagedPower = Power;
	m_HitState = HitState;
	if(Dir != nullptr)
		m_KnockbackDir = *Dir;

	m_State = STATUS_STATE_DAMAGED;
	return 0;
}

void CStatus::Heal(_float Heal)
{
	m_Hp += Heal;
	if (m_Hp > m_StatusDesc.fMaxHp) {
		m_Hp = m_StatusDesc.fMaxHp;
	}
}

CStatus * CStatus::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CStatus*	pInstance = new CStatus(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CStatus"));
		Safe_Release(pInstance);
	}
	 
	return pInstance;
}

CComponent * CStatus::Clone(void * pArg)
{
	CStatus*	pInstance = new CStatus(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CStatus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatus::Free()
{
	__super::Free();
}
