#pragma once
#include "CMainWeapon.h"
class CImageUI;
class CKatana :
    public CMainWeapon
{
private:
	explicit CKatana(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CKatana(const CKatana& rhs);
	virtual ~CKatana();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg)override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	virtual HRESULT Set_Texture() override; // palyerInfo에 따라 texture 셋팅
	virtual HRESULT Texture_Clone()	override;
	virtual HRESULT Change_Texture(const _tchar* pTextureTag) override;

private:
	map<const _tchar*, CTexture*> m_mapTextures;    // 애니메이션 텍스쳐
	wstring m_CurrentAnimTag;                       // 현재 애니메이션 태그
	CUIBase* m_pOpenUI;
	CImageUI* m_pSheathUI;

public:
	static CKatana* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

