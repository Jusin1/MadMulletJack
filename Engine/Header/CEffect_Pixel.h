#pragma once
#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CEffect_Pixel : public CGameObject
{
private:
	typedef struct tagParticleInfo
	{
		_vec3		vPosition;
		_vec3		vVelocity;
		// 경과 시간
		_float       fDurationTime;
		// 수명
		_float       fLifeTime;
		D3DCOLOR    colorStart;
		D3DCOLOR    colorEnd;
		_float       fSize;
		_bool        bAlive;
	} ParticleInfo;
private:
	explicit CEffect_Pixel(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_Pixel(const CEffect_Pixel &rhs);
	virtual ~CEffect_Pixel();

	virtual void Free() override;
public:
	static			CEffect_Pixel *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual			CGameObject *Clone(void *pArg) override;

	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void *pArg);
	virtual			_int		Update_GameObject(const _float &fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float &fTimeDelta) {};
	virtual			void		Render_GameObject();

	void SetOptions(const EffectOptions &tOption, _bool bRemakeVB = true);
	void Trigger();

	// 프리셋
	
private:
	void Do_Once();
	void Particle_Update(_float fDeltaTime);
	HRESULT Ready_VB();
	D3DXCOLOR LerpColor(const D3DXCOLOR &a, const D3DXCOLOR &b, _float _f);
	_float  rand01() const;
	_float  randRange(_float a, _float b) const;
	D3DCOLOR ToColor(const D3DXCOLOR &c) { return D3DCOLOR_COLORVALUE(c.r, c.g, c.b, c.a); }
	_vec3 randomDir_HalfSphere() const;
	void Effect_SetRenderState();
	DWORD FtoDW(_float f)
	{
		DWORD dwReturn;
		::memcpy(&dwReturn, &f, sizeof(_float));
		return dwReturn;
	}
private:
	_bool m_bAlive{ FALSE };
	_int m_iAliveCount{ 0 };
	_int m_iRepeatRemain{ 0 };
	_int m_iVBCapacity{ 0 };
	_float m_fRepeatTimer{ 0.f };
	LPDIRECT3DVERTEXBUFFER9		m_pVB{ nullptr };
	EffectOptions m_tOption;
	vector<ParticleInfo> m_vecParticles;
};

END