#include "stdafx.h"
#include "..\Public\ExpTrail.h"
#include "GameInstance.h"

CExpTrail::CExpTrail(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CExpTrail::CExpTrail(const CExpTrail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CExpTrail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExpTrail::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	WRAP temp;

	memcpy(&temp, pArg, sizeof(WRAP));

	
	m_Target = temp.pTarget;

	if (m_Target == nullptr)
		return E_FAIL;

	m_pVIBufferCom->Set_TrailPoint(XMVectorSet(-0.1f, 0.f, -0.5f, 0.f), XMVectorSet(0.1f, 0.f, -0.5f, 0.f));

	m_pVIBufferCom2->Set_TrailPoint(XMVectorSet(0.f, 0.1f,0.f, 0.f), XMVectorSet(0.f, -0.1f, 0.f, 0.f));

	m_pVIBufferCom->Set_MaxFrame(2);

	m_pVIBufferCom2->Set_MaxFrame(2);

	//버퍼렉트2 릴리즈

	return S_OK;
}

_int CExpTrail::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	if (m_Target->Get_isDead() == true) {
		m_Dead = true;
		return 1;
	}

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);

	CTransform* CameraTrans = (CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Camera", m_pTransformTag);
	_matrix CameraMtx = CameraTrans->Get_WorldMatrix();

	RELEASE_INSTANCE(CGameInstance);

	//_matrix Billboard = XMMatrixIdentity();
	//_matrix temp;
	//Billboard = XMMatrixSet(XMVectorGetX(CameraMtx.r[0]), 0.f, XMVectorGetZ(CameraMtx.r[0]), 0.f, 0.f, 1.f, 0.f, 0.f, XMVectorGetX(CameraMtx.r[2]), 0.f, XMVectorGetZ(CameraMtx.r[2]), 0.f, 0.f, 0.f, 0.f, 1.f);
	//temp = XMMatrixInverse(nullptr, Billboard);
	//temp.r[3] = m_TargetTrans->Get_State(CTransform::STATE_POSITION);
	//temp.r[3] = XMVectorSetW(temp.r[3], 1.f);

	m_pVIBufferCom->Update(TimeDelta,m_Target->Get_Transform()->Get_WorldMatrix());
	m_pVIBufferCom2->Update(TimeDelta, m_Target->Get_Transform()->Get_WorldMatrix());

	//_matrix Billboard = XMMatrixIdentity();


	return 0;
}

void CExpTrail::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_CamearaDistance();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
}

HRESULT CExpTrail::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom2->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExpTrail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Trail"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CExpTrail::SetUp_ConstantTable()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("MyColor", &m_Color, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CExpTrail * CExpTrail::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CExpTrail*	pInstance = new CExpTrail(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CExpTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CExpTrail::Clone(void * pArg)
{
	CExpTrail*	pInstance = new CExpTrail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CExpTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExpTrail::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pRendererCom);

}
