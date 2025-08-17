#pragma once
#include "CHpBarUI.h"
class CMan_HpBarUI : public CHpBarUI
{
private:
	explicit CMan_HpBarUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMan_HpBarUI(const CMan_HpBarUI& rhs);
	virtual ~CMan_HpBarUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta) override;
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual			void		Render_GameObject() override;

private:
	virtual HRESULT			Set_Component();
	virtual HRESULT Set_Texture() override; // palyerInfo에 따라 texture 셋팅
	HRESULT Change_Texture(const _tchar* pTextureTag);
	HRESULT Texture_Clone();

private:
	map<const _tchar*, CTexture*> m_mapTextures;    // 애니메이션 텍스쳐
	wstring m_CurrentAnimTag;                       // 현재 애니메이션 태그

public:
	static  CMan_HpBarUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

