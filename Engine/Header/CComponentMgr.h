#pragma once
#include "CBase.h"
#include "CComponent.h"

#include "CTriCol.h"
#include "CRcCol.h"
#include "CVIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "CColider_Rect.h"
#include "CColider_Cube.h"
#include "CColider_Sphere.h"
#include "VIBuffer_Color.h"

#include "CTransform.h"
#include "CTexture.h"
#include "CCalculator.h"
#include "CRenderer.h"

BEGIN(Engine)

// 컴포넌트의 원본을 생성하여 보관하고 복제
class ENGINE_DLL CComponentMgr :   public CBase
{
	DECLARE_SINGLETON(CComponentMgr)

private:
	explicit CComponentMgr();
	virtual ~CComponentMgr();

public:
	HRESULT			Ready_Prototype(_uint iSceneNum); // 씬 갯수만큼 컨테이너 배열 생성
	HRESULT			Add_Prototype(_uint iSceneIdx, const _tchar* pPrototypeTag, CComponent* pPrototype); // 원형 등록
	CComponent* Clone_Component(_uint iSceneIdx, const _tchar* pPrototypeTag, void* pArg = nullptr); // 복제 생성
	void Clear(_uint iSceneIdx);// 컴포넌트 원본 제거

private:
	_uint								m_iSceneNum;
	map<const _tchar*, CComponent*>* m_mapPrototype;
	typedef map<const _tchar*, class CComponent*>		PROTOTYPES; 

	
public:
	class CComponent* Find_Component(_uint iSceneIdx, const _tchar* pPrototypeTag); // 특정씬에서 해당 태그의 컴포넌트 검색

public:
	virtual void		Free(); 
};

END

