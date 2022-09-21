#include "stdafx.h"
#include "..\Public\ExpBall.h"
#include "GameInstance.h"
#include "Player_Manager.h"

CExpBall::CExpBall(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CExpBall::CExpBall(const CExpBall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CExpBall::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExpBall::NativeConstruct(void * pArg)
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

	m_pTransformCom->LookAt(PlayerTrans->Get_State(CTransform::STATE_POSITION));

	if (pArg == nullptr)
		return E_FAIL;
	_vector vPos;
	memcpy(&vPos, pArg, sizeof(_vector));
	DefaultY = XMVectorGetY(vPos);
	m_vDir = XMVector3Normalize(XMVectorSet(_float(rand() % 10 + 1) - 5, 0.f, _float(rand() % 10 + 1) - 5, 0.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Scaled(_float3(1.f, 1.f, 1.f));
	m_JumpDesc.fPower = _float((rand() % 30) /10.f) + 5.f;
	m_JumpDesc.vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Create_Trail();
	return S_OK;
}

_int CExpBall::Tick(_double TimeDelta)
{
	if (__super::Tick(TimeDelta) == 1) {
		m_ImgIndex++;
		if (m_ImgIndex >= 14)
			return 1;
		else
			return 0;
	}

	m_TotalTime += TimeDelta;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* PlayerTrans = (CTransform*)pGameInstance->Get_Component(m_iLevelIndex, L"Layer_Player", m_pTransformTag);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(PlayerTrans->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_vector Result = vPos;
	_float Dist = XMVectorGetX(XMVector3Length(PlayerTrans->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	
	_float fY = (m_JumpDesc.fPower * m_JumpDesc.fTime * (1 + (m_JumpDesc.fSpeed * 0.2f))) - ((9.81f * m_JumpDesc.fTime * m_JumpDesc.fTime) / 2.f) + XMVectorGetY(m_JumpDesc.vOriginPos);

	if (fY < DefaultY && IdleState == false) {
		IdleState = true;
		m_DownPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_UpPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_UpPos = XMVectorSetY(m_UpPos, XMVectorGetY(m_UpPos) + 0.7f);
	}

	if (Dist < 7) {
		m_isTargetOn = true;
		if (pGameInstance->Collison_Sphere_Layer(m_iLevelIndex, this, L"Layer_Player") != nullptr) {
			CPlayer_Manager::Get_Instance()->Set_Exp(1);
			m_Dead = true;
		}
	}

	if (IdleState == false) {
		Result = XMLoadFloat4(&_float4(XMVectorGetX(vPos) + XMVectorGetX(m_vDir) * 0.05f, fY, XMVectorGetZ(vPos) + XMVectorGetZ(m_vDir) * 0.05f, 1));
		m_JumpDesc.fTime += (_float)TimeDelta;
	}
	else {
		if (m_isTargetOn == false) {
			if (m_isDown == false) {
				if (m_UpDownTime >= 1.f) {
					m_UpDownTime = 1;
					m_isDown = true;
				}
				Result = XMVectorLerp(m_DownPos, m_UpPos, m_UpDownTime);
				m_UpDownTime += (_float)TimeDelta;
			}
			else {
				if (m_UpDownTime <= 0.f) {
					m_UpDownTime = 0;
					m_isDown = false;
				}

				Result = XMVectorLerp(m_DownPos, m_UpPos, m_UpDownTime);
				m_UpDownTime -= (_float)TimeDelta;
			}
		}
		else {
			Result += vDir *(0.07f) * Dist;
		}
	}
	
	

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Result);

	m_pTransformCom->LookAt(PlayerTrans->Get_State(CTransform::STATE_POSITION));

	RELEASE_INSTANCE(CGameInstance);

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	return 0;
}

void CExpBall::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
}

HRESULT CExpBall::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

//#ifdef _DEBUG
//	m_pSphereCom->Render();
//#endif // _DEBUG

	return S_OK;
}

void CExpBall::Create_Trail()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	WRAP temp;
	temp.pTarget = this;
	temp.pTransform = m_pTransformCom;
	if (FAILED(pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"BulletTrail", L"Prototype_Effect_BulletTrail", &temp)))
		int a = 10;

	RELEASE_INSTANCE(CGameInstance);

}

HRESULT CExpBall::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ExpBall"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 1.f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CExpBall::SetUp_ConstantTable()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", m_ImgIndex)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CExpBall * CExpBall::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CExpBall*	pInstance = new CExpBall(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CExpBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CExpBall::Clone(void * pArg)
{
	CExpBall*	pInstance = new CExpBall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CExpBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExpBall::Free()
{
	__super::Free();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
