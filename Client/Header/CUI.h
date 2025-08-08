#pragma once
#include "CUIBase.h"
#include "CPlayer_StateInfo.h"
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
	_bool Get_AniFinish() { return m_bAniFinish; }
	void Set_AniFinish(_bool _bAniFinish) { m_bAniFinish = _bAniFinish; }

protected:
	_matrix					m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_bool m_bAniFinish; // animation 끝났는지 다른 클래스에게 전해주기 위해

protected:
	virtual HRESULT			Set_Component();
	virtual			HRESULT Set_Texture() { return S_OK; }; // texture 변경 로직 담는 함수
	
public:
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

