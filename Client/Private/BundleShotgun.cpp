#include "stdafx.h"
#include "..\Public\BundleShotgun.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Bullet.h"
#include "Event_Manager.h"
#include "Weapon_Effect_Manager.h"

CBundleShotgun::CBundleShotgun(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CWeaponObj(pDeviceOut, pDeviceContextOut)
{

}

CBundleShotgun::CBundleShotgun(const CBundleShotgun & rhs)
	: CWeaponObj(rhs)
{
}

HRESULT CBundleShotgun::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBundleShotgun::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_SocketDesc, pArg, sizeof(SOCKETDESC));

	m_pSocketMatrix = m_SocketDesc.pModelCom->Get_CombinedTransformationMatrix(m_SocketDesc.pBoneName);

	m_PivotMatrix = m_SocketDesc.pModelCom->Get_PivotMatrix();

	if (nullptr == m_pSocketMatrix)
		return E_FAIL;

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	


	m_WeaponDesc.fAttackSpeed = 1.f;
	m_WeaponDesc.fCritical_Rate = 0.f;
	m_WeaponDesc.fDamageRange = 0.f;
	m_WeaponDesc.fRebound = 25.0f;
	m_WeaponDesc.fReboundTime = 10.f;
	m_WeaponDesc.fReroadSpeed = 1.0f;
	m_WeaponDesc.fWeaponDamage = 3.f;

	m_Type = (_uint)TYPE_0;

	m_pModelCom->Set_AnimationIndex(1, 99.f, 99.f, false);
	m_pModelCom->Set_FinishFalse(1);

	m_GrowDesc.iLevel = CPlayer_Manager::Get_Instance()->Get_WeaponLevel(1);
	m_GrowDesc.Exp = CPlayer_Manager::Get_Instance()->Get_WeaponExp(1);

	return S_OK;
}

_int CBundleShotgun::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	if (FAILED(Update_WeaponState()))
		return 0;
	Update_WorldState();
	Update_Growing();
	Anim_Tick(TimeDelta);
	m_pModelCom->Update(TimeDelta);

	return 0;
}

void CBundleShotgun::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom && m_isAppear == true && CEvent_Manager::Get_Instance()->Get_Air() == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CBundleShotgun::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 0)))
			return E_FAIL;
	}

	return S_OK;

}

void CBundleShotgun::Anim_Tick(_double TimeDelta)
{
	m_PlayerState = CPlayer_Manager::Get_Instance()->Get_PlayerState();


	//if ((_uint)m_PlayerState > 1)
	//	return;
	//
	//switch (m_PlayerState)
	//{
	//case CPlayer_Manager::PLAYER_REROAD:
	//	if (m_pModelCom->Get_isAnimEnd(0) == true) {
	//		m_pModelCom->Set_FinishFalse(0);
	//	}
	//	else
	//		m_pModelCom->Set_AnimationIndex(0, 1.35, 2.0, false);
	//	break;
	//case CPlayer_Manager::PLAYER_ATTACK:
	//	if (m_pModelCom->Get_isAnimEnd(1) == true) {
	//		m_pModelCom->Set_FinishFalse(1);
	//	}
	//	else
	//		m_pModelCom->Set_AnimationIndex(1, 1.4, 0.5, false);
	//	break;
	//case CPlayer_Manager::PLAYER_END:
	//	break;
	//}

	if (CPlayer_Manager::Get_Instance()->Get_Reroad() == true) {
		if (m_isReroad == false) {
			switch (m_GrowDesc.iLevel)
			{
			case 0:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"TacticalReroad.ogg", 2, 1.f);
				break;
			case 1:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"HuntingReroad.ogg", 2, 1.f);
				break;
			case 2:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"TripleReroad.ogg", 2, 1.f);
				break;
			default:
				break;
			}
			m_isReroad = true;
		}
		m_pModelCom->Set_AnimationIndex(0, 2.0, 1.0, false); //1.6
	}
	else if (CPlayer_Manager::Get_Instance()->Get_Reroad() == false) {
		m_pModelCom->Set_FinishFalse(0);
		m_isReroad = false;
	}

	if (CPlayer_Manager::Get_Instance()->Get_isAttack() == true) {
		m_pModelCom->Set_AnimationIndex(1, 1.5, 2.0, false);
		if (m_isShot == false) {
			switch (m_GrowDesc.iLevel)
			{
			case 0:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"TacticalShoot.ogg", 2, 1.f);
				break;
			case 1:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"HuntingShoot.ogg", 2, 1.f);
				break;
			case 2:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"TripleShoot.ogg", 2, 1.f);
				break;
			default:
				break;
			}
			m_isShot = true;
		}
	}
	else if (CPlayer_Manager::Get_Instance()->Get_isAttack() == false) {
		m_isShot = false;
		m_pModelCom->Set_FinishFalse(1);
	}
}

