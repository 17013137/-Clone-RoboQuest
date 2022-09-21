#include "stdafx.h"
#include "..\Public\Camera_First.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player_Manager.h"


CCamera_First::CCamera_First(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CCamera(pDeviceOut, pDeviceContextOut)
{
}

CCamera_First::CCamera_First(const CCamera_First & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_First::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_First::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 10.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (m_CameraDesc.pPlayerTrans == nullptr)
		return E_FAIL;

	if (m_iLevelIndex == LEVEL_BOSSTOWER)
		m_SceneType = 0;
	else if (m_iLevelIndex == LEVEL_BOSSDISCO)
		m_SceneType = 1;

	return S_OK;
	
}

_int CCamera_First::Tick(_double TimeDelta)
{
	
	if (CKeyMgr::Get_Instance()->Key_Down(VK_F1))
		flag = flag == TRUE ? FALSE : TRUE;

	if (m_EventMgr->Get_Air() == true) {
		if (m_SceneType == 0)
			Boss_Tower_Event(TimeDelta);
		else if (m_SceneType == 1)
			Boss_Disco_Event(TimeDelta);
	}
	else {
		m_pTransformCom->Set_WorldMTX(m_CameraDesc.pPlayerTrans->Get_WorldFloat4x4());
		if (flag == true) {
			_vector temp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 3.f;
			temp = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - temp;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp);
		}
		else {
			_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 0.22f;
			_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.13f;
			//더하고 노말라이즈

			vLook = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vLook + vUp;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLook);
		}
	}

	Shaking(TimeDelta);

	__super::Tick(TimeDelta);

	return 0;
}

void CCamera_First::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CCamera_First::Render()
{
	if (FAILED(__super::Render()))
		return S_OK;

	return S_OK;
}

void CCamera_First::Shaking(_double TimeDelta)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	//CStatus* PlayerStatus = (CStatus*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Player", L"Com_Status");
	//
	//if (PlayerStatus->Status_Tick(TimeDelta) == 2) {
	//	CPlayer_Manager::Get_Instance()->Set_Damaged(true);
	//	m_isShaking = true;
	//}
	if (CPlayer_Manager::Get_Instance()->Get_Damaged() == true)
		m_isShaking = true;

	Safe_Release(pGameinstance);
	if (m_isShaking == true) {
		if (FAILED(m_pTransformCom->Go_Shake(TimeDelta, 0.03f, 15.f, 5)))
			m_isShaking = false;
	}

}

void CCamera_First::Boss_Tower_Event(_double TimeDelta)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	if (m_EventMgr->Get_TakeStart() == false) {
		m_EventMgr->Set_TakeStart(true);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(185.f, 4.f, 195.f, 1.f));
	}
	CModel* BossModel = (CModel*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Enemy", L"Com_Model", 0);
	CTransform* BossTrans = (CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Enemy", m_pTransformTag, 0);
	_float4x4* fBone = BossModel->Get_CombinedTransformationMatrix("Head");
	_matrix Bone = XMMatrixMultiply(XMLoadFloat4x4(fBone), XMLoadFloat4x4(&BossModel->Get_PivotMatrix()));
	Bone = XMMatrixMultiply(Bone, BossTrans->Get_WorldMatrix());

	Safe_Release(pGameinstance);

	if (m_EventMgr->Get_NowTake() == 0) {
		if (BossModel == nullptr || BossTrans == nullptr)
			return;
		
		_vector BonePos = Bone.r[3];
		m_pTransformCom->CameraLookAt(BonePos);
	}

	if (m_EventMgr->Get_NowTake() == 1) {
		_vector BonePos = Bone.r[3];
		m_pTransformCom->CameraLookAt(BonePos);

		_vector vLerp = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorSet(185.f, 1.f, 190.f, 1.f), m_EventMgr->Get_TimeAcc());
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerp);

		m_EventMgr->Add_TimeAcc(TimeDelta);
	}

	if (m_EventMgr->Get_NowTake() == 2) {
		m_pTransformCom->Go_BackWard(TimeDelta*0.1f);
	}
}

void CCamera_First::Boss_Disco_Event(_double TimeDelta)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);
	CModel* BossModel = (CModel*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Enemy", L"Com_Model", 0);
	CTransform* BossTrans = (CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Enemy", m_pTransformTag, 0);
	if (m_EventMgr->Get_TakeStart() == false) {
		m_EventMgr->Set_TakeStart(true);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(176, 5.f, 183.f, 1.f));
		m_pTransformCom->CameraLookAt(BossTrans->Get_State(CTransform::STATE_POSITION));
	}
	_float4x4* fBone = BossModel->Get_CombinedTransformationMatrix("Head");
	_matrix Bone = XMMatrixMultiply(XMLoadFloat4x4(fBone), XMLoadFloat4x4(&BossModel->Get_PivotMatrix()));
	Bone = XMMatrixMultiply(Bone, BossTrans->Get_WorldMatrix());
	_vector BonePos = Bone.r[3];
	Safe_Release(pGameinstance);

	if (m_EventMgr->Get_NowTake() == 0) {
		if (BossModel == nullptr || BossTrans == nullptr)
			return;
		
		m_pTransformCom->CameraLookAt(BonePos);
		m_pTransformCom->Go_Straight(TimeDelta * 0.1f);
	}

	if (m_EventMgr->Get_NowTake() == 1) {
		m_pTransformCom->CameraLookAt(BonePos);
		m_EventMgr->Add_TimeAcc(TimeDelta);
	}

	if (m_EventMgr->Get_NowTake() == 2) {
		m_pTransformCom->CameraLookAt(BonePos);
	}
}

CCamera_First * CCamera_First::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CCamera_First*	pInstance = new CCamera_First(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CCamera_First"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_First::Clone(void * pArg)
{
	CCamera_First*	pInstance = new CCamera_First(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CCamera_First"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_First::Free()
{
	__super::Free();

}
