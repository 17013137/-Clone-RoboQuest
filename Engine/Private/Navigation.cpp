#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "Transform.h"
#include "PipeLine.h"
#include "Object_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CNavigation)

CNavigation::CNavigation()
{
}

HRESULT CNavigation::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar * pNavigationData, CTransform* pTransform)
{
	_ulong			dwByte = 0;

	HANDLE			hFile = CreateFile(pNavigationData, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;
	
	if (m_Cells.size() != 0) {
		for (auto& iter : m_Cells)
			iter->Free();
	}

	m_Cells.clear();

	while (true)
	{
		CELLDESC vPoint;
		ReadFile(hFile, &vPoint, sizeof(CELLDESC), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell*			pCell = CCell::Create(pDevice, pDeviceContext, &vPoint, m_Cells.size());

		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	m_pTransform = pTransform;
	Safe_AddRef(m_pTransform);

	if (FAILED(SetUp_Neighbor()))
		return E_FAIL;

#ifdef _DEBUG	
	m_pDebugShader = CShader::Create(pDevice, pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pDebugShader)
		return E_FAIL;
#endif // _DEBUG
	return S_OK;
}

_bool CNavigation::Move_OnNavigation(_float3* vPosition, _uint * pCurrentIndex, _fvector PlayerDir, _bool isJump)
{
	if (*pCurrentIndex >= m_Cells.size())
		return false;	

	_matrix		WorldMatrix = m_pTransform->Get_WorldMatrix();

	_int		iNeighborIndex = -1;
	if (true == m_Cells[*pCurrentIndex]->isIn(vPosition, WorldMatrix, &iNeighborIndex, PlayerDir, isJump))
		return true;
	else
	{
		/* 이웃이 있는지 없는지 검사를 하고. */
		/* 이웃이 있었다면. */
		if (-1 != iNeighborIndex)
		{
			
			while (true)
			{
				if (-1 == iNeighborIndex) {

					return false;
				}

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, WorldMatrix, &iNeighborIndex, PlayerDir, isJump))
				{
					/* 그 이웃의 인덱스를 pCurrentIndex에 채워준다. */
					*pCurrentIndex = iNeighborIndex;
					return true;
				}

				m_NowIndex = iNeighborIndex;
				if (m_PrevIndex == m_NowIndex) {
					return false;
				}

				m_PrevIndex = m_NowIndex;
			}
		}
		/* 이웃이 없다면. */
		else {
			return false;
		}
	}

	return _bool();
}

_bool CNavigation::Sky_Move_OnNavi(_fvector vPosition, _uint * pCurrentIndex)
{
	if (*pCurrentIndex >= m_Cells.size())
		return false;

	_matrix		WorldMatrix = m_pTransform->Get_WorldMatrix();

	_int		iNeighborIndex = -1;

	if (0 == m_Cells[*pCurrentIndex]->isOn(vPosition, WorldMatrix, &iNeighborIndex))
		return true;
	else
	{
		/* 이웃이 있는지 없는지 검사를 하고. */
		/* 이웃이 있었다면. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;
				_int flag = m_Cells[iNeighborIndex]->isOn(vPosition, WorldMatrix, &iNeighborIndex);
				if (flag == 0)
				{
					/* 그 이웃의 인덱스를 pCurrentIndex에 채워준다. */
					*pCurrentIndex = iNeighborIndex;

					return true;
				}
				else if (flag == 2) {
					return false;
				}
			}
		}
		/* 이웃이 없다면. */
		else
			return false;
	}

	return _bool();
}

_bool CNavigation::CollideWithNavi(CCollider* pColide, _fvector vDir, _float3* Normal)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	if (pColide == nullptr)
		return false;
	
	for (_int i = 0; i < m_Cells.size(); i++) {
		if (pColide->Collision_Point(m_Cells[i]->Get_Point(0), m_Cells[i]->Get_Point(1), m_Cells[i]->Get_Point(2)) == true) {
			XMStoreFloat3(Normal, XMVector3Normalize(XMVector3Cross(m_Cells[i]->Get_Point(1) - m_Cells[i]->Get_Point(0), m_Cells[i]->Get_Point(2) - m_Cells[i]->Get_Point(0))));
			return true;
		}
	}

	Safe_Release(pGameinstance);

	return false;
}

_int CNavigation::Search_MyIndex(_fvector vPosition)
{
	_int CurrentIndex = 0;
	for (int i = 0; i < m_Cells.size(); i++) {
		if (m_Cells[i]->isOn(vPosition, m_pTransform->Get_WorldMatrix(), &CurrentIndex) == 0) {
			return CurrentIndex;
		}
	}

	return -1;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (nullptr == m_pTransform)
		return E_FAIL;

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pDebugShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pDebugShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pDebugShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));
	m_pDebugShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Render(m_pDebugShader);
	}

	RELEASE_INSTANCE(CPipeLine);

	return S_OK;
}

HRESULT CNavigation::Render_Cell(_int iIndex)
{
	if (nullptr == m_pTransform)
		return E_FAIL;

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pDebugShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pDebugShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pDebugShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));
	m_pDebugShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

	m_Cells[iIndex]->Render(m_pDebugShader);

	RELEASE_INSTANCE(CPipeLine);

	return S_OK;	
}

HRESULT CNavigation::Push_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CELLDESC * Cell)
{
	CCell*			pCell = CCell::Create(pDevice, pDeviceContext, Cell, m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.push_back(pCell);

	return S_OK;
}

HRESULT CNavigation::Remove_LastCell()
{
	if (m_Cells.size() == 0)
		return E_FAIL;
	
	m_Cells.pop_back();

	list<CGameObject*>* boxlist = CObject_Manager::GetInstance()->Get_GameObjectList(CGameInstance::GetInstance()->Get_LevelIndex(), L"NaviBox");

	for (int i = 0; i < 3; i++) {
		CGameObject* box = boxlist->back();
		boxlist->pop_back();
		if (box == nullptr)
			return E_FAIL;

		box->Set_Dead(true);
	}

	return S_OK;
}



#endif // _DEBUG

HRESULT CNavigation::SetUp_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if(true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_AB, pDestCell->Get_Index());
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_BC, pDestCell->Get_Index());
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_CA, pDestCell->Get_Index());
			}
		}
	}
	return S_OK;
}

void CNavigation::Free()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

	Safe_Release(m_pTransform);

#ifdef _DEBUG
	Safe_Release(m_pDebugShader);
#endif // _DEBUG
}
