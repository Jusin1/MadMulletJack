#pragma once
#include "CUI.h"
class CHeartUI :
    public CUI
{
private:
	explicit CHeartUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHeartUI(const CHeartUI& rhs);
	virtual ~CHeartUI();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void	Render_GameObject() override;

public:
	static CHeartUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public: // 배치 전용 함수
	void SetHeartSizePx(_float px);
	void SetHeartManual(_float xLeft, _float xRight, _float y);

	void SetLineHeightPx(_float px);
	void SetLineYOffset(_float py);
	void SetLineRangePx(_float xL, _float xR);
	void SetLineTint(D3DCOLOR tint);

	void SetPulseStyle(_float widthPx, _float heightPx, _float yOffsetPx, D3DCOLOR tint);
	void SetPulseSpeed(_float pxPerSec);
	void SetPulseStartOffsetPx(_float px);
	void SetBeatTrackPx(_float xL, _float xR);
	void SetBeatYOffset(_float py);

protected:
	HRESULT	Texture_Clone();
	void	RenderBaseline();
	void	RenderPulse();
	void	RenderHearts();

protected: // ===== 텍스처 =====
	CTexture* m_pTexHeartL;
	CTexture* m_pTexHeartR;
	CTexture* m_pTexLine;
	CTexture* m_pTexPulse;

protected: // ===== 심장 =====
	_float m_heartSizePx;
	_float m_heartPosLX;
	_float m_heartPosRX;
	_float m_heartPosY;

protected: // ===== 선 =====
	_float m_lineHeightPx;
	_float m_lineOffY;
	_float m_lineXL;
	_float m_lineXR;
	D3DCOLOR m_lineTint;

protected: // ===== 비트 =====
	_bool  m_pulseEnabled;
	_float m_pulseWidthPx;
	_float m_pulseHeightPx;
	_float m_pulseOffY;
	_float m_pulseSpeed;
	_float m_pulsePhase;
	D3DCOLOR m_pulseTint;
	_float m_beatXL;
	_float m_beatXR;
	_bool  m_beatYOverride;
	_float m_beatY;
};

