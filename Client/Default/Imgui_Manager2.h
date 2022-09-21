#pragma once

#include "Base.h"

BEGIN(Client)

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

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
	bool WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Shutdown(void);

private:
	bool m_bImguiEnable = false;

public:
	void OnOffImgui(void) { m_bImguiEnable = !m_bImguiEnable; }
	bool isImguiEnable() { return m_bImguiEnable; }

public:
	virtual void Free() override;
};

END