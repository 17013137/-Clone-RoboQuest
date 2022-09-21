#include "stdafx.h"
#include "..\Public\Fire_Effect.h"
#include "GameInstance.h"
#include "Boss_Manager.h"

CFire_Effect::CFire_Effect(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CFire_Effect::CFire_Effect(const CFire_Effect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFire_Effect::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFire_Effect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(165.f, 0.5f, 175.f, 1.f));

	Set_TowerBossBone();

	return S_OK;
}

_int CFire_Effect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (CBoss_Manager::Get_Instance()->Get_isFlameShoot() == false)
		return 1;

	if (FAILED(m_pVIBufferCom->Update(TimeDelta)))
		return 1;

	Set_TowerBossBone();

	m_AccTime += TimeDelta * 0.5f;

	if (m_AccTime > 1.f)
		m_AccTime = 0.f;

	return 0;
}

void CFire_Effect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_INSTANCE, this);

}

HRESULT CFire_Effect::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CFire_Effect::Set_TowerBossBone()
{
	if (m_iLevelIndex == LEVEL_BOSSTOWER) {
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		CTransform* BossTrans = (CTransform*)pGameInstance->Get_Transform(m_iLevelIndex, L"Layer_Enemy", 0);
		
		if (BossTrans == nullptr) {
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		_matrix WorldMtx = CBoss_Manager::Get_Instance()->Get_IK_Mtx();
//		m_pTransformCom->MyLookSet(BossTrans->Get_State(CTransform::STATE_LOOK));

		m_pTransformCom->MyLookSet(WorldMtx.r[0]);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMtx.r[3]);

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CFire_Effect::Update_Position()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CFire_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlameThrower"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_MaskTexture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"), (CComponent**)&m_pMaskTexCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Spray"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CFire_Effect::SetUp_ConstantTable()
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
	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pMaskTexCom->SetUp_ShaderResourceView(m_pShaderCom, "g_MaskTexture", 2)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("AccTime", &m_AccTime, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CFire_Effect * CFire_Effect::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CFire_Effect*	pInstance = new CFire_Effect(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CFire_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFire_Effect::Clone(void * pArg)
{
	CFire_Effect*	pInstance = new CFire_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CFire_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFire_Effect::Free()
{
	__super::Free();	
	
	Safe_Release(m_pMaskTexCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
