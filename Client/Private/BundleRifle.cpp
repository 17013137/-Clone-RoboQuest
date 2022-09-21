#include "stdafx.h"
#include "..\Public\BundleRifle.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Event_Manager.h"
#include "Weapon_Effect_Manager.h"

CBundleRifle::CBundleRifle(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CWeaponObj(pDeviceOut, pDeviceContextOut)
{
}

CBundleRifle::CBundleRifle(const CBundleRifle & rhs)
	: CWeaponObj(rhs)
{
}

HRESULT CBundleRifle::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBundleRifle::NativeConstruct(void * pArg)
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

	
	m_WeaponDesc.fAttackSpeed = 3.f;
	m_WeaponDesc.fCritical_Rate = 0.f;
	m_WeaponDesc.fDamageRange = 5.f;
	m_WeaponDesc.fRebound = 1.0f;
	m_WeaponDesc.fReboundTime = 1.f;
	m_WeaponDesc.fReroadSpeed = 1.0f;
	m_WeaponDesc.fWeaponDamage = 10.f;


	m_Type = (_uint)TYPE_0;

	m_pModelCom->Set_AnimationIndex(1, 99.f, 99.f, false);
	m_pModelCom->Set_FinishFalse(1);
	m_GrowDesc.iLevel = CPlayer_Manager::Get_Instance()->Get_WeaponLevel(0);
	m_GrowDesc.Exp = CPlayer_Manager::Get_Instance()->Get_WeaponExp(0);
	m_GrowDesc.Exp = CPlayer_Manager::Get_Instance()->Get_NowWeaponExp();
	return S_OK;
}

_int CBundleRifle::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	if (FAILED(Update_WeaponState()))
		return 0;
	Update_WorldState();
	Update_Growing();
	Anim_Tick(TimeDelta);
	Sound_Update();
	m_pModelCom->Update(TimeDelta);
	return 0;
}

void CBundleRifle::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom && m_isAppear == true && CEvent_Manager::Get_Instance()->Get_Air() == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CBundleRifle::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 3)))
			return E_FAIL;
	}

	return S_OK;

}

void CBundleRifle::Anim_Tick(_double TimeDelta)
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
	//		m_pModelCom->Set_AnimationIndex(0, 1.40, 10.0, false);
	//	break;
	//case CPlayer_Manager::PLAYER_ATTACK:
	//	if (m_pModelCom->Get_isAnimEnd(1) == true) {
	//		m_pModelCom->Set_FinishFalse(1);
	//	}
	//	else
	//		m_pModelCom->Set_AnimationIndex(1, 6.0, 1.0, false);
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
				CSound_Manager::GetInstance()->SoundPlay(L"M4Reroad.ogg", 2, 1.f);
				break;
			case 1:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"AssultReroad.ogg", 2, 1.f);
				break;
			case 2:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"LMGReroad.ogg", 2, 1.f);
				break;
			default:
				break;
			}
			m_isReroad = true;
			
		}
		m_pModelCom->Set_AnimationIndex(0, 1.40, 10.0, false);
	}
	else if (CPlayer_Manager::Get_Instance()->Get_Reroad() == false) {
		m_pModelCom->Set_FinishFalse(0);
		m_isReroad = false;
	}

	if (CPlayer_Manager::Get_Instance()->Get_isAttack() == true) {
		m_pModelCom->Set_AnimationIndex(1, 6, 1.0, false);
		if (m_isShot == false) {
			switch (m_GrowDesc.iLevel)
			{
			case 0:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"M4Shoot.ogg", 2, 1.f);
				break;
			case 1:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"AssultShoot.ogg", 2, 1.f);
				break;
			case 2:
				
				CSound_Manager::GetInstance()->SoundPlay(L"LMGShoot.ogg", 2, 1.f);
				break;
			default:
				break;
			}
			m_isShot = true;
		}
	}
	else if (CPlayer_Manager::Get_Instance()->Get_isAttack() == false) {
		if (m_GrowDesc.iLevel == 2) {
			CSound_Manager::GetInstance()->StopSound(2);
		}

		m_isShot = false;
		m_pModelCom->Set_FinishFalse(1);
	}
}

