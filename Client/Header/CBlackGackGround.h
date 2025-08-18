#pragma once
#include "CUI.h"
class CBlackGackGround : public CUI
{
private:
	explicit CBlackGackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBlackGackGround(const CBlackGackGround& rhs);
	virtual ~CBlackGackGround();

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	Render_GameObject() override;

public: // 舅颇, 其捞靛
	void SetAlpha(BYTE alpha) { m_alpha = alpha; }
	void FadeTo(BYTE target, _float delay, _float duration);

public: // 备港 墩扁
	void SetHoleRect(_float x, _float y, _float w, _float h);
	void ClearHole() { m_useHole = false; }

public:
	static CBlackGackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	void DrawSolidQuad(_float cx, _float cy, _float w, _float h);

private: // 其捞靛
	BYTE  m_alpha;
	BYTE  m_start;
	BYTE  m_target;
	_bool m_fadeActive;
	_float m_t;
	_float m_delay;
	_float m_dur;

private: // 备港
	_bool  m_useHole;
	_float m_holeX;
	_float m_holeY;
	_float m_holeW;
	_float m_holeH;
};

