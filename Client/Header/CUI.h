#pragma once
#include "CUIBase.h"
#include "CGlobal_Info.h"
class CUI : public CUIBase
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI(const CUI& rhs);
	virtual ~CUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

public:
	void Set_UIPosition(_float fX, _float fY, _float fSizeX, _float fSizeY)
	{
		m_fX = fX;
		m_fY = fY;
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
	}

	void		Move_UI(const _float& fTimeDelta); // UIMOVE에 따라 움직임을 줌

	// getter setter func
public:
	void Get_UIPosition(float& x, float& y) const { x = m_fX; y = m_fY; }
	void Get_UISize(float& w, float& h) const { w = m_fSizeX; h = m_fSizeY; }

	void Set_UIMoveInfo(UIMoveInfo _tMoveInfo) { m_tMoveInfo = _tMoveInfo; }
	UIMoveInfo Get_UIMoveInfo() const { return m_tMoveInfo; }
	void Set_UIMove(UIMOVE _eUIMove) { m_tMoveInfo.eUIMove = _eUIMove; }

protected:
	_matrix					m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	UIMoveInfo m_tMoveInfo; //ui 움직임시 필요한 정보를 담는 struct.

protected:
	virtual		HRESULT	Set_Component();
	virtual		HRESULT	Set_Texture() { return S_OK; }; // texture 변경 로직 담는 함수

public:
	void		Set_UISizeAndPos(_float _fSizeX, _float _fSizeY, _float _fX, _float _fY); // size부터 pos까지 셋팅
	void		Set_UISize(_float _fSizeX, _float _fSizeY); // size만 셋팅
	
public:
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

