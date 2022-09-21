#include "stdafx.h"
#include "..\Public\BossDisco.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Bullet.h"
#include "Event_Manager.h"

CBossDisco::CBossDisco(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CBossDisco::CBossDisco(const CBossDisco & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossDisco::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossDisco::NativeConstruct(void * pArg)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(176.f, 4.0f, 211.f, 1.f));
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 2.f);

	m_PrevPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());



	return S_OK;
}

_int CBossDisco::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	int flag = false;

	if (m_isIntro == true)
		flag = true;

	for (_uint i = 0; i < SKILL_END; i++) {
		if (m_isSkill[i] == true) {
			flag = true;
		}
	}

	if (flag == false) {
		if (FAILED(m_pTransformCom->MyAirLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.0, TimeDelta)))
			KeyInput();
	}

	Set_Bone();
	KeyInput();
	Animation_Tick(TimeDelta);
	State_Tick(TimeDelta);
	m_pModelCom->Update(TimeDelta);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	return 0;
}



void CBossDisco::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_PrevPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (nullptr != m_pRendererCom) {
		if (m_isIntro == true)
			m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
	}
	
}

HRESULT CBossDisco::Render()
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

HRESULT CBossDisco::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), m_iLevelIndex, TEXT("Prototype_Component_Model_BossDisco"), (CComponent**)&m_pModelCom)))
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
	//if (FAILED(__super::SetUp_Components(TEXT("Com_AABB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
	//	return E_FAIL;

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 5.5f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBossDisco::SetUp_ConstantTable()
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

