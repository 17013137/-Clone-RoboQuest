#include "stdafx.h"
#include "..\Public\RedSmoke.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "KeyMgr.h"
CRedSmoke::CRedSmoke(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CRedSmoke::CRedSmoke(const CRedSmoke & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRedSmoke::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CRedSmoke::NativeConstruct(void * pArg)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(175.f, 0.f, 170.f, 1.f));
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

_int CRedSmoke::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_TotalTime += TimeDelta;

	Anim_Tick(TimeDelta);

	m_pModelCom->Update(TimeDelta);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	Update_Alpha(TimeDelta*2.f);

	return 0;
}

void CRedSmoke::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CRedSmoke::Render()
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


		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, m_BlackWhite)))
			return E_FAIL;
	}	
#ifdef _DEBUG
	//m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CRedSmoke::Anim_Tick(_double TImeDelta)
{
	if (m_Trigger == true)
		m_State = STATE_EVENT;

	switch (m_State)
	{
	case Client::CRedSmoke::STATE_IDLE:
		m_pModelCom->Set_AnimationIndex(m_State, 1.0, 1.0);
		break;
	case Client::CRedSmoke::STATE_EVENT:
		if (m_pModelCom->Get_isAnimEnd(m_State) == true) {
			m_BlackWhite = 2;
			//m_pModelCom->Set_FinishFalse(m_State);
		}
		else
			m_pModelCom->Set_AnimationIndex(m_State, 1.0, 1.0, false);
		break;
	default:
		break;
	}
}

void CRedSmoke::Update_Alpha(_double Timedelta)
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

HRESULT CRedSmoke::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RedSmoke"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_AABB */
	ColliderDesc.vPosition = _float3(0.f, 2.f, 0.f);
	ColliderDesc.fRadius = 5.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CRedSmoke::SetUp_ConstantTable()
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

_bool CRedSmoke::Heal_Player()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CStatus* Status = (CStatus*)pGameInstance->Get_Component(m_iLevelIndex, L"Layer_Player", L"Com_Status");
	pGameInstance->Add_GameObjectToLayer(m_iLevelIndex, L"Recovering", L"Prototype_UI_Recovering");

	RELEASE_INSTANCE(CGameInstance);

	if (Status == nullptr)
		return false;

	Status->Heal(50.f);

	return true;
}

CRedSmoke * CRedSmoke::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CRedSmoke*	pInstance = new CRedSmoke(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CRedSmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRedSmoke::Clone(void * pArg)
{
	CRedSmoke*	pInstance = new CRedSmoke(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CRedSmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRedSmoke::Free()
{
	__super::Free();	
	
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
