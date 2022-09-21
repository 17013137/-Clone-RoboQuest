#include "stdafx.h"
#include "..\Public\DoorBoss.h"
#include "GameInstance.h"
#include "KeyMgr.h"

CDoorBoss::CDoorBoss(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CDoorBoss::CDoorBoss(const CDoorBoss & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDoorBoss::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoorBoss::NativeConstruct(void * pArg)
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
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(165.f, 0.f, 190.f, 1.f));
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP),-2.f);
	}

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

_int CDoorBoss::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Anim_Tick(TimeDelta);

	m_pModelCom->Update(TimeDelta);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	Update_Alpha(TimeDelta*2.f);
	return 0;
}

void CDoorBoss::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider* pTargetCollider = (CCollider*)pGameInstance->Get_Component(m_iLevelIndex, TEXT("Layer_Player"), TEXT("Com_Sphere"));

	if (nullptr != pTargetCollider) {
		BoundingSphere* TargetSphere = pTargetCollider->Get_SphereWorld();

		if (TargetSphere != nullptr) {
			if (m_pSphereCom->InterSectSphere(*TargetSphere)) {
				m_ShadeCnt = 1;
			}
			else
				m_ShadeCnt = 0;
		}
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CDoorBoss::Render()
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
//	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CDoorBoss::Anim_Tick(_double TImeDelta)
{
	switch (m_State)
	{
	case Client::CDoorBoss::STATE_CLOSE:
		if (m_pModelCom->Get_isAnimEnd(STATE_CLOSE)) {
			m_pModelCom->Set_FinishFalse(STATE_OPEN);
			break;
		}
		m_isOpen = false;
		m_pModelCom->Set_AnimationIndex(STATE_CLOSE, 1.0, 1.0, false);
		break;
	case Client::CDoorBoss::STATE_OPEN:
		if (m_pModelCom->Get_isAnimEnd(STATE_OPEN)) {
			if (m_TotalTime >= 5.f) {
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

void CDoorBoss::Update_Alpha(_double Timedelta)
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

HRESULT CDoorBoss::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), m_iLevelIndex, TEXT("Prototype_Component_Model_DoorBoss"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_AABB */
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//ColliderDesc.vSize = _float3(5.f, 5.f, 5.f);
	ColliderDesc.fRadius = 10.f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CDoorBoss::SetUp_ConstantTable()
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

CDoorBoss * CDoorBoss::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CDoorBoss*	pInstance = new CDoorBoss(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CDoorBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDoorBoss::Clone(void * pArg)
{
	CDoorBoss*	pInstance = new CDoorBoss(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CDoorBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoorBoss::Free()
{
	__super::Free();	
	
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
