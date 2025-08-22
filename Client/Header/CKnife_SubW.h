#pragma once
#include "CSubWeapon.h"
class CKnife_SubW : public CSubWeapon
{
private:
	explicit CKnife_SubW(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CKnife_SubW(const CKnife_SubW& rhs);
	virtual ~CKnife_SubW();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg)override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	virtual HRESULT Set_Texture() override; // subW는 필요 없을지도

private:
	virtual HRESULT Texture_Clone()	override;
	virtual HRESULT Change_Texture(const _tchar* pTextureTag) override;

private:
	map<const _tchar*, CTexture*> m_mapTextures;    // 애니메이션 텍스쳐
	wstring m_CurrentAnimTag;                       // 현재 애니메이션 태그

	PLAYERSTATE m_ePlayerState;

public:
	static CKnife_SubW* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

