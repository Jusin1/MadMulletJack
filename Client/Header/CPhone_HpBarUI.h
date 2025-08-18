#pragma once
#include "CUI.h"
class CPhone_HpBarUI : public CUI
{
private:
	explicit CPhone_HpBarUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPhone_HpBarUI(const CPhone_HpBarUI& rhs);
	virtual ~CPhone_HpBarUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta) override;
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual			void		Render_GameObject() override;

public:
	virtual HRESULT			Set_Component();
	 HRESULT			Set_Texture(SCENE _eSCENE) ;
	 HRESULT			Change_Texture(const _tchar* pTextureTag);
	HRESULT					Texture_Clone();

	map<const _tchar*, CTexture*> m_mapTextures;    // 애니메이션 텍스쳐
	wstring m_CurrentAnimTag;

public:
	static  CPhone_HpBarUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