HRESULT CBundleRifle::Update_WeaponState()
{
	if (CPlayer_Manager::Get_Instance()->Get_WeaponType() == TYPE_RIFLE) {
		m_isAppear = true;

		if (m_Weapons == RIFLE1) {
			m_WeaponDesc.fWeaponDamage = 10.f;
		}
		if (m_Weapons == RIFLE2) {
			m_WeaponDesc.fWeaponDamage = 15.f;
		}

		CPlayer_Manager::Get_Instance()->Set_PoseType((POSE_TYPE)m_Type);
		CPlayer_Manager::Get_Instance()->Set_WeaponDesc(m_WeaponDesc);
		CPlayer_Manager::Get_Instance()->Set_Level(m_GrowDesc.iLevel, TYPE_RIFLE);
		CPlayer_Manager::Get_Instance()->Set_NowWeaponExp(m_GrowDesc.Exp);
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

	return S_OK;
}

void CBundleRifle::Update_Growing()
{
	_bool flag = false;

	if (CKeyMgr::Get_Instance()->Key_Down('Z')) {
		m_GrowDesc.iLevel--;
		flag = true;
	}
	if (CKeyMgr::Get_Instance()->Key_Down('X')) {
		m_GrowDesc.iLevel++;
		flag = true;
	}

	switch (m_GrowDesc.iLevel)
	{
	case 0:
		m_pModelCom = m_pM4Com;
		m_Weapons = RIFLE0;
		break;
	case 1:
		m_pModelCom = m_pRifleCom;
		m_Weapons = RIFLE1;
		break;
	case 2:
		m_pModelCom = m_pLMGCom;
		m_Weapons = RIFLE2;
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
	CPlayer_Manager::Get_Instance()->Set_Weapon(m_Weapons);
	CPlayer_Manager::Get_Instance()->Set_WeaponLevel(0, m_GrowDesc.iLevel);
	CPlayer_Manager::Get_Instance()->Set_WeaponExp(0, m_GrowDesc.Exp);
}

void CBundleRifle::Sound_Update()
{
	
}

//void CBundleRifle::Bone_Update()
//{
//	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
//	Safe_AddRef(pGameinstance);
//	
//	CGameObject* Effect = pGameinstance->Add_GameObjToLayer(m_iLevelIndex, L"MuzzleEffect", L"Prototype_Effect_Muzzle");
//	if (Effect != nullptr) {
//		CTransform* Effecttrans = (CTransform*)Effect->Get_Component(m_pTransformTag);
//		
//		_vector mMuzzlePos = m_MuzzlePos[m_GrowDesc.iLevel];
//		
//		mMuzzlePos = XMVector3TransformCoord(mMuzzlePos, m_AttachmentMatrix);
//	
//		Effecttrans->Set_State(CTransform::STATE_POSITION, mMuzzlePos);
//	}
//	
//	Safe_Release(pGameinstance);
//
//}


HRESULT CBundleRifle::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	if (FAILED(__super::SetUp_Components(TEXT("Com_M4"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AssultM4"), (CComponent**)&m_pM4Com)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Rifle"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rifle"), (CComponent**)&m_pRifleCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_LMG"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LMG"), (CComponent**)&m_pLMGCom)))
		return E_FAIL;
	
	if (FAILED(__super::SetUp_Components(TEXT("Com_Weapon"), LEVEL_STATIC, TEXT("Prototype_Component_Weapon"), (CComponent**)&m_pWeaponCom)))
		return E_FAIL;

	m_pModelCom = m_pM4Com;

	return S_OK;
}

HRESULT CBundleRifle::SetUp_ConstantTable()
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

CBundleRifle * CBundleRifle::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBundleRifle*	pInstance = new CBundleRifle(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBundleRifle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBundleRifle::Clone(void * pArg)
{
	CBundleRifle*	pInstance = new CBundleRifle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBundleRifle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBundleRifle::Free()
{
	__super::Free();	
	
	Safe_Release(m_pWeaponCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pM4Com);
	Safe_Release(m_pRifleCom);
	Safe_Release(m_pLMGCom);
	Safe_Release(m_pRendererCom);

}
