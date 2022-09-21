#include "stdafx.h"
#include "..\Public\CrossHairRect.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "Event_Manager.h"

CCrossHairRect::CCrossHairRect(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CCrossHairRect::CCrossHairRect(const CCrossHairRect & rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)	
{
}

HRESULT CCrossHairRect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	
	return S_OK;
}

HRESULT CCrossHairRect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fX = g_iWinCX * 0.5f;
	m_fY = g_iWinCY * 0.5f;

	m_fSizeX = 1.0f;
	m_fSizeY = 3.f;

	if (pArg != nullptr) {
		_float4 vData;   //x(fX) y(fY) z(fSizeX) w(fSizeY)

		memcpy(&vData, pArg, sizeof(_vector));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vData.x - g_iWinCX * 0.5f, -vData.y + g_iWinCY * 0.5f, 0.f, 1.f));
		m_pTransformCom->Scaled(_float3(vData.z, vData.w, 1.f));

		if (vData.y > g_iWinCY * 0.5f) {
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f));
		}

		if (vData.x < g_iWinCX*0.5f) {
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));
		}

		if (vData.x > g_iWinCX*0.5f) {
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(90.f));
		}


	}
	else {
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
		m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	}

	m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	return S_OK;
}

_int CCrossHairRect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_OriginPos);

	if(CPlayer_Manager::Get_Instance()->Get_PlayerState() == 1) {
		m_isAttack = true;
		if (m_CrossHairDesc.Max > m_CrossHairDesc.Now) {
			m_pTransformCom->Go_Up(m_CrossHairDesc.Speed);
			m_CrossHairDesc.Now += m_CrossHairDesc.Speed;
		}

	}
	else {
		m_isAttack = false;
		if (m_CrossHairDesc.Now > 0.0) {
			m_pTransformCom->Go_Down(m_CrossHairDesc.Speed);
			m_CrossHairDesc.Now -= m_CrossHairDesc.Speed;
		}
		else {
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_OriginPos);
		}
	}


	return 0;
}

void CCrossHairRect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom && CEvent_Manager::Get_Instance()->Get_Air() == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_UI, this);
}

HRESULT CCrossHairRect::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	
	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4	ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4	ProjMatrixTP;
	XMStoreFloat4x4(&ProjMatrixTP, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrixTP, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrossHairRect::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CrossHair"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CCrossHairRect * CCrossHairRect::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CCrossHairRect*	pInstance = new CCrossHairRect(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CCrossHairRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCrossHairRect::Clone(void * pArg)
{
	CCrossHairRect*	pInstance = new CCrossHairRect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CCrossHairRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrossHairRect::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
