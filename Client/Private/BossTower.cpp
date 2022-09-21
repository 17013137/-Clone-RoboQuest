#include "stdafx.h"
#include "..\Public\BossTower.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Bullet.h"
#include "Event_Manager.h"

CBossTower::CBossTower(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CBossTower::CBossTower(const CBossTower & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossTower::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossTower::NativeConstruct(void * pArg)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(185.f, 0.f, 213.f, 1.f));
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 2.f);

//	m_pModelCom->Set_AnimationIndex(CANON_END + FLAME_END + HANDBEAM_END + NORMAL_INTRO, 1.0, 1.0, false);
	m_pModelCom->Set_AnimationIndex(0, 1.0, 1.0, false);

	m_pModelCom->Set_FinishFalse(m_iAnimIndex);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	//m_pTransformCom->Scaled(_float3(1.5f, 1.5f, 1.5f));
	return S_OK;
}

_int CBossTower::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	int flag = false;
	
	if (m_Dead == true) {
		if (m_isDisolve == false) {
			m_DisolveTime -= TimeDelta*0.3f;
		}
		
		if (m_DisolveTime < 0.f) {
			m_DisolveTime = 0.f;
			m_isDisolve = true;
		}

		return 0;
	}

	for (_uint i = 0; i < SKILL_END; i++) {
		if (m_isSkill[i] == true) {
			flag = true;
		}
	}

	if (flag == false) {
		CSound_Manager::GetInstance()->SoundPlay(L"Rotation.ogg", 3, 1.f);
		if (FAILED(m_pTransformCom->MyLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.0, TimeDelta))) {
			CSound_Manager::GetInstance()->StopSound(3);
			CSound_Manager::GetInstance()->SoundPlay(L"RotationEnd.ogg", 3, 1.f);
			KeyInput();
		}
	}

	Set_Bone();
	Animation_Tick(TimeDelta);
	m_pModelCom->Update(TimeDelta);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	Status_Tick(TimeDelta);

	return 0;
}



void CBossTower::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom && m_isDisolve == false) {
		if(m_isIntro == true)
			m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
	}
}


HRESULT CBossTower::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 4)))
			return E_FAIL;
	}