HRESULT CBundleShotgun::Update_WeaponState()
{
	

	if (CPlayer_Manager::Get_Instance()->Get_WeaponType() == TYPE_SHOTGUN) {

		if (m_Weapons == SHOTGUN0) {
			m_WeaponDesc.fWeaponDamage = 5.f;
			m_WeaponDesc.fCritical_Rate = 10.f;
		}
		if (m_Weapons == SHOTGUN1) {
			m_WeaponDesc.fWeaponDamage = 10.f;
			m_WeaponDesc.fCritical_Rate = 10.f;
		}
		if (m_Weapons == SHOTGUN2) {
			m_WeaponDesc.fWeaponDamage = 15.f;
			m_WeaponDesc.fCritical_Rate = 10.f;
		}

		m_isAppear = true;
		CPlayer_Manager::Get_Instance()->Set_PoseType((POSE_TYPE)m_Type);
		CPlayer_Manager::Get_Instance()->Set_WeaponDesc(m_WeaponDesc);
		CPlayer_Manager::Get_Instance()->Set_Level(m_GrowDesc.iLevel, TYPE_SHOTGUN);
		CPlayer_Manager::Get_Instance()->Set_NowWeaponExp(m_GrowDesc.Exp);

		CPlayer_Manager::Get_Instance()->Set_WeaponLevel(1, m_GrowDesc.iLevel);
		CPlayer_Manager::Get_Instance()->Set_WeaponExp(1, m_GrowDesc.Exp);

		_float FlagExp = CPlayer_Manager::Get_Instance()->Get_Exp();
		if (FlagExp > 0) {
			m_GrowDesc.Exp += FlagExp;
			CPlayer_Manager::Get_Instance()->Set_Exp(-FlagExp);
		}
	}
	else {
		m_isAppear = false;
		return E_FAIL;
	}
	if (m_isAppear == true) {
		
	}

	return S_OK;
}

void CBundleShotgun::Update_Growing()
{
	_bool flag = false;

	if (CKeyMgr::Get_Instance()->Key_Down('Z')) {
		if (m_GrowDesc.iLevel > 0) {
			m_GrowDesc.iLevel--;
			flag = true;
		}
	}
	if (CKeyMgr::Get_Instance()->Key_Down('X')) {
		if (m_GrowDesc.iLevel < 2) {
			m_GrowDesc.iLevel++;
			flag = true;
		}
	}

	switch (m_GrowDesc.iLevel)
	{
	case 0:
		m_pModelCom = m_pTacticalCom;
		m_Weapons = SHOTGUN0;
		m_BulletCnt = 6;

		break;
	case 1:
		m_pModelCom = m_pHuntingCom;
		m_Weapons = SHOTGUN1;
		m_BulletCnt = 10;
		break;
	case 2:
		m_pModelCom = m_pTripleCom;
		m_Weapons = SHOTGUN2;
		m_BulletCnt = 16;
		break;
	default:
		break;
	}

	if (flag == true) {
		m_pModelCom->Set_AnimationIndex(1, 99.f, 99.f, false);
		m_pModelCom->Set_FinishFalse(1);
	}

	_vector mMuzzlePos = m_MuzzlePos[m_GrowDesc.iLevel];
	mMuzzlePos = XMVector3TransformCoord(mMuzzlePos, m_AttachmentMatrix);
	CWeapon_Effect_Manager::Get_Instance()->Set_MuzzlePos(mMuzzlePos);
	CPlayer_Manager::Get_Instance()->Set_BulletCnt(m_BulletCnt);
	CPlayer_Manager::Get_Instance()->Set_Weapon(m_Weapons);
}

HRESULT CBundleShotgun::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	if (FAILED(__super::SetUp_Components(TEXT("Com_Weapon"), LEVEL_STATIC, TEXT("Prototype_Component_Weapon"), (CComponent**)&m_pWeaponCom)))
		return E_FAIL;
	
	if (FAILED(__super::SetUp_Components(TEXT("Com_Hunting"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HuntingShotgun"), (CComponent**)&m_pHuntingCom)))
		return E_FAIL;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Tactical"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TacticalShotgun"), (CComponent**)&m_pTacticalCom)))
		return E_FAIL;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Triple"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TripleShotgun"), (CComponent**)&m_pTripleCom)))
		return E_FAIL;

	m_pModelCom = m_pTacticalCom;

	return S_OK;
}

HRESULT CBundleShotgun::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionFloat4(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBundleShotgun * CBundleShotgun::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBundleShotgun*	pInstance = new CBundleShotgun(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBundleShotgun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBundleShotgun::Clone(void * pArg)
{
	CBundleShotgun*	pInstance = new CBundleShotgun(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBundleShotgun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBundleShotgun::Free()
{
	__super::Free();	
	
	Safe_Release(m_pHuntingCom);
	Safe_Release(m_pTacticalCom);
	Safe_Release(m_pTripleCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
