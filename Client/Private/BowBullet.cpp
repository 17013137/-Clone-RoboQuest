#include "stdafx.h"
#include "..\Public\BowBullet.h"
#include "GameInstance.h"

CBowBullet::CBowBullet(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CBowBullet::CBowBullet(const CBowBullet & rhs)
	: CBullet(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CBowBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBowBullet::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->MyLookSet(m_vDir);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	Create_Trail();
	m_Damage = 50.f;
	return S_OK;
}

_int CBowBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	if (m_Dead == true)
		return 1;

	m_pTransformCom->Go_Straight(TimeDelta * 10.f);

	if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) > -0.8f) {
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_Gravitation);
		m_Gravitation += TimeDelta * 0.001f;
	}

	if (true == Collide_Object(L"Layer_Enemy")) {
		m_Dead = true;
	}

	if (m_TotalTime >= 10.f)
		m_Dead = true;


//	Bound_Bullet();
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_TotalTime += TimeDelta;
	
	return 0;
}

void CBowBullet::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CBowBullet::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;

	
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

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	//m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CBowBullet::Create_Trail()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	WRAP temp;
	temp.pTransform = m_pTransformCom;
	temp.pTarget = this;
	temp.fColor = _float3(0.8f, 0.8f, 0.8f);
	if (FAILED(pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"BulletTrail", L"Prototype_Effect_Trail_Bow", &temp)))
		int a = 10;

	RELEASE_INSTANCE(CGameInstance);
}

void CBowBullet::Bound_Bullet()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_PrevPos;
	_float3 NaviNormal = _float3();

	if (true == pGameinstance->CollideWithNavi(m_pSphereCom, vDir, &NaviNormal)) {
		if (m_BoundCnt > 0) {
			m_pTransformCom->MyLookSet(XMVector3Normalize(XMLoadFloat3(&NaviNormal)));
		}
		else
			m_Dead = true;
	}

	Safe_Release(pGameinstance);
}

HRESULT CBowBullet::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainMenu"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 0.5f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CBowBullet * CBowBullet::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBowBullet*	pInstance = new CBowBullet(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBowBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBowBullet::Clone(void * pArg)
{
	CBowBullet*	pInstance = new CBowBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBowBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBowBullet::Free()
{
	__super::Free();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
