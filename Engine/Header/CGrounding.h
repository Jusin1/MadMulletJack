#pragma once
#include "CComponent.h"

BEGIN(Engine)

class CGameObject;
class CVIBuffer_GridPanelBase;
typedef struct tagPanelEntry PANELENTRY;

class ENGINE_DLL CGrounding : public CComponent
{
private:
	explicit CGrounding();
	explicit CGrounding(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGrounding(const CGrounding &rhs);
	virtual ~CGrounding();

	virtual void Free() override;
public:
	static CGrounding *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent *Clone(void *pArg = nullptr) override;

	HRESULT Ready_Component();
	virtual HRESULT Initialize(void *pArg);
public:
	_bool GetHeight(const vector<PANELENTRY> *pPanelEntries, _float fX, _float fZ, _float *fOutY);
	_uint GetCurrentIndex() const { return m_iCurrentIndex; }
	_bool Initialize_CurrentIndex(const vector<PANELENTRY> *pPanelEntries, _float fX, _float fZ, _float *fOutY);
private:
	_bool IsInside(const PANELENTRY &tPanelEntry, _float fX, _float fZ);
	_bool IsInside_Slope(const PANELENTRY &tPanelEntry, _float fX, _float fZ);
	_float Compute_Height(const PANELENTRY &tPanelEntry, _float fX, _float fZ);
private:
	_bool m_bGrounded{ true };
	constexpr static _float m_fGravity{ 9.8f };
	constexpr static _float m_fEpsilon{ 1e-4f };
	_int m_iCurrentIndex{ 0 };
};

END