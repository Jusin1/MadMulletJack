#pragma once
#include "CGameObject.h"
#include "CTransform.h"
BEGIN(Engine)

class ENGINE_DLL CCamera :   public CGameObject
{
public:
	typedef struct tagCameraInfo
	{
		_vec3 vEye;
		_vec3 vAt;

		_float fAspect;
		_float fFov;
		_float fNear;
		_float fFar;

		CTransform::TRANSFORMINFO		TransformInfo;
	}CAMINFO;
	
protected:
	explicit	CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg);
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			HRESULT     Render();

public:
	HRESULT Apply_ViewPorjection();

protected:
	static const _tchar* m_pTransformTag;
	CTransform* m_pTransform = nullptr;
	CAMINFO			  m_camInfo;

public:
	virtual		CCamera* Clone(void* pArg) = 0;
	virtual		void		Free();

};
END
