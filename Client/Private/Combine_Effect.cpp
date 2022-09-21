#include "stdafx.h"
#include "..\Public\Combine_Effect.h"
#include "GameInstance.h"

CCombine_Effect::CCombine_Effect(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CCombine_Effect::CCombine_Effect(const CCombine_Effect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCombine_Effect::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCombine_Effect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Update_Position();

	CSound_Manager::GetInstance()->SoundPlay(L"LevelUp.ogg", 11, 1.f);

	return S_OK;
}

_int CCombine_Effect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Update_Position();

	if (FAILED(m_pVIBufferCom->Update(TimeDelta)))
		return 1;

	return 0;
}

void CCombine_Effect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_INSTANCE, this);
}

HRESULT CCombine_Effect::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CCombine_Effect::Update_Position()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* WeaponTrans = pGameInstance->Get_Transform(m_iLevelIndex, L"Layer_Weapon");
	
	RELEASE_INSTANCE(CGameInstance);

	if (WeaponTrans == nullptr)
		return;

	_vector vPos = WeaponTrans->Get_State(CTransform::STATE_POSITION) - XMVector3Normalize(WeaponTrans->Get_State(CTransform::STATE_LOOK)) + XMVector3Normalize(WeaponTrans->Get_State(CTransform::STATE_UP))*0.1f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

HRESULT CCombine_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UpgradeBall"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Combine"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCombine_Effect::SetUp_ConstantTable()
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


	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CCombine_Effect * CCombine_Effect::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CCombine_Effect*	pInstance = new CCombine_Effect(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CCombine_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCombine_Effect::Clone(void * pArg)
{
	CCombine_Effect*	pInstance = new CCombine_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CCombine_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCombine_Effect::Free()
{
	__super::Free();	
	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