void CBossDisco::Animation_Tick(_double TimeDelta)
{
	_bool flag = false;
	
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	CBullet::BulletDesc BulletDesc;
	BulletDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	BulletDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	BulletDesc.Damage = 10.f;
	BulletDesc.CriticalPer = 0.f;
	m_iAnimIndex = DEFAULT_IDLE;
	if (m_Manner.isMannerTime == true) {
		m_iAnimIndex = DEFAULT_IDLE;

		m_Manner.TimeAcc += TimeDelta;

		if (m_Manner.TimeAcc >= 3.0) {
			m_Manner.isMannerTime = false;
			
		}
	}
	else if (m_isIntro == true) {		//인트로 애니메이션
		m_iAnimIndex = DEFAULT_INTRO;
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex) == true) {
			CEvent_Manager::Get_Instance()->Set_NowTake(2);
			//m_pModelCom->Set_AnimationIndex(DEFAULT_IDLE, 0.01, 0.01);
			if (CEvent_Manager::Get_Instance()->Get_Air() == false) {
				m_Manner.isMannerTime = true;
				m_isIntro = false;
				m_EndIntro = true;
			}
		
		}
		else {
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0 * CEvent_Manager::Get_Instance()->Get_SlowTime(), 1.0, false);
			if (m_pModelCom->Get_CurrentIndex(m_iAnimIndex) == 39) {
				CEvent_Manager::Get_Instance()->Set_NowTake(1);
				CEvent_Manager::Get_Instance()->Set_SlowTime(0.2f);
				if (m_CreateIntro == false) {
					m_CreateIntro = true;
					pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Intro", L"Prototype_GameObject_IntroBackGround");
					pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Intro", L"Prototype_GameObject_IntroTag");
					pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Fade", L"Prototype_GameObject_FadeInOut");
				}
			}

		}
	}
	else if (m_isSkill[SKILL_ARTILLERY] == true) {
		m_iAnimIndex = DEFAULT_END;

		switch (m_Progression[SKILL_ARTILLERY])
		{
		case 0:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + ARTILLERY_PRESHOOT) == true) {
				m_Progression[SKILL_ARTILLERY]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + ARTILLERY_SHOOT);
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + ARTILLERY_PRESHOOT, 1.0, 1.0, false);
			break;
		case 1:
			if (m_SkillDesc[SKILL_ARTILLERY].TotalTime > m_SkillDesc[SKILL_ARTILLERY].Duration) {
				m_Progression[SKILL_ARTILLERY]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + ARTILLERY_OUT);
				m_SkillDesc[SKILL_ARTILLERY].TotalTime = 0.0;
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + ARTILLERY_SHOOT, 1.0, 1.0);
			m_SkillDesc[SKILL_ARTILLERY].TotalTime += TimeDelta;
			break;
		case 2:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + ARTILLERY_OUT) == true) {
				m_isSkill[SKILL_ARTILLERY] = false;
				m_Progression[SKILL_ARTILLERY] = 0;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + ARTILLERY_PRESHOOT);
				m_pTransformCom->Reset_AxisY();
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + ARTILLERY_OUT, 1.0, 1.0, false);
			break;
		default:
			break;
		}
	}
	else if (m_isSkill[SKILL_BOUNCINGBOLT] == true) {
		m_iAnimIndex = DEFAULT_END + ARTILLERY_END;
		switch (m_Progression[SKILL_BOUNCINGBOLT])
		{
		case 0:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + BOUNCINGBOLT_PRESHOOT) == true) {
				m_Progression[SKILL_BOUNCINGBOLT]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + BOUNCINGBOLT_SHOOT);
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + ARTILLERY_PRESHOOT, 1.0, 1.0, false);
			break;
		case 1:
			if (m_SkillDesc[SKILL_BOUNCINGBOLT].TotalTime > m_SkillDesc[SKILL_BOUNCINGBOLT].Duration) {
				m_Progression[SKILL_BOUNCINGBOLT]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + BOUNCINGBOLT_OUT);
				m_SkillDesc[SKILL_BOUNCINGBOLT].TotalTime = 0.0;
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + BOUNCINGBOLT_SHOOT, 1.0, 1.0);
			m_SkillDesc[SKILL_BOUNCINGBOLT].TotalTime += TimeDelta;
			break;
		case 2:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + BOUNCINGBOLT_OUT) == true) {
				m_isSkill[SKILL_BOUNCINGBOLT] = false;
				m_Progression[SKILL_BOUNCINGBOLT] = 0;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + ARTILLERY_PRESHOOT);
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + BOUNCINGBOLT_OUT, 1.0, 1.0, false);
			break;
		default:
			break;
		}

	}
	else if (m_isSkill[SKILL_CHARGE] == true) {
		m_iAnimIndex = DEFAULT_END + ARTILLERY_END + BOUNCINGBOLT_END;

		switch (m_Progression[SKILL_CHARGE])
		{
		case 0:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + CHARGE_PRESHOOT) == true) {
				m_Progression[SKILL_CHARGE]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + CHARGE_FOCUS);
				m_ChargeDesc.vDir = -XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + CHARGE_PRESHOOT, 1.0, 1.0, false);
			m_pTransformCom->MyAirLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.0f, TimeDelta);
			break;
		case 1:
			if (m_ChargeDesc.TotalTime > m_ChargeDesc.ChargeDuration) {
				m_Progression[SKILL_CHARGE]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + CHARGE_LOOP);
				m_ChargeDesc.TotalTime = 0.0;
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + CHARGE_FOCUS, 1.0, 1.0);
			m_pTransformCom->Go_Straight(TimeDelta * m_ChargeDesc.ChargeSpeed);
			m_ChargeDesc.TotalTime += TimeDelta;
			//충돌 등등 빠운스
			break;
		case 2:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + CHARGE_OUT) == true) {
				m_Progression[SKILL_CHARGE]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + CHARGE_PRESHOOT);
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + CHARGE_OUT, 1.0, 1.0, false);
			break;
		case 3:
			if (m_ChargeDesc.TotalRot <= m_ChargeDesc.NowRot) {
				m_Progression[SKILL_CHARGE] = 0;
				m_isSkill[SKILL_CHARGE] = false;
				m_pTransformCom->Reset_AxisY();
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + CHARGE_OUT);
				m_ChargeDesc.TotalTime = 0.0;
				m_ChargeDesc.NowRot = 0.f;
				break;
			}
			
			m_pModelCom->Set_AnimationIndex(DEFAULT_IDLE, 1.0, 1.0);
			m_pTransformCom->MyRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-2.f));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_ChargeDesc.vDir * 0.3f));
			m_ChargeDesc.NowRot += 2.f;
			break;
		default:
			break;
		}

	}
	else if (m_isSkill[SKILL_LASER] == true) {
		m_iAnimIndex = DEFAULT_END + ARTILLERY_END + BOUNCINGBOLT_END + CHARGE_END;

		switch (m_Progression[SKILL_LASER])
		{
		case 0:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + LASER_PRESHOOT) == true) {
				m_Progression[SKILL_LASER]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + LASER_LOOP);
				m_Manager->Set_isLaser(true);
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"HitBox", L"Prototype_GameObject_LaserHitBox", &BulletDesc);
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + LASER_PRESHOOT, 1.0, 1.0, false);
			break;
		case 1:
			if (m_SkillDesc[SKILL_LASER].TotalTime >= m_SkillDesc[SKILL_LASER].Duration) {
				m_Progression[SKILL_LASER]++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + LASER_OUT);
				m_SkillDesc[SKILL_LASER].TotalTime = 0.0;
				m_Manager->Set_isLaser(false);
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + LASER_LOOP, 1.0, 1.0);
			m_SkillDesc[SKILL_LASER].TotalTime += TimeDelta;
			m_pTransformCom->MyLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.5f, TimeDelta);
			m_pTransformCom->Go_MyStraight(TimeDelta * 5.0);
			break;
		case 2:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + LASER_OUT) == true) {
				m_Progression[SKILL_LASER] = 0;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + LASER_PRESHOOT);
				m_isSkill[SKILL_LASER] = false;
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + LASER_OUT, 1.0, 1.0, false);
			break;
		default:
			break;
		}
	}
	else{
		switch (m_State)
		{
		case Client::CBossDisco::DEFAULT_IDLE:
			if (m_EndIntro == true) {
				m_EndIntro = false;
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				vPos = XMVectorSetY(vPos, m_DefaultY);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			}
			m_pModelCom->Set_AnimationIndex(DEFAULT_IDLE, 1.0, 2.0);
			m_pTransformCom->MyLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.0f, TimeDelta);
			break;
		default:
			break;
		}
	}

	Safe_Release(pGameinstance);
}

