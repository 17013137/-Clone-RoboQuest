#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCell() = default;

public:
	_int Get_Index() const {return m_iIndex;}
	_vector Get_Point(_int ePoint) {return XMLoadFloat3(&m_vPoint[ePoint]);}
	_float3 Get_fPoint(_int Point_Index) { return m_vPoint[Point_Index]; }
	_vector Get_Line(_int Line_Index) { return m_vLine[Line_Index]; }
	_int Get_Neighbor(_int Line_Index) { return m_iNeighbor[Line_Index]; }
	void Set_NeighborIndex(LINE eLine, _int iIndex) {m_iNeighbor[eLine] = iIndex;}
	_bool isInCollider(_fvector Position, _float Radius);
	_vector Get_Plane() { return m_Plane; }

public:
	HRESULT NativeConstruct(CELLDESC* CellDesc, _int iIndex);
	_bool isIn(_float3* vPosition, _fmatrix WorldMatrix, _int* pNeighborIndex, _fvector PlayerDir, _bool isJump = false);
	_int isOn(_fvector vPosition, _fmatrix WorldMatrix, _int* pNeighborIndex);
	_bool Compare_Points(_vector vSour, _vector vDest);

#ifdef _DEBUG
public:
	HRESULT Render(class CShader* pShader);
#endif // _DEBUG

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	_float3				m_vPoint[POINT_END];
	_int				m_iIndex = 0;
	_int				m_TypeIndex = 0;
	_int				m_iNeighbor[LINE_END] = { -1, -1, -1 };
	_vector				m_vLine[LINE_END];
	_float				m_MaxHegiht = 0.0f;
	_float				m_MinHeight = (_float)INT_MAX;
	_vector				m_vNormal;
	_vector				m_Plane;

	_bool m_isCulling = false;

#ifdef _DEBUG
private:
	class CVIBuffer_Line*		m_pDebugBuffer = nullptr;
#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CELLDESC* CellDesc, _int iIndex);
	virtual void Free() override;
};

END