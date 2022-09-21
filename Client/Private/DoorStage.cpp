#include "stdafx.h"
#include "..\Public\DoorStage.h"
#include "GameInstance.h"
#include "KeyMgr.h"

CDoorStage::CDoorStage(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CDoorStage::CDoorStage(const CDoorStage & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDoorStage::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoorStage::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	m_pModelCom->Set_AnimationIndex(0, 1.0, 1.0, false);

	if (pArg != nullptr) {
		_vector posInfo;
		memcpy(&posInfo, pArg, sizeof(_vector));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(posInfo), XMVectorGetY(posInfo), XMVectorGetZ(posInfo), 1.f));
		m_pTransformCom->MyRotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMVectorGetW(posInfo));
	}
	else {
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(165.f, 0.f, 175.f, 1.f));
	}

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

_int CDoorStage::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Anim_Tick(TimeDelta);

	m_pModelCom->Update(TimeDelta);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	Update_Alpha(TimeDelta*2.f);
	return 0;
}

void CDoorStage::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider* pTargetCollider = (CCollider*)pGameInstance->Get_Component(m_iLevelIndex, TEXT("Layer_Player"), TEXT("Com_Sphere"));

	if (nullptr != pTargetCollider) {
		BoundingSphere* TargetSphere = pTargetCollider->Get_SphereWorld();
		if (TargetSphere != nullptr) {
			if (m_pSphereCom->InterSectSphere(*TargetSphere)) {
				m_ShadeCnt = 1;
				if (CKeyMgr::Get_Instance()->Key_Down('E')) {
					m_State = STATE_OPEN;
					CSound_Manager::GetInstance()->StopSound(6);
					CSound_Manager::GetInstance()->SoundPlay(L"OpenDoor.ogg", 6, 1.f);
				}
			}
			else
				m_ShadeCnt = 0;
		}
		
	}
		

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CDoorStage::Render()
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


		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, m_ShadeCnt)))
			return E_FAIL;
	}	
#ifdef _DEBUG
	//m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CDoorStage::Anim_Tick(_double TImeDelta)
{
	switch (m_State)
	{
	case Client::CDoorStage::STATE_CLOSE:
		if (m_pModelCom->Get_isAnimEnd(STATE_CLOSE)) {
			m_pModelCom->Set_FinishFalse(STATE_OPEN);
			break;
		}
		m_isOpen = false;
		m_pModelCom->Set_AnimationIndex(STATE_CLOSE, 1.0, 1.0, false);
		break;
	case Client::CDoorStage::STATE_OPEN:
		if (m_pModelCom->Get_isAnimEnd(STATE_OPEN)) {
			if (m_TotalTime >= 5.f) {
				CSound_Manager::GetInstance()->StopSound(6);
				CSound_Manager::GetInstance()->SoundPlay(L"CloseDoor.ogg", 6, 1.f);
				m_State = STATE_CLOSE;
				m_pModelCom->Set_FinishFalse(STATE_CLOSE);
				m_TotalTime = 0.f;
				break;
			}
			m_isOpen = true;
			m_TotalTime += TImeDelta;
			break;
		}
		m_pModelCom->Set_AnimationIndex(STATE_OPEN, 1.0, 1.0, false);
		break;
	default:
		break;
	}
}

void CDoorStage::Update_Alpha(_double Timedelta)
{
	if (m_flag == false) {
		if (m_fAlpha > 1.0f) {
			m_fAlpha = 1.f;
			m_flag = true;
		}
		m_fAlpha += Timedelta;
	}
	else {
		if (m_fAlpha < 0.0f) {
			m_fAlpha = 0.f;
			m_flag = false;
		}

		m_fAlpha -= Timedelta;
	}
}

HRESULT CDoorStage::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), m_iLevelIndex, TEXT("Prototype_Component_Model_DoorStage"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_AABB */
	ColliderDesc.vPosition = _float3(0.f, 5.f, 0.f);
	//ColliderDesc.vSize = _float3(5.f, 5.f, 5.f);
	ColliderDesc.fRadius = 10.f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CDoorStage::SetUp_ConstantTable()
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

CDoorStage * CDoorStage::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CDoorStage*	pInstance = new CDoorStage(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CDoorStage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDoorStage::Clone(void * pArg)
{
	CDoorStage*	pInstance = new CDoorStage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CDoorStage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoorStage::Free()
{
	__super::Free();	
	
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
