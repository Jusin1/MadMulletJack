#pragma once
#include "CGameObject.h"

BEGIN(Engine)

class CTexture;
class CVIBuffer_Rect;
typedef struct tagParticleInfo ParticleInfo;

struct SpriteParticleOptions
{
	EffectOptions tEffectOption;
	SpriteParticleType eType;
	_float m_fGroundY{ 0.f };
};

class ENGINE_DLL CEffect_Pixel_Sprite : public CGameObject
{
private:
	explicit CEffect_Pixel_Sprite(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_Pixel_Sprite(const CEffect_Pixel_Sprite &rhs);
	virtual ~CEffect_Pixel_Sprite();

	virtual void Free() override;
public:
	static			CEffect_Pixel_Sprite *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual			CGameObject *Clone(void *pArg) override;

	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void *pArg);
	virtual			_int		Update_GameObject(const _float &fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float &fTimeDelta) {};
	virtual			void		Render_GameObject();

	virtual HRESULT Spawn_Pooling(void *pArg = nullptr) override;
	virtual HRESULT Despawn_Pooling() override;
private:
	HRESULT Texture_Clone();
	HRESULT Set_TextureInit(SpriteParticleType _e);
	HRESULT Change_Texture(const _tchar *LayerTag);
	void SetUp_BillBoard();
	void SetOptions(const EffectOptions &tOption, _bool bRemakeVB = true);
	void Trigger();
	void Do_Once();
	void Particle_Update(_float fDeltaTime);
	HRESULT Ready_VB();
	_float  rand01() const;
	_float  randRange(_float a, _float b) const;
	_vec3 randomDir_HalfSphere() const;
	void Effect_SetRenderState();
	DWORD FtoDW(_float f)
	{
		DWORD dwReturn;
		::memcpy(&dwReturn, &f, sizeof(_float));
		return dwReturn;
	}
private:
	_int m_iTextureCount{ 0 };
	_int m_iAliveCount{ 0 };
	_int m_iVBCapacity{ 0 };
	_float m_fY;
	LPDIRECT3DVERTEXBUFFER9		m_pVB{ nullptr };
	LPDIRECT3DINDEXBUFFER9		m_pIB{ nullptr };
	CTexture *m_pTextureCom{ nullptr };
	vector<ParticleInfo> m_vecParticles;
	EffectOptions m_tOption;
};

END