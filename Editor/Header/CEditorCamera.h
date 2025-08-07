#pragma once
#include "CCamera.h"

class CEditorCamera : public CCamera
{
public:
	enum CAMMODE { CAM_DEFAULT, CAM_END };
private:
	explicit CEditorCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEditorCamera(const CEditorCamera &rhs);
	virtual ~CEditorCamera();

	virtual void Free() override;
public:
	static CEditorCamera *Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual CCamera *Clone(void *pArg);

	virtual			HRESULT Ready_GameObject();
	virtual			HRESULT Initialize(void *pArg)override;
	virtual			_int		Update_GameObject(const _float &fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float &fTimeDelta);
	virtual			void		Render_GameObject() {}
private:
	void DefaultCamera(const _float &fTimeDelta);
private:
	_float		m_fSpeed;
	_bool		m_bFix;
	_bool		m_bCheck;
	_long		m_lMouseWheel = 0;
	_vec3		m_vDistance = _vec3(0, 3, -3);
	CAMMODE		m_eCamMode = CAM_DEFAULT;
};