void CBossDisco::State_Tick(_double TimeDelta)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	_float3 CellNormal = _float3();
	_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_PrevPos;
	if (pGameinstance->CollideWithNavi(m_pSphereCom, vDir, &CellNormal) == true) {
		int a = 10;
	}

	CBullet::BulletDesc BulletDesc;
	BulletDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	BulletDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	BulletDesc.Damage = 10.f;
	BulletDesc.CriticalPer = 0.f;


	if (m_isSkill[SKILL_ARTILLERY]) {
		m_pTransformCom->MyAirLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.0, TimeDelta);

		switch (m_Progression[SKILL_ARTILLERY])
		{
		case 0:
			
			break;
		case 1:
			if (m_Artilley_HitBoxDesc.WaveTime >= m_Artilley_HitBoxDesc.WaveRegenTime) {
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"HitBox", L"Prototype_GameObject_WaveHitBox", &BulletDesc);
				m_Artilley_HitBoxDesc.WaveTime = 0.0;
			}
			if (m_Artilley_HitBoxDesc.StickTime >= m_Artilley_HitBoxDesc.StickRegenTime) {
				BulletDesc.vPos = XMLoadFloat4(&m_BonePart[m_Artilley_HitBoxDesc.StickTimeStartPointing % 2 + 6]);
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"HitBox", L"Prototype_GameObject_StickHitBox", &BulletDesc);
				m_Artilley_HitBoxDesc.StickTime = 0.0;
				m_Artilley_HitBoxDesc.StickTimeStartPointing++;
			}

			if (m_Artilley_HitBoxDesc.NormalTime >= m_Artilley_HitBoxDesc.NormalRegenTime) {
				BulletDesc.vPos = XMLoadFloat4(&m_BonePart[m_Artilley_HitBoxDesc.NormalTimeStartPointing % 6]);
				if (m_Artilley_HitBoxDesc.NormalTimeStartPointing % 2 == 0)
					BulletDesc.vLook += m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.2f;
				else
					BulletDesc.vLook -= m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.2f;
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"HitBox", L"Prototype_GameObject_NormalHitBox", &BulletDesc);
				m_Artilley_HitBoxDesc.NormalTime = 0.0;
				m_Artilley_HitBoxDesc.NormalTimeStartPointing++;
			}

			m_Artilley_HitBoxDesc.NormalTime += TimeDelta;
			m_Artilley_HitBoxDesc.StickTime += TimeDelta;
			m_Artilley_HitBoxDesc.WaveTime += TimeDelta;
			break;
		default:
			break;
		}
	}

	if (m_isSkill[SKILL_BOUNCINGBOLT] == true) {
		if (m_Progression[SKILL_BOUNCINGBOLT] == 1) {
			if (m_BouncingDesc.Duration < m_BouncingDesc.AccTime) {
				m_BouncingDesc.AccTime = 0.0;
				BulletDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_UP);
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"HitBox", L"Prototype_GameObject_JumpingHitBox", &BulletDesc);
			}
		}

		m_BouncingDesc.AccTime += TimeDelta;
	}

	if (m_isSkill[SKILL_BOUNCINGBOLT]) {
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), TimeDelta * 1.5);
	}

	if (m_isSkill[SKILL_CHARGE]) {
		//m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), TimeDelta * 1.5);
	}

	Safe_Release(pGameinstance);
}

void CBossDisco::Set_Bone()
{
	for (int i = 0; i < BONE_END; i++) {
		_matrix temp = XMMatrixMultiply(XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix(Bone_Str[i])), XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix()));
		temp = XMMatrixMultiply(temp, m_pTransformCom->Get_WorldMatrix());
		_float4x4 temp2;
		XMStoreFloat4x4(&temp2, temp);

		m_BonePart[i] = _float4(temp2.m[3][0], temp2.m[3][1], temp2.m[3][2], 1.f);
	}
}

void CBossDisco::KeyInput()
{
	_bool flag = false;

	if (CKeyMgr::Get_Instance()->Key_Down(VK_NUMPAD5)) {
		flag = true;
		m_isSkill[SKILL_ARTILLERY] = true;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_NUMPAD6)) {
		flag = true;
		m_isSkill[SKILL_BOUNCINGBOLT] = true;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_NUMPAD7)) {
		flag = true;
		m_isSkill[SKILL_CHARGE] = true;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_NUMPAD8)) {
		flag = true;
		m_isSkill[SKILL_LASER] = true;
	}

	if (flag == false) {
		m_State = DEFAULT_IDLE;
	}
}


CBossDisco * CBossDisco::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBossDisco*	pInstance = new CBossDisco(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBossDisco"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossDisco::Clone(void * pArg)
{
	CBossDisco*	pInstance = new CBossDisco(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBossDisco"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossDisco::Free()
{
	__super::Free();
	m_Manager->Destroy_Instance();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pStatus);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
