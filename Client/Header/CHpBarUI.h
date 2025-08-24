#pragma once
#include "CUI.h"

class CHpBarUI : public CUI
{
private:
    explicit CHpBarUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHpBarUI(const CHpBarUI& rhs);
    virtual ~CHpBarUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta) override;
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual			void		Render_GameObject() override;

public:
	void HitCount_Up() { m_iHitCount++; m_bHitChange = true; } // hitcount 조절 함수
	void HitCount_Down() { m_iHitCount--; m_bHitChange = true;}
	void HitCount_Reset() { m_iHitCount = 0; m_bHitChange = true;}

	void	Set_Hp(_float _fMaxHp, _float _fCurHp); // player에서 hp 전해줌

private:
	virtual HRESULT			Set_Component();

	_bool Is_Scene_Change(); // 플레이어의 상태가 바뀌었는지 + 변화값 받음

private:
	HRESULT Set_HpBarUI();
	
	//getter setter func
public:
	SCENE Get_Scene() const { return m_eScene; }
	void Set_Scene(SCENE _eScene) { m_eScene = _eScene; }

	void Set_HpPercent(_float _fHpPercent) { m_fHpPercent = _fHpPercent; }
	_float Get_HpPercent() const { return m_fHpPercent; }
	void Set_HitCount(_int _iHitCount) { m_iHitCount = _iHitCount; }
	_int Get_HitCount()const { return m_iHitCount; }
	
private:
	SCENE m_eScene;

    _float		m_fHpPercent; // 체력 비율 (0~1)
	_int		m_iHitCount;
	_bool		m_bHitChange;
	_float		m_fRectY; // rect 초기 사이즈 저장하기 위함

public:
	static  CHpBarUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

