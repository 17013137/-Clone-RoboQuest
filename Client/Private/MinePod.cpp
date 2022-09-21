#include "stdafx.h"
#include "..\Public\MinePod.h"
#include "GameInstance.h"
#include "Bullet.h"


CMinePod::CMinePod(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CMinePod::CMinePod(const CMinePod & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMinePod::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMinePod::NativeConstruct(void * pArg)
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

	if (pArg != nullptr) {
		_float3 fPos = _float3();

		memcpy(&fPos, pArg, sizeof(_float3));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPos.x, fPos.y, fPos.z, 1.0f));
	}
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), rand() % 10);

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_LayerIndex = 3;

	m_Speed = 0.5f;
	return S_OK;
}

_int CMinePod::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Status_Tick(TimeDelta);

	if (m_Dead == true) {
		int Cnt = rand() % 3 + 1;
		for (int i = 0; i < Cnt; i++)
			Create_ExpBall();
		Create_Smoke(XMLoadFloat3(&m_pSphereCom->Get_Position()));
		CSound_Manager::GetInstance()->StopSound(m_LayerIndex);
		CSound_Manager::GetInstance()->SoundPlay(L"EnemyDeath.ogg", m_LayerIndex, 1.f);
		return 1;
	}

	State_Tick(TimeDelta);
	Animation_Tick(TimeDelta);
	if (m_Culling == false) {
		m_pModelCom->Update(TimeDelta);
	}
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	return 0;
}



void CMinePod::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom && m_Culling == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CMinePod::Render()
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

HRESULT CMinePod::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MinePod"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CStatus::STATUSDESC temp;
	temp.fMaxHp = 500.f;
	temp.fMaxShield = 0.f;
	temp.fSpeed = 2.f;
	m_Speed = temp.fSpeed;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Status"), LEVEL_STATIC, TEXT("Prototype_Component_Status"), (CComponent**)&m_pStatus, &temp)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 1.5f, 0.f);
	ColliderDesc.fRadius = 1.5f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMinePod::SetUp_ConstantTable()
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
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMinePod::State_Tick(_double TimeDelta)
{
	_float Dist = fabs(XMVectorGetX(XMVector3Length(m_Player->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))));
	_bool flag = false;

	if (Dist < 50.f) {
		m_LockOn = true;
	}

	if (m_LockOn == true && m_AttackDesc.isCoolTime == false) {
		CSound_Manager::GetInstance()->SoundPlay(L"MineRotation.ogg", m_LayerIndex, 0.7f);

		if (FAILED(m_pTransformCom->MyLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.0f, TimeDelta))) {
			CSound_Manager::GetInstance()->StopSound(m_LayerIndex);
			m_AttackDesc.isAttack = true;
			m_AttackDesc.isCoolTime = true;
		}
	}

	if (m_AttackDesc.CoolTime < m_AttackDesc.TotalTime) {
		m_AttackDesc.isCoolTime = false;
		m_AttackDesc.TotalTime = 0.0;
	}

	m_AttackDesc.TotalTime += TimeDelta;
	if (flag == false) {
		m_iAnimIndex = ANIM_IDLE;
	}
}


