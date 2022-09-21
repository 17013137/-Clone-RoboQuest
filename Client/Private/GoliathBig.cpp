#include "stdafx.h"
#include "..\Public\GoliathBig.h"
#include "GameInstance.h"
#include "Bullet.h"


CGoliathBig::CGoliathBig(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CGoliathBig::CGoliathBig(const CGoliathBig & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGoliathBig::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoliathBig::NativeConstruct(void * pArg)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(170.f, 0.f, 175.f, 1.f));
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 2.f);

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_Speed = 1.0f;

	return S_OK;
}

_int CGoliathBig::Tick(_double TimeDelta)
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



void CGoliathBig::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom && m_Culling == true)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CGoliathBig::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

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
	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CGoliathBig::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), m_iLevelIndex, TEXT("Prototype_Component_Model_RocketPawn"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CStatus::STATUSDESC temp;
	temp.fMaxHp = 100.f;
	temp.fMaxShield = 0.f;
	temp.fSpeed = 2.f;
	m_Speed = temp.fSpeed;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Status"), LEVEL_STATIC, TEXT("Prototype_Component_Status"), (CComponent**)&m_pStatus, &temp)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 1.7f, 0.f);
	ColliderDesc.fRadius = 1.5f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CGoliathBig::SetUp_ConstantTable()
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

void CGoliathBig::State_Tick(_double TimeDelta)
{
	_float Dist = fabs(XMVectorGetX(XMVector3Length(m_Player->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))));
	_bool flag = false;

	if (Dist < 30.f) {
		m_LockOn = true;
	}

	if (m_LockOn == true && m_AttackDesc.isAttack == false && m_Damaged == false) {
		if (FAILED(m_pTransformCom->MyLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.3f, TimeDelta))) {
			if (m_AttackDesc.isAttack == false) {
				if (m_AttackDesc.isCoolTime == false) {
					flag = true;
					m_AttackDesc.isAttack = true;
					m_AttackDesc.Initialize();
				}

				if (m_AttackDesc.isCoolTime == true && m_AttackDesc.CoolTime <= m_AttackDesc.TotalTime) {
					m_AttackDesc.isCoolTime = false;
					m_AttackDesc.TotalTime = 0.0;
				}
			}
		}

		if (m_MovingDesc.isStart == false) {
			m_MovingDesc.isStart = true;
			m_randomMove = rand() % 5;

			if (m_randomMove == 0 && Dist <= 5.f) {
				m_randomMove = 3;
			}

			m_MovingDesc.TotalTime = 0.f;
		}

		if (m_MovingDesc.isStart == true) {

			if (m_MovingDesc.Duration <= m_MovingDesc.TotalTime) {
				m_MovingDesc.isStart = false;
			}
			else if (m_AttackDesc.isAttack == false && m_Damaged == false) {
				flag = true;

				switch (m_randomMove)
				{
				case 0:
					m_iAnimIndex = ANIM_RUN_FRONT;
					m_pTransformCom->Go_StraightOnNavigation(TimeDelta * m_Speed, &m_iCurrentCellIndex, 0.f);
					break;
				case 1:
					m_pTransformCom->Go_RightOnNavigation(TimeDelta * m_Speed, &m_iCurrentCellIndex, 0.f);
					m_iAnimIndex = ANIM_RUN_RIGHT;
					break;
				case 2:
					m_pTransformCom->Go_LeftOnNavigation(TimeDelta * m_Speed, &m_iCurrentCellIndex, 0.f);
					m_iAnimIndex = ANIM_RUN_LEFT;
					break;
				case 3:
					m_pTransformCom->Go_BackOnNavigation(TimeDelta * m_Speed, &m_iCurrentCellIndex, 0.f);
					m_iAnimIndex = ANIM_RUN_BACK;
					break;
				default:
					break;
				}
			}
			m_MovingDesc.TotalTime += TimeDelta;
		}
	}

	m_AttackDesc.TotalTime += TimeDelta;
	if (flag == false) {
		m_iAnimIndex = ANIM_IDLE;
	}
}