#ifdef _DEBUG
//	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CBossTower::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), m_iLevelIndex, TEXT("Prototype_Component_Model_BossTower"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CStatus::STATUSDESC temp;
	temp.fMaxHp = 5000.f;
	temp.fMaxShield = 0.f;
	temp.fSpeed = 2.f;
	m_Speed = temp.fSpeed;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Status"), LEVEL_STATIC, TEXT("Prototype_Component_Status"), (CComponent**)&m_pStatus, &temp)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_AABB */

	ColliderDesc.vPosition = _float3(0.f, 5.0f, 0.f);
	ColliderDesc.fRadius = 5.0f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;
	if (FAILED(__super::SetUp_Components(TEXT("Com_DisolveTexture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossTower::SetUp_ConstantTable()
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
	if (FAILED(m_pShaderCom->Set_RawValue("fAlpha", &m_fAmbient, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("Disolve", &m_DisolveTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTexture->SetUp_ShaderResourceView(m_pShaderCom, "g_DisolveTexture", 9)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBossTower::Animation_Tick(_double TimeDelta)
{
	_bool flag = false;
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	

	CBullet::BulletDesc BulletDesc;
	BulletDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	BulletDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	BulletDesc.Damage = 10.f;
	BulletDesc.CriticalPer = 0.f;

	CTransform* HitBoxTrans = (CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"CanonHitBox", m_pTransformTag);
	if (m_Manner.isMannerTime == true) {
		m_iAnimIndex = CANON_END + FLAME_END + HANDBEAM_END + NORMAL_IDLE;
		m_Manner.TimeAcc += TimeDelta;

		if (m_Manner.TimeAcc >= 3.0) {
			m_Manner.isMannerTime = false;
		}
	}
	else if (m_isIntro) {		//인트로 애니메이션
		m_iAnimIndex = CANON_END + FLAME_END + HANDBEAM_END + NORMAL_INTRO;
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex) == true) {
			CEvent_Manager::Get_Instance()->Set_NowTake(2);
			m_pModelCom->Set_AnimationIndex(CANON_END + FLAME_END + HANDBEAM_END + NORMAL_IDLE, 0.7 * CEvent_Manager::Get_Instance()->Get_SlowTime(), 0.7 * CEvent_Manager::Get_Instance()->Get_SlowTime());
			
			if (CEvent_Manager::Get_Instance()->Get_Air() == false) {
				m_Manner.isMannerTime = true;
				m_isIntro = false;
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

			if (m_pModelCom->Get_CurrentIndex(m_iAnimIndex) == 56) { //원래 58
				CEvent_Manager::Get_Instance()->Set_SlowTime(1.f);
			}

			if (m_pModelCom->Get_CurrentIndex(m_iAnimIndex) == 3) {
				CSound_Manager::GetInstance()->StopSound(5);
				CSound_Manager::GetInstance()->SoundPlay(L"BossIntro.ogg", 5, 1.f);
			}
		}
	}
	else if (m_isSkill[SKILL_CANON] == true) {				// 터닝 캐논
		switch (m_Progression[SKILL_CANON])
		{
		case 0:
			if(m_pModelCom->Get_isAnimEnd(CANON_IN) == true) {
				m_Progression[SKILL_CANON]++;
				CSound_Manager::GetInstance()->StopSound(3);
				CSound_Manager::GetInstance()->SoundPlay(L"LaserStart.ogg", 3, 1.f);
				break;
			}
			m_pModelCom->Set_AnimationIndex(CANON_IN, 1.0, 1.0, false);
			break;
		case 1:
			if (m_pModelCom->Get_isAnimEnd(CANON_PRESHOOT) == true) {
				m_Progression[SKILL_CANON]++;
				BulletDesc.vPos = XMLoadFloat4(&m_BonePart[BONE_MORTAR_TOP]);
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"EnemyBullet", L"Prototype_GameObject_CanonHitBox", &BulletDesc);
				break;
			}
			m_pModelCom->Set_AnimationIndex(CANON_PRESHOOT, 1.0, 1.0, false);
			break;
		case 2:
			if (FAILED(m_pTransformCom->AccelTurn(m_pTransformCom->Get_State(CTransform::STATE_UP), TimeDelta * 0.5f, &flag))) {
				m_Manager->Set_CanonFinish(true);
				m_Progression[SKILL_CANON]++;
				CSound_Manager::GetInstance()->StopSound(4);
				CSound_Manager::GetInstance()->SoundPlay(L"LaserStop.ogg", 4, 1.f);
			}
			if (HitBoxTrans != nullptr) {
				_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				HitBoxTrans->MyLookSet(vLook);
				HitBoxTrans->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_BonePart[BONE_MORTAR_TOP]) + vLook * 25.f );
			}
			if(flag == false)
				m_pModelCom->Set_AnimationIndex(CANON_SHOOT_L, 1.0, 1.0);
			else
				m_pModelCom->Set_AnimationIndex(CANON_SHOOT_R, 1.0, 1.0);
			CSound_Manager::GetInstance()->SoundPlay(L"LaserLoop.ogg", 4, 1.f);
			break;
		case 3:
			if (m_pModelCom->Get_isAnimEnd(CANON_OUT) == true) {
				m_isSkill[SKILL_CANON] = false;
				m_Progression[SKILL_CANON] = 0;
				
				break;
			}
			m_pModelCom->Set_AnimationIndex(CANON_OUT, 1.0, 1.0, false);
			break;
		default:
			break;
		}
	}
	else if (m_isSkill[SKILL_FLAME] == true) {		//플레임 샷
		m_iAnimIndex = CANON_END;
		switch (m_Progression[SKILL_FLAME])
		{
			
		case 0:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + FLAME_IN) == true) {
				CSound_Manager::GetInstance()->StopSound(3);
				CSound_Manager::GetInstance()->SoundPlay(L"FlameCast.ogg", 3, 1.f);
				m_Progression[SKILL_FLAME]++;
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + FLAME_IN, 1.0, 1.0, false);
			break;
		case 1:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + FLAME_PRESHOOT) == true) {
				m_Manager->Set_isFlameShoot(true);
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Effect", L"Prototype_Effect_FireEffect");
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Effect", L"Prototype_GameObject_FlameCone");
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Effect", L"Prototype_GameObject_FlameTriangle");
				CSound_Manager::GetInstance()->StopSound(3);
				CSound_Manager::GetInstance()->SoundPlay(L"FlameShoot.ogg", 3, 1.f);
				//for(_uint i = 0 ; i < 20; i++)
				//	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Flame_HitBox", L"Prototype_GameObject_FlameHitBox", &BulletDesc);
				m_Progression[SKILL_FLAME]++;
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + FLAME_PRESHOOT, 0.8, 1.0, false);
			break;
		case 2:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + FLAME_SHOOT) == true) {
				m_Progression[SKILL_FLAME]++;
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + FLAME_SHOOT, 0.6, 1.0, false);
			CSound_Manager::GetInstance()->SoundPlay(L"FlameLoop.ogg", 4, 1.f);
			break;
		case 3:
			if (m_FlameCnt == 0) {
				CSound_Manager::GetInstance()->SoundPlay(L"Rotation.ogg", 3, 1.f);
				if (FAILED(m_pTransformCom->Reserve_Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), 0.5, TimeDelta))) {
					CSound_Manager::GetInstance()->StopSound(3);
					CSound_Manager::GetInstance()->SoundPlay(L"RotationEnd.ogg", 3, 1.f);
					m_Progression[SKILL_FLAME] = 1;
					m_FlameCnt++;
					for (_uint i = CANON_END; i < CANON_END + FLAME_END; i++)
						m_pModelCom->Set_FinishFalse(i);
				}
				m_pModelCom->Set_AnimationIndex(m_iAnimIndex + FLAME_IDLE, 1.0, 1.0);
			}
			else {
				if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + FLAME_OUT) == true) {
					CSound_Manager::GetInstance()->StopSound(4);
					m_isSkill[SKILL_FLAME] = false;
					m_FlameCnt = 0;
					m_Progression[SKILL_FLAME] = 0;
					m_Manager->Set_isFlameShoot(false);
					break;
				}
				m_pModelCom->Set_AnimationIndex(m_iAnimIndex + FLAME_OUT, 1.0, 1.0, false);
			}
			break;
		default:
			break;
		}


	}
	else if (m_isSkill[SKILL_HANDBEAM] == true) {					/* 핸드빔 */
		_vector vPos = m_Player->Get_State(CTransform::STATE_POSITION);
		_float temp = 0.0f;
		m_iAnimIndex = CANON_END + FLAME_END;
		switch (m_Progression[SKILL_HANDBEAM])
		{
		case 0:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + HANDBEAM_IN) == true) {
				CSound_Manager::GetInstance()->StopSound(3);
				CSound_Manager::GetInstance()->SoundPlay(L"ShieldOut.ogg", 3, 1.f);
				m_Progression[SKILL_HANDBEAM]++;
				BulletDesc.vPos = XMLoadFloat4(&m_BonePart[BONE_L_WRIST]);
				//pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"HandBeam", L"Prototype_GameObject_HandBeam", &BulletDesc);
				pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Layer_Enemy", TEXT("Prototype_GameObject_BossShield"));
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + HANDBEAM_IN, 1.0, 1.0, false);
			break;
		case 1:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + HANDBEAM_SHOOT_OPEN) == true) {
				CSound_Manager::GetInstance()->StopSound(3);
				CSound_Manager::GetInstance()->SoundPlay(L"ShieldShoot.ogg", 3, 1.f);
				m_Progression[SKILL_HANDBEAM]++;
				break;
			}
			temp = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))));
			if (0.8f < temp) {
				//XMVectorSetX(vPos, XMVectorGetX(XMVector3Normalize(vPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))*0.5f);
				//XMVectorSetZ(vPos, XMVectorGetZ(XMVector3Normalize(vPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))*0.5f);
				//
				//m_Player->Set_State(CTransform::STATE_POSITION, vPos);
			}
			
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + HANDBEAM_SHOOT_OPEN, 1.0, 1.0, false);
			break;
		case 2:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + HANDBEAM_SHOOT_CLOSE) == true) {
				CSound_Manager::GetInstance()->StopSound(3);
				CSound_Manager::GetInstance()->SoundPlay(L"ShieldEnd.ogg", 3, 1.f);
				m_Progression[SKILL_HANDBEAM]++;
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + HANDBEAM_SHOOT_CLOSE, 1.0, 1.0, false);
			break;
		case 3:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + HANDBEAM_OUT) == true) {
				m_Progression[SKILL_HANDBEAM] = 0;
				m_isSkill[SKILL_HANDBEAM] = false;
				break;
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + HANDBEAM_OUT, 1.0, 1.0, false);
			break;
		default:
			break;
		}
	}
	else if (m_isSkill[SKILL_MORTAR] == true) {						/* 모터샷 */
		m_iAnimIndex = CANON_END + FLAME_END + HANDBEAM_END;
		if (m_Progression[SKILL_MORTAR] == 0) {

			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + NORMAL_MORTAR_PRESHOOT) == true) {
				CSound_Manager::GetInstance()->StopSound(3);
				CSound_Manager::GetInstance()->SoundPlay(L"MortarShoot.ogg", 3, 1.f);
				m_Progression[SKILL_MORTAR]++;
				for(_uint i = 0 ; i < 3 ; i++)
					pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"MortarHitBox", L"Prototype_GameObject_OverHit");
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + NORMAL_MORTAR_SHOOT);
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + NORMAL_MORTAR_PRESHOOT, 1.0, 1.0, false);
		}
		else if (m_Progression[SKILL_MORTAR] == 1) {
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex + NORMAL_MORTAR_SHOOT) == true) {
				
				m_MortarDesc.Cnt++;
				m_pModelCom->Set_FinishFalse(m_iAnimIndex + NORMAL_MORTAR_PRESHOOT);
				if (m_MortarDesc.Cnt >= m_MortarDesc.MortarCnt) {
					m_isSkill[SKILL_MORTAR] = false;
					m_MortarDesc.Cnt = 0;
				}
			}
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex + NORMAL_MORTAR_SHOOT, 1.0, 1.0, false);
		}
	}
	else {
		m_iAnimIndex = (_uint)CANON_END + (_uint)FLAME_END + (_uint)HANDBEAM_END + (_uint)m_State;
		switch (m_State)
		{
		case Client::CBossTower::NORMAL_IDLE:
			//m_pTransformCom->MyLook(m_Player->Get_State(CTransform::STATE_POSITION), 1.0f, TimeDelta);
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
			break;
		case Client::CBossTower::NORMAL_MORTAR_PRESHOOT:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
			break;
		case Client::CBossTower::NORMAL_MORTAR_SHOOT:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
			break;
		case Client::CBossTower::NORMAL_SAW_OFF:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
			break;
		case Client::CBossTower::NORMAL_SAW_ON:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
			break;
		case Client::CBossTower::NORMAL_DAMAGED_LOOP:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
			break;
		default:
			break;
		}
	}

	Safe_Release(pGameinstance);

}

