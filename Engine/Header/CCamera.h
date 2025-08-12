#pragma once
#include "CGameObject.h"
#include "CTransform.h"
BEGIN(Engine)

// 카메라 오브젝트
class ENGINE_DLL CCamera :   public CGameObject
{
public:
	// 카메라 정보
	typedef struct tagCameraInfo
	{
		_vec3 vEye; // 위치
		_vec3 vAt; // 방향

		_float fAspect; // 종횡비
		_float fFov; // 시야각
		_float fNear; // 근평면
		_float fFar; // 먼 평면

		CTransform::TRANSFORMINFO		TransformInfo; // 초기 위치
	}CAMINFO;
	
protected:
	explicit	CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera();



public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg); // 카메라 정보 세팅
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			HRESULT     Render();

public:
	static const _matrix& GetView() { return m_matView; }
	static const _matrix& GetProj() { return m_matProj; }

public:
	HRESULT Apply_ViewPorjection(); // View/Projection 행렬 반영

protected:
	static const _tchar* m_pTransformTag; // Transform 키 태그
	CAMINFO			  m_camInfo; // 카메라 설정 값 저장

	// 전역 참조용 정적 행렬
public:
	static _matrix m_matView;
	static _matrix m_matProj;


public:
	virtual		CCamera* Clone(void* pArg) = 0;
	virtual		void		Free();

};
END
