#include "stdafx.h"
#include "..\Public\BundleBow.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Bullet.h"
#include "Event_Manager.h"

CBundleBow::CBundleBow(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CWeaponObj(pDeviceOut, pDeviceContextOut)
{

}

CBundleBow::CBundleBow(const CBundleBow & rhs)
	: CWeaponObj(rhs)
{
}

HRESULT CBundleBow::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBundleBow::NativeConstruct(void * pArg)
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
	m_WeaponDesc.fRebound = 5.0f;
	m_WeaponDesc.fReboundTime = 10.f;
	m_WeaponDesc.fReroadSpeed = 1.0f;
	m_WeaponDesc.fWeaponDamage = 10.f;

	m_Type = (_uint)TYPE_1;

	m_pModelCom->Set_AnimationIndex(0, 99.f, 99.f, false);
	m_pModelCom->Set_FinishFalse(0);

	m_GrowDesc.iLevel = CPlayer_Manager::Get_Instance()->Get_WeaponLevel(2);
	m_GrowDesc.Exp = CPlayer_Manager::Get_Instance()->Get_WeaponExp(2);

	return S_OK;
}

_int CBundleBow::Tick(_double TimeDelta)
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

void CBundleBow::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom && m_isAppear == true && CEvent_Manager::Get_Instance()->Get_Air() == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CBundleBow::Render()
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

void CBundleBow::Anim_Tick(_double TimeDelta)
{
	//m_PlayerState = CPlayer_Manager::Get_Instance()->Get_PlayerState();


	//if ((_uint)m_PlayerState > 1)
	//	return;
	//
	//switch (m_PlayerState)
	//{
	//case CPlayer_Manager::PLAYER_ATTACK:
	//	if (m_pModelCom->Get_isAnimEnd(0) == true)
	//		m_pModelCom->Set_FinishFalse(0); 
	//	else
	//		m_pModelCom->Set_AnimationIndex(0, 1.7, 2.0, false);
	//	break;
	//case CPlayer_Manager::PLAYER_REROAD:
	//	break;
	//case CPlayer_Manager::PLAYER_END:
	//	break;
	//}

	if (CPlayer_Manager::Get_Instance()->Get_isAttack() == true) {
		m_pModelCom->Set_AnimationIndex(0, 1.8, 1.0, false);
		if (m_isShot == false) {
			switch (m_GrowDesc.iLevel)
			{
			case 0:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"CrossBowShoot.ogg", 2, 1.f);
				break;
			case 1:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"BowShoot.ogg", 2, 1.f);
				break;
			case 2:
				CSound_Manager::GetInstance()->StopSound(2);
				CSound_Manager::GetInstance()->SoundPlay(L"RocketShoot.ogg", 2, 1.f);
				break;
			default:
				break;
			}
			m_isShot = true;
		}
	}
	else if (CPlayer_Manager::Get_Instance()->Get_isAttack() == false) {
		m_pModelCom->Set_FinishFalse(0);
		m_isShot = false;
	}

}

HRESULT CBundleBow::Update_WeaponState()
{
	if (CPlayer_Manager::Get_Instance()->Get_WeaponType() == TYPE_BOW) {
		m_isAppear = true;
		CPlayer_Manager::Get_Instance()->Set_PoseType((POSE_TYPE)m_Type);
		CPlayer_Manager::Get_Instance()->Set_WeaponDesc(m_WeaponDesc);
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

void CBundleBow::Update_Growing()
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
		m_pModelCom = m_pCrossBowCom;
		m_Type = 0;
		m_Weapons = BOW1;
		break;
	case 1:
		m_pModelCom = m_pLongBowCom;
		m_Type = 1;
		m_Weapons = BOW0;
		break;
	case 2:
		m_pModelCom = m_pRocket;
		m_Type = 1;
		m_Weapons = BOW2;
		break;
	default:
		break;
	}

	if (flag == true) {
		m_pModelCom->Set_AnimationIndex(0, 99.f, 99.f, false);
		m_pModelCom->Set_FinishFalse(0);
	}

	CPlayer_Manager::Get_Instance()->Set_Weapon(m_Weapons);
	CPlayer_Manager::Get_Instance()->Set_WeaponLevel(2, m_GrowDesc.iLevel);
	CPlayer_Manager::Get_Instance()->Set_WeaponExp(2, m_GrowDesc.Exp);

}

HRESULT CBundleBow::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	if (FAILED(__super::SetUp_Components(TEXT("Com_Weapon"), LEVEL_STATIC, TEXT("Prototype_Component_Weapon"), (CComponent**)&m_pWeaponCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_LongBow"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LongBow"), (CComponent**)&m_pLongBowCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_CrossBow"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HeavyCrossBow"), (CComponent**)&m_pCrossBowCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Rocket"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RocketLauncher"), (CComponent**)&m_pRocket)))
		return E_FAIL;

	m_pModelCom = m_pLongBowCom;

	return S_OK;
}

HRESULT CBundleBow::SetUp_ConstantTable()
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

CBundleBow * CBundleBow::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBundleBow*	pInstance = new CBundleBow(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBundleBow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBundleBow::Clone(void * pArg)
{
	CBundleBow*	pInstance = new CBundleBow(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBundleBow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBundleBow::Free()
{
	__super::Free();	
	
	Safe_Release(m_pCrossBowCom);
	Safe_Release(m_pLongBowCom);
	Safe_Release(m_pRocket);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