_int CBossTower::Status_Tick(_double Timedelta)
{
	int flag = m_pStatus->Status_Tick(Timedelta);
	if (flag == 1)
		m_Dead = true;

	if (flag == 2) {
		if ((rand() % 10) == 1)
			Create_ExpBall();

		m_fAmbient = 1.f;
		CSound_Manager::GetInstance()->StopSound(5);
		CSound_Manager::GetInstance()->SoundPlay(L"MonsterDamaged.ogg", 5, 0.5f);
	}
	if (m_fAmbient > 0.f) {
		m_fAmbient -= Timedelta*4.f;
	}
	else
		m_fAmbient = 0.f;

	return 0;
}

void CBossTower::KeyInput()
{
	_bool flag = false;

	//if (m_DealTime > m_AccDealTime) {
	//	return;
	//}

	if (m_isSkill[SKILL_CANON] == false && m_isSkill[SKILL_FLAME] == false && m_isSkill[SKILL_HANDBEAM]== false && m_isSkill[SKILL_MORTAR] == false) {
		_uint randomPattern = rand() % 4;


		while (randomPattern == m_PrevPattern) {
			randomPattern = rand() % 4;
		}

		m_PrevPattern = randomPattern;
		switch (randomPattern)
		{
		case 0:
			m_isSkill[SKILL_CANON] = true;
			flag = true;

			for (_uint i = 0; i < CANON_END; i++)
				m_pModelCom->Set_FinishFalse(i);
			break;
		case 1:
			m_isSkill[SKILL_MORTAR] = true;
			flag = true;
			m_MortarDesc.MortarCnt = (rand() % 3) + 1;
			CSound_Manager::GetInstance()->StopSound(3);
			CSound_Manager::GetInstance()->SoundPlay(L"MortarCast.ogg", 3, 1.f);
			for (_uint i = CANON_END + FLAME_END + HANDBEAM_END + NORMAL_MORTAR_PRESHOOT; i < CANON_END + FLAME_END + HANDBEAM_END + NORMAL_SAW_OFF; i++)
				m_pModelCom->Set_FinishFalse(i);
			break;
		case 2:
			m_isSkill[SKILL_HANDBEAM] = true;
			flag = true;

			for (_uint i = CANON_END + FLAME_END; i < CANON_END + FLAME_END + HANDBEAM_END; i++)
				m_pModelCom->Set_FinishFalse(i);
			break;
		case 3:
			m_isSkill[SKILL_FLAME] = true;
			flag = true;

			for (_uint i = CANON_END; i < CANON_END + FLAME_END; i++)
				m_pModelCom->Set_FinishFalse(i);
			break;
		default:
			break;
		}
	}

	//if (CKeyMgr::Get_Instance()->Key_Down(VK_NUMPAD1) && m_isSkill[SKILL_FLAME] == false && m_isSkill[SKILL_HANDBEAM] == false&& m_isSkill[SKILL_CANON] == false && m_isSkill[SKILL_MORTAR] == false) {
	//	m_isSkill[SKILL_CANON] = true;
	//	flag = true;
	//
	//	for (_uint i = 0; i < CANON_END; i++)
	//		m_pModelCom->Set_FinishFalse(i);
	//}
	
	//if (CKeyMgr::Get_Instance()->Key_Down(VK_NUMPAD2) && m_isSkill[SKILL_FLAME] == false && m_isSkill[SKILL_HANDBEAM] == false && m_isSkill[SKILL_CANON] == false && m_isSkill[SKILL_MORTAR] == false) {
	//	m_isSkill[SKILL_FLAME] = true;
	//	flag = true;
	//
	//	for (_uint i = CANON_END; i < CANON_END + FLAME_END; i++)
	//		m_pModelCom->Set_FinishFalse(i);
	//}

	//if (CKeyMgr::Get_Instance()->Key_Down(VK_NUMPAD3) && m_isSkill[SKILL_FLAME] == false && m_isSkill[SKILL_HANDBEAM] == false && m_isSkill[SKILL_CANON] == false && m_isSkill[SKILL_MORTAR] == false) {
	//	m_isSkill[SKILL_HANDBEAM] = true;
	//	flag = true;
	//
	//	for (_uint i = CANON_END + FLAME_END; i < CANON_END + FLAME_END + HANDBEAM_END; i++)
	//		m_pModelCom->Set_FinishFalse(i);
	//}

	//if (CKeyMgr::Get_Instance()->Key_Down(VK_NUMPAD4) && m_isSkill[SKILL_FLAME] == false && m_isSkill[SKILL_HANDBEAM] == false && m_isSkill[SKILL_CANON] == false && m_isSkill[SKILL_MORTAR] == false) {
	//	m_isSkill[SKILL_MORTAR] = true;
	//	flag = true;
	//	m_MortarDesc.MortarCnt = (rand() % 3) + 1;
	//
	//	for (_uint i = CANON_END + FLAME_END + HANDBEAM_END + NORMAL_MORTAR_PRESHOOT; i < CANON_END + FLAME_END + HANDBEAM_END + NORMAL_SAW_OFF; i++)
	//		m_pModelCom->Set_FinishFalse(i);
	//}


	if (flag == false) 
		m_State = NORMAL_IDLE;

}

void CBossTower::Set_Bone()
{
	for (int i = 0; i < BONE_END; i++) {
		_matrix temp = XMMatrixMultiply(XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix(Bone_Str[i])), XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix()));
		temp = XMMatrixMultiply(temp, m_pTransformCom->Get_WorldMatrix());
		_float4x4 temp2;
		XMStoreFloat4x4(&temp2, temp);

		m_BonePart[i] = _float4(temp2.m[3][0], temp2.m[3][1], temp2.m[3][2], 1.f);

		m_Manager->Set_IK_Mtx(temp);
	}

}


CBossTower * CBossTower::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBossTower*	pInstance = new CBossTower(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBossTower"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossTower::Clone(void * pArg)
{
	CBossTower*	pInstance = new CBossTower(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBossTower"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossTower::Free()
{
	__super::Free();	
	m_Manager->Destroy_Instance();
	Safe_Release(m_pTexture);
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pStatus);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
