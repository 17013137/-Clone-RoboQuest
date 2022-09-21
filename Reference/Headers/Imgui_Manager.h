#pragma once

#include "Base.h"

BEGIN(Engine)

class CTransform;

class ENGINE_DLL CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)
public:
	char* CellIndex[5]{ "Normal", "Hegiht", "Block", "Door", "Temp"};

public:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT InitImGui(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT StartFrame(void);
	HRESULT Set_Contents(void);
	HRESULT Render(void);

public:
	void InitWin32(HWND hWnd);
	LRESULT WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Shutdown(void);

public:
	void OnOffImgui(void) { m_bImguiEnable = !m_bImguiEnable; }
	bool isImguiEnable() { return m_bImguiEnable; }

public:
	void Get_NaviPoint(class CGameObject* Obj);
	void Undo_NaviBox();
	void Move_NaviBox();
	_bool Get_isNaviMode() { return m_isNaviMode; }

private:
	_bool Save_NaviFile();
	HRESULT Load_NaviFile();
	void Save_Text();
	void Push_Cell();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_DeviceContext = nullptr;


public:
	_int m_PointCnt = 0;
	_int m_CellIndex = 0;
	_float m_SetY = 0.0f;
	bool m_isNaviMode = false;
	bool m_bImguiEnable = false;
	vector<CELLDESC> m_Cell;
	vector<CGameObject*> m_vNaviBox;
	CTransform* m_Current_Transform = nullptr;

private:
	CGameObject* m_NaviObj[3] = { nullptr };
	CELLDESC m_Point = { _float3() };

	_int m_isSuccess = 2;

public:
	virtual void Free() override;
};

END