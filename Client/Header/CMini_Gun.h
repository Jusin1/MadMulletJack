#pragma once
#include "CGun.h"
class CMini_Gun :
	public CGun
{
private:
	explicit CMini_Gun(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMini_Gun(const CMini_Gun& rhs);
	virtual ~CMini_Gun();

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

	void SpawnEff(_vec4 _vSizeOffset);
	void DeleteEff();

private:
	map<const _tchar*, CTexture*> m_mapTextures;    // 애니메이션 텍스쳐
	wstring m_CurrentAnimTag;                       // 현재 애니메이션 태그

	_float m_fScale;
	_float m_fEffCoolTime;
	_float m_fEffRenderTime;

public:
	static CMini_Gun* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

