#include "stdafx.h"
#include "..\Public\MuzzleEffect.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "Weapon_Effect_Manager.h"

CMuzzleEffect::CMuzzleEffect(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CMuzzleEffect::CMuzzleEffect(const CMuzzleEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMuzzleEffect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMuzzleEffect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* PlayerTrans = (CTransform*)pGameInstance->Get_Component(m_iLevelIndex, L"Layer_Player", m_pTransformTag);

	RELEASE_INSTANCE(CGameInstance);
	
	if (PlayerTrans == nullptr)
		return E_FAIL;

	_vector vPos = PlayerTrans->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CWeapon_Effect_Manager::Get_Instance()->Get_MuzzlePos());
	m_pTransformCom->Scaled(_float3(0.3f, 0.3f, 0.3f));
	m_pTransformCom->LookAt(vPos);
	return S_OK;
}

_int CMuzzleEffect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	if (CPlayer_Manager::Get_Instance()->Get_isAttack()) {
		if (m_isAttack == false && m_EffectEnd == false) {
			m_isAttack = true;
			m_Frame = 0.f;
		}
	}
	else
		m_EffectEnd = false;

	if (CPlayer_Manager::Get_Instance()->Get_NowWeapon() >= 6 && CPlayer_Manager::Get_Instance()->Get_NowWeapon() <= 8) {
		m_ImgIndex = 1.f;
		m_FrameMax = 5.f;
	}
	else {
		m_ImgIndex = 0.f;
		m_FrameMax = 5.f;
	}

	if (m_isAttack == true)
		m_Frame++;
	if (m_Frame >= m_FrameMax) {
		m_isAttack = false;
		m_EffectEnd = true;
		m_Frame = 0.f;
	}

	

	return 0;
}

void CMuzzleEffect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* CameraTrans = (CTransform*)pGameInstance->Get_Component(m_iLevelIndex, L"Layer_Camera", m_pTransformTag);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CWeapon_Effect_Manager::Get_Instance()->Get_MuzzlePos());
	m_pTransformCom->LookAt(CameraTrans->Get_State(CTransform::STATE_POSITION));

	RELEASE_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
}

HRESULT CMuzzleEffect::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMuzzleEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MuzzleEffect"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMuzzleEffect::SetUp_ConstantTable()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 5)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("XYIndex", &_float2(5, 5), sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("Frame", &m_Frame, sizeof(float))))
		return E_FAIL;
	//
	if (FAILED(m_pShaderCom->Set_RawValue("OnOff",&m_isAttack, sizeof(bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("ImgIndex", &m_ImgIndex, sizeof(float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMuzzleEffect * CMuzzleEffect::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CMuzzleEffect*	pInstance = new CMuzzleEffect(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CMuzzleEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMuzzleEffect::Clone(void * pArg)
{
	CMuzzleEffect*	pInstance = new CMuzzleEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CMuzzleEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMuzzleEffect::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
