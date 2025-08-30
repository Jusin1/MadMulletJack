#pragma once
#include "CGameObject.h"
#include "CTexture.h"

BEGIN(Engine)

class CVIBuffer_Rect;

typedef struct tagEffectInfo
{
	_float fAngle{ 0.f };
	WorldEffectType eType{ WorldEffectType::NONE };
}EFFECTINFO;

class ENGINE_DLL CEffect_World : public CGameObject
{
private:
	explicit CEffect_World(LPDIRECT3DDEVICE9 pGraphivDevice);
	explicit CEffect_World(const CEffect_World &rhs);
	virtual ~CEffect_World();

	virtual void Free() override;
public:
	static			CEffect_World *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual			CGameObject *Clone(void *pArg) override;

	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void *pArg);
	virtual			_int		Update_GameObject(const _float &fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float &fTimeDelta);
	virtual			void		Render_GameObject();

	virtual HRESULT Spawn_Pooling(void *pArg = nullptr);
	virtual HRESULT Despawn_Pooling();

	void Set_TintColor(D3DXCOLOR c) { m_TintColor = D3DCOLOR_COLORVALUE(c.r, c.g, c.b, c.a); m_bTintFlag = true; }
private:
	HRESULT Texture_Clone();
	HRESULT Set_Component();
	HRESULT Set_TextureInit(WorldEffectType _e);
	HRESULT Change_Texture(const _tchar *LayerTag);
	void Update_Animation();
	void SetUp_BillBoard();
private:
	_bool m_bTintFlag{false};
	D3DCOLOR m_TintColor{ D3DCOLOR_COLORVALUE(255.f, 255.f, 255.f, 255.f) };
	CTexture *m_pTextureCom{ nullptr };
	map<wstring, CTexture *> m_mapTexture;
	CVIBuffer_Rect *m_pBufferCom{ nullptr };
};

END