#include "stdafx.h"
#include "..\Public\FlameTriangle.h"
#include "GameInstance.h"

CFlameTriangle::CFlameTriangle(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CFlameTriangle::CFlameTriangle(const CFlameTriangle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFlameTriangle::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlameTriangle::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	Set_Position();

	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

_int CFlameTriangle::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_AccDuration >= m_Duration)
		return 1;

	Collide_Target(L"Layer_Player");

	m_AccTime += (_float)TimeDelta * 0.05f; 

	m_AccDuration += TimeDelta;

	return 0;
}

void CFlameTriangle::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
}

HRESULT CFlameTriangle::Render()
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
	//m_pOBBCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CFlameTriangle::Set_Position()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* BossTrans =  (CTransform*)pGameInstance->Get_Component(m_iLevelIndex, L"Layer_Enemy", m_pTransformTag, 0);
	if (BossTrans == nullptr)
		return;

	_vector vPos = BossTrans->Get_State(CTransform::STATE_POSITION);

	vPos = XMVectorSetY(vPos, 0.5f);

	_vector vLook = BossTrans->Get_State(CTransform::STATE_LOOK);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->MyLookSet(vLook);

	RELEASE_INSTANCE(CGameInstance);
}

void CFlameTriangle::Collide_Target(const _tchar * LayerTag)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	CCollider* PlayerCollide = (CCollider*)pGameinstance->Get_Component(m_iLevelIndex, LayerTag, L"Com_AABB");
	if (PlayerCollide == nullptr)
		return;
	CStatus* PlayerStatus = (CStatus*)pGameinstance->Get_Component(m_iLevelIndex, LayerTag, L"Com_Status");

	if (m_pOBBCom->InterSectAABB(*PlayerCollide->Get_AABBWorld()) == true) {
		_vector Dir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		PlayerStatus->Damaged(m_Damage, CStatus::HIT_STATE_KNOCKBACK, 5.f, &Dir);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CFlameTriangle::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlameTriangle"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPosition = _float3(0.f, 0.0f, 30.f);
	ColliderDesc.vSize = _float3(15.f, 2.5f, 30.f);

	if (FAILED(__super::SetUp_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FirePattern"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlameTriangle::SetUp_ConstantTable()
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
	if (FAILED(m_pShaderCom->Set_RawValue("AccTime", &m_AccTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_PatternTex", 0)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}



CFlameTriangle * CFlameTriangle::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CFlameTriangle*	pInstance = new CFlameTriangle(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CFlameTriangle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlameTriangle::Clone(void * pArg)
{
	CFlameTriangle*	pInstance = new CFlameTriangle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CFlameTriangle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlameTriangle::Free()
{
	__super::Free();	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