void CMinePod::Animation_Tick(_double TimeDelta)
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
	case Client::CMinePod::ANIM_IDLE:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CMinePod::ANIM_PRESHOOT:
		if (m_pModelCom->Get_isAnimEnd(ANIM_PRESHOOT)) {
			m_AttackDesc.Progress++;
			m_pModelCom->Set_FinishFalse(ANIM_SHOOT);
			Set_Bullet();
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
		break;
	case Client::CMinePod::ANIM_SHOOT:
		if (m_pModelCom->Get_isAnimEnd(ANIM_SHOOT)) {
			m_AttackDesc.Progress = 0;
			m_AttackDesc.isAttack = false;
			m_pModelCom->Set_FinishFalse(ANIM_PRESHOOT);
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
	case Client::CMinePod::ANIM_STAGGER_BACK:
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex)) {
			m_pModelCom->Set_AnimationIndex(ANIM_IDLE, 1.0, 1.0);
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Damaged = false;
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.5, 1.0, false);
		break;
	case Client::CMinePod::ANIM_STAGGER_FRONT:
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex)) {
			m_pModelCom->Set_AnimationIndex(ANIM_IDLE, 1.0, 1.0);
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Damaged = false;
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.5, 1.0, false);
		break;
	case Client::CMinePod::ANIM_STAGGER_LEFT:
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex)) {
			m_pModelCom->Set_AnimationIndex(ANIM_IDLE, 1.0, 1.0);
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Damaged = false;
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.5, 1.0, false);
		break;
	case Client::CMinePod::ANIM_STAGGER_RIGHT:
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex)) {
			m_pModelCom->Set_AnimationIndex(ANIM_IDLE, 1.0, 1.0);
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Damaged = false;
		}
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.5, 1.0, false);
		break;
	case Client::CMinePod::ANIM_STUN_LOOP:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
		break;
	case Client::CMinePod::ANIM_STUN_START:
		m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
		break;
	default:
		break;
	}
}

_int CMinePod::Status_Tick(_double TimeDelta)
{
	int flag = m_pStatus->Status_Tick(TimeDelta);
	if (flag == 1)
		m_Dead = true;

	if (flag == 2) {
		CSound_Manager::GetInstance()->StopSound(m_LayerIndex);
		CSound_Manager::GetInstance()->SoundPlay(L"MonsterDamaged.ogg", m_LayerIndex, 0.5f);
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

void CMinePod::Set_Bullet()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	CSound_Manager::GetInstance()->StopSound(m_LayerIndex);
	CSound_Manager::GetInstance()->SoundPlay(L"MineAttack.ogg", m_LayerIndex, 1.f);

	_matrix temp = XMMatrixMultiply(XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("Canon_Middle")), XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix()));
	temp = XMMatrixMultiply(temp, m_pTransformCom->Get_WorldMatrix());
	_float4x4 temp2;
	_float4x4 temp3;
	_vector vDir = _vector();
	XMStoreFloat4x4(&temp2, temp);

	CBullet::BulletDesc BulletDesc;
	BulletDesc.vPos = XMLoadFloat3(&_float3(temp2.m[3][0], temp2.m[3][1], temp2.m[3][2]));
	BulletDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	BulletDesc.Damage = 20.f;
	BulletDesc.CriticalPer = 0.f;
	
	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Layer_Enemy", L"Prototype_GameObject_MineHitBox", &BulletDesc);

	temp = XMMatrixMultiply(XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("Canon_Right")), XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix()));
	temp = XMMatrixMultiply(temp, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&temp3, temp);
	vDir = XMVectorSet(temp3.m[3][0] - temp2.m[3][0], temp3.m[3][1] - temp2.m[3][1], temp3.m[3][2] - temp2.m[3][2], 0.f);
	BulletDesc.vLook = XMVector3Normalize(vDir);
	if (FAILED(pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Layer_Enemy", L"Prototype_GameObject_MineHitBox", &BulletDesc))) {
		int a = 10;
	}

	temp = XMMatrixMultiply(XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("Canon_Left")), XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix()));
	temp = XMMatrixMultiply(temp, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&temp3, temp);
	vDir = XMVectorSet(temp3.m[3][0] - temp2.m[3][0], temp3.m[3][1] - temp2.m[3][1], temp3.m[3][2] - temp2.m[3][2], 0.f);
	BulletDesc.vLook = XMVector3Normalize(vDir);
	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Layer_Enemy", L"Prototype_GameObject_MineHitBox", &BulletDesc);

	Safe_Release(pGameinstance);
}


CMinePod * CMinePod::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CMinePod*	pInstance = new CMinePod(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CMinePod"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMinePod::Clone(void * pArg)
{
	CMinePod*	pInstance = new CMinePod(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CMinePod"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMinePod::Free()
{
	__super::Free();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pStatus);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
