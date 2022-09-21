#include "stdafx.h"
#include "..\Public\BossBeetle.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Bullet.h"


CBossBeetle::CBossBeetle(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CBossBeetle::CBossBeetle(const CBossBeetle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossBeetle::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossBeetle::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	m_Player = (CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Player", m_pTransformTag);

	Safe_Release(pGameinstance);
	
	if (m_Player == nullptr)
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(0, 1.0, 1.0, false);

	m_pModelCom->Set_FinishFalse(m_iAnimIndex);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(40.f, 5.f, 20.f, 1.f));
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-100));
	return S_OK;
}

_int CBossBeetle::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	KeyInput();
	Animation_Tick(TimeDelta);
	m_pModelCom->Update(TimeDelta);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	return 0;
}



void CBossBeetle::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

	
}

HRESULT CBossBeetle::Render()
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

#ifdef _DEBUG
//	m_pSphereCom->Render();
	//m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CBossBeetle::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), m_iLevelIndex, TEXT("Prototype_Component_Model_BossBeetle"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CStatus::STATUSDESC temp;
	temp.fMaxHp = 100.f;
	temp.fMaxShield = 50.f;
	temp.fSpeed = 2.f;
	m_Speed = temp.fSpeed;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Status"), LEVEL_STATIC, TEXT("Prototype_Component_Status"), (CComponent**)&m_pStatus, &temp)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_AABB */
	//ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);
	//ColliderDesc.vSize = _float3(0.7f, 1.4f, 0.7f);
	//if (FAILED(__super::SetUp_Components(TEXT("Com_AABB"), m_iLevelIndex, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
	//	return E_FAIL;

	ColliderDesc.vPosition = _float3(0.f, 5.0f, 0.f);
	ColliderDesc.fRadius = 5.0f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBossBeetle::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;	
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBossBeetle::Animation_Tick(_double TimeDelta)
{
	_bool flag = false;
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	if (m_isIntro == true) {		//인트로 애니메이션
		m_iAnimIndex = DEFAULT_INTRO;
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex) == true) {
			m_pModelCom->Set_AnimationIndex(DEFAULT_IDLE, 1.0, 1.0);
			m_isIntro = false;
		}
		else
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
	}
	else if (m_isSkill[SKILL_DESTRUCTPLATE] == true) {
		m_iAnimIndex = DEFAULT_END;
		switch (m_Progression[SKILL_DESTRUCTPLATE])
		{
		case 0:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + DESTRUCTPLATE_PRESHOOT) == true) {
				m_Progression[SKILL_DESTRUCTPLATE]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + DESTRUCTPLATE_SHOOT);
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + DESTRUCTPLATE_PRESHOOT, 1.0, 1.0, false);
			break;
		case 1:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + DESTRUCTPLATE_SHOOT) == true) {
				m_Progression[SKILL_DESTRUCTPLATE]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + DESTRUCTPLATE_PRESHOOT);
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + DESTRUCTPLATE_SHOOT, 1.0, 1.0, false);
			break;
		case 2:
			if (m_Destructplate_cnt == m_Destructplate_rand) {
				m_isSkill[SKILL_DESTRUCTPLATE] = false;
				m_Progression[SKILL_DESTRUCTPLATE] = 0;
				m_Destructplate_cnt = 0;
				break;
			}
			m_pTransformCom->MyAirLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.0f, TimeDelta);
			m_DestructTime += TimeDelta;
			if (m_DestructTime > 1.0) {
				m_Progression[SKILL_DESTRUCTPLATE] = 0;
				m_Destructplate_cnt++;
				m_DestructTime = 0.0;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + DESTRUCTPLATE_PRESHOOT, 1.0, 1.0, false);
			break;
		default:
			break;
		}

	}
	else {
		switch (m_State)
		{
		case Client::CBossBeetle::DEFAULT_IDLE:
			m_pModelCom->Set_AnimationIndex(DEFAULT_IDLE, 1.0, 2.0);
			m_pTransformCom->MyAirLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.0f, TimeDelta);
			break;
		default:
			break;
		}
	}
	Safe_Release(pGameinstance);

}

void CBossBeetle::KeyInput()
{
	_bool flag = false;

	if (CKeyMgr::Get_Instance()->Key_Down(VK_NUMPAD5)) {
		m_isSkill[SKILL_DESTRUCTPLATE] = true;
		flag = true;
		m_Destructplate_rand = rand() % 3;
		for (_uint i = DEFAULT_END; i < DESTRUCTPLATE_END + DEFAULT_END; i++)
			m_pModelCom->Set_FinishFalse(i);
	}


	if (flag == false) {
		m_State = DEFAULT_IDLE;
	}
}


CBossBeetle * CBossBeetle::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBossBeetle*	pInstance = new CBossBeetle(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBossBeetle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossBeetle::Clone(void * pArg)
{
	CBossBeetle*	pInstance = new CBossBeetle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBossBeetle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossBeetle::Free()
{
	__super::Free();	
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pStatus);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
