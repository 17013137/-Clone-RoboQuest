#include "stdafx.h"
#include "..\Public\Joy.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"
CJoy::CJoy(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CJoy::CJoy(const CJoy & rhs)
	: CGameObject(rhs)
{
}

HRESULT CJoy::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CJoy::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	m_pModelCom->Set_AnimationIndex(0, 1.0, 1.0);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(165.f, 0.f, 175.f, 1.f));
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

_int CJoy::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_TotalTime > 10.0) {
		m_TotalTime = 0.0;
		m_State = NORMAL_STATE(rand() % 2 + 1);
	}

	m_TotalTime += TimeDelta;

	Anim_Tick(TimeDelta);

	m_pModelCom->Update(TimeDelta);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	Update_Alpha(TimeDelta*2.0f);

	return 0;
}

void CJoy::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider* pTargetCollider = (CCollider*)pGameInstance->Get_Component(m_iLevelIndex, TEXT("Layer_Player"), TEXT("Com_Sphere"));

	if (nullptr != pTargetCollider) {
		BoundingSphere* TargetSphere = pTargetCollider->Get_SphereWorld();

		if (TargetSphere != nullptr) {
			if (m_pSphereCom->InterSectSphere(*TargetSphere)) {
				m_ShaderCnt = 1;
			}
			else
				m_ShaderCnt = 0;
		}
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CJoy::Render()
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


		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, m_ShaderCnt)))
			return E_FAIL;
	}	
#ifdef _DEBUG
	//m_pAABBCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CJoy::Anim_Tick(_double TImeDelta)
{
	switch (m_State)
	{
	case Client::CJoy::STATE_BASE:
		m_pModelCom->Set_AnimationIndex(m_State, 1.0, 1.0);
		break;
	case Client::CJoy::STATE_SPECAIL1:
		if (m_pModelCom->Get_isAnimEnd(m_State) == true) {
			m_pModelCom->Set_FinishFalse(m_State);
			m_State = STATE_BASE;
		}
		else
			m_pModelCom->Set_AnimationIndex(m_State, 1.0, 1.0);
		break;
	case Client::CJoy::STATE_SPECAIL2:
		if (m_pModelCom->Get_isAnimEnd(m_State) == true) {
			m_pModelCom->Set_FinishFalse(m_State);
			m_State = STATE_BASE;
		}
		else
			m_pModelCom->Set_AnimationIndex(m_State, 1.0, 1.0);
		break;
	case Client::CJoy::STATE_SPECAIL3:
		if (m_pModelCom->Get_isAnimEnd(m_State) == true) {
			m_pModelCom->Set_FinishFalse(m_State);
			m_State = STATE_BASE;
		}
		else
			m_pModelCom->Set_AnimationIndex(m_State, 1.0, 1.0);
		break;
	default:
		break;
	}
}

void CJoy::Update_Alpha(_double Timedelta)
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

HRESULT CJoy::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), m_iLevelIndex, TEXT("Prototype_Component_Model_Joy"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_AABB */
	ColliderDesc.vPosition = _float3(0.f, 2.5f, 0.f);
	ColliderDesc.fRadius = 5.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CJoy::SetUp_ConstantTable()
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

CJoy * CJoy::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CJoy*	pInstance = new CJoy(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CJoy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJoy::Clone(void * pArg)
{
	CJoy*	pInstance = new CJoy(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CJoy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJoy::Free()
{
	__super::Free();	
	
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