void CGoliathBig::Animation_Tick(_double TimeDelta)
{
	if (m_Damaged == true) {
		switch (m_Stagger)
		{
		case 0:
			m_iAnimIndex = ANIM_STAGGER_BACK;
			break;
		case 1:
			m_iAnimIndex = ANIM_STAGGER_FRONT;
			break;
		case 2:
			m_iAnimIndex = ANIM_STAGGER_RIGHT;
			break;
		case 3:
			m_iAnimIndex = ANIM_STAGGER_LEFT;
			break;
		default:
			break;
		}
	}

	if (m_AttackDesc.isAttack == true) {
		if (m_AttackDesc.Progress == 0)
			m_iAnimIndex = ANIM_PRESHOOT;
		else
			m_iAnimIndex = ANIM_SHOOT;
	}

	switch (m_iAnimIndex)
	{
	case Client::CGoliathBig::ANIM_IDLE:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CGoliathBig::ANIM_JETPACK_LOOP:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CGoliathBig::ANIM_JETPACK_START:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CGoliathBig::ANIM_PRESHOOT:
		if (m_pModelCom->Get_isAnimEnd(ANIM_PRESHOOT)) {
			m_AttackDesc.Progress++;
			m_pModelCom->Set_FinishFalse(ANIM_SHOOT);

			Set_Bullet();
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
		break;
	case Client::CGoliathBig::ANIM_RUN_BACK:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CGoliathBig::ANIM_RUN_FRONT:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CGoliathBig::ANIM_RUN_LEFT:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CGoliathBig::ANIM_RUN_RIGHT:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CGoliathBig::ANIM_SHOOT:
		if (m_pModelCom->Get_isAnimEnd(ANIM_SHOOT)) {
			m_AttackDesc.Progress = 0;
			m_AttackDesc.isAttack = false;
			m_AttackDesc.Reset_CoolTime();
			m_pModelCom->Set_FinishFalse(ANIM_PRESHOOT);
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
		break;
	case Client::CGoliathBig::ANIM_STAGGER_BACK:
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex)) {
			m_pModelCom->Set_AnimationIndex(ANIM_IDLE, 1.0, 1.0);
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Damaged = false;
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.5, 1.0, false);
		break;
	case Client::CGoliathBig::ANIM_STAGGER_FRONT:
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex)) {
			m_pModelCom->Set_AnimationIndex(ANIM_IDLE, 1.0, 1.0);
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Damaged = false;
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.5, 1.0, false);
		break;
	case Client::CGoliathBig::ANIM_STAGGER_LEFT:
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex)) {
			m_pModelCom->Set_AnimationIndex(ANIM_IDLE, 1.0, 1.0);
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Damaged = false;
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.5, 1.0, false);
		break;
	case Client::CGoliathBig::ANIM_STAGGER_RIGHT:
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex)) {
			m_pModelCom->Set_AnimationIndex(ANIM_IDLE, 1.0, 1.0);
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Damaged = false;
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.5, 1.0, false);
		break;
	case Client::CGoliathBig::ANIM_STURN_LOOP:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CGoliathBig::ANIM_STURN_START:
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex)) {
			m_pModelCom->Set_AnimationIndex(ANIM_STURN_LOOP, 1.0, 1.0);
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Damaged = false;
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.5, 1.0, false);
		break;
	default:
		break;
	}


}

_int CGoliathBig::Status_Tick(_double TimeDelta)
{
	int flag = m_pStatus->Status_Tick(TimeDelta);
	if (flag == 1)
		m_Dead = true;

	if (flag == 2) {
		if (m_AttackDesc.isAttack == false)
			m_Damaged = true;
		m_Stagger = rand() % 4;
		while (m_prevStagger == m_Stagger) {
			m_Stagger = rand() % 4;
		}
		m_prevStagger = m_Stagger;
	}

	return _int();
}

void CGoliathBig::Set_Bullet()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	_matrix temp = XMMatrixMultiply(XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("R_Canon_Scale_02")), XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix()));
	temp = XMMatrixMultiply(temp, m_pTransformCom->Get_WorldMatrix());
	_float4x4 temp2;
	XMStoreFloat4x4(&temp2, temp);

	CBullet::BulletDesc BulletDesc;
	BulletDesc.vPos = XMLoadFloat3(&_float3(temp2.m[3][0], temp2.m[3][1], temp2.m[3][2]));
	BulletDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	BulletDesc.Damage = 10.f;
	BulletDesc.CriticalPer = 0.f;
	BulletDesc.Size = 3.0f;
	BulletDesc.Speed = 1.5f;

	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"HitBox", L"Prototype_GameObject_EnemyBullet", &BulletDesc);

	Safe_Release(pGameinstance);
}


CGoliathBig * CGoliathBig::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CGoliathBig*	pInstance = new CGoliathBig(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CGoliathBig"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGoliathBig::Clone(void * pArg)
{
	CGoliathBig*	pInstance = new CGoliathBig(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CGoliathBig"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoliathBig::Free()
{
	__super::Free();
	Safe_Release(m_Player);
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pStatus);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
