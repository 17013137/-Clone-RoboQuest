#include "stdafx.h"
#include "..\Public\BoomBot.h"
#include "GameInstance.h"
#include "Bullet.h"


CBoomBot::CBoomBot(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CBoomBot::CBoomBot(const CBoomBot & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBoomBot::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoomBot::NativeConstruct(void * pArg)
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

	m_pModelCom->Set_AnimationIndex(0, 1.0, 1.0);

	m_pModelCom->Set_FinishFalse(m_iAnimIndex);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(170.f, 0.f, 170.f, 1.f));
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 2.f);

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_Speed = 0.5f;
	return S_OK;
}

_int CBoomBot::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Status_Tick(TimeDelta);

	if (m_Dead == true)
		return 1;

	State_Tick(TimeDelta);
	Animation_Tick(TimeDelta);
	m_pModelCom->Update(TimeDelta);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	return 0;
}



void CBoomBot::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CBoomBot::Render()
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
#endif // _DEBUG

	return S_OK;
}

HRESULT CBoomBot::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), m_iLevelIndex, TEXT("Prototype_Component_Model_BoomBot"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CStatus::STATUSDESC temp;
	temp.fMaxHp = 30.f;
	temp.fMaxShield = 0.f;
	temp.fSpeed = 2.f;
	m_Speed = temp.fSpeed;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Status"), LEVEL_STATIC, TEXT("Prototype_Component_Status"), (CComponent**)&m_pStatus, &temp)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	ColliderDesc.fRadius = 0.1f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBoomBot::SetUp_ConstantTable()
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

void CBoomBot::State_Tick(_double TimeDelta)
{
	_float Dist = fabs(XMVectorGetX(XMVector3Length(m_Player->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))));
	_bool flag = false;

	if (Dist < 20.f) {
		m_LockOn = true;
	}

	if (m_Damaged == true) {


	}
	else if (m_LockOn == true) {
		if (m_AnimDesc.isStart == false) {
			if (FAILED(m_pTransformCom->MyLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.5f, TimeDelta))) {
				m_AnimDesc.isStart = true;
				m_randomMove = rand() % 5 + 1;
			}

			m_iAnimIndex = ANIM_IDLE;
		}
	}
}

void CBoomBot::Animation_Tick(_double TimeDelta)
{
	switch (m_iAnimIndex)
	{
	case Client::CBoomBot::ANIM_IDLE:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CBoomBot::ANIM_PRESHOOT:
		if (m_pModelCom->Get_isAnimEnd(ANIM_PRESHOOT)) {
			m_pModelCom->Set_FinishFalse(ANIM_SHOOT);
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.f, 1.f, false);
		break;
	case Client::CBoomBot::ANIM_RUN_FRONT:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CBoomBot::ANIM_SHOOT:
		if (m_pModelCom->Get_isAnimEnd(ANIM_SHOOT)) {
			m_pModelCom->Set_FinishFalse(ANIM_PRESHOOT);
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.f, 1.f, false);
		break;
	case Client::CBoomBot::ANIM_STUN_LOOP:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		m_pModelCom->Set_FinishFalse(ANIM_STUN_START);
		break;
	case Client::CBoomBot::ANIM_STUN_START:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.f, 1.f, false);
		break;
	default:
		break;
	}


}

_int CBoomBot::Status_Tick(_double TimeDelta)
{
	int flag = m_pStatus->Status_Tick(TimeDelta);
	if (flag == 1)
		m_Dead = true;

	if (flag == 2) {
		m_Damaged = true;
	}

	return _int();
}

void CBoomBot::Set_Bullet()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	_matrix temp = XMMatrixMultiply(XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("R_Canon_Scale01")), XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix()));
	temp = XMMatrixMultiply(temp, m_pTransformCom->Get_WorldMatrix());
	_float4x4 temp2;
	XMStoreFloat4x4(&temp2, temp);

	CBullet::BulletDesc BulletDesc;
	BulletDesc.vPos = XMLoadFloat3(&_float3(temp2.m[3][0], temp2.m[3][1], temp2.m[3][2]));
	BulletDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	BulletDesc.Damage = 10.f;
	BulletDesc.CriticalPer = 0.f;

	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"HitBox", L"Prototype_GameObject_EnemyBullet", &BulletDesc);

	Safe_Release(pGameinstance);
}


CBoomBot * CBoomBot::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBoomBot*	pInstance = new CBoomBot(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBoomBot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoomBot::Clone(void * pArg)
{
	CBoomBot*	pInstance = new CBoomBot(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBoomBot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoomBot::Free()
{
	__super::Free();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pStatus);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
