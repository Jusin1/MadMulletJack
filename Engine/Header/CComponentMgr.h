#pragma once
#include "CBase.h"
#include "CComponent.h"

#include "CTriCol.h"
#include "CRcCol.h"
#include "CVIBuffer_Rect.h"
#include "CVIBuffer_Terrian.h"
#include "VIBuffer_Cube.h"
#include "CColider_Rect.h"

#include "CTransform.h"
#include "CTexture.h"
#include "CCalculator.h"
#include "CRenderer.h"

BEGIN(Engine)

// 컴포넌트의 원본을 생성하여 보관
class ENGINE_DLL CComponentMgr :   public CBase
{
	DECLARE_SINGLETON(CComponentMgr)

private:
	explicit CComponentMgr();
	virtual ~CComponentMgr();

public:
	HRESULT			Ready_Prototype(_uint iSceneNum);
	HRESULT			Add_Prototype(_uint iSceneIdx, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iSceneIdx, const _tchar* pPrototypeTag, void* pArg = nullptr);
	void Clear(_uint iSceneIdx);

private:
	_uint								m_iSceneNum = 0;
	map<const _tchar*, CComponent*>* m_mapPrototype = nullptr;	// 원본 컴포넌트
	typedef map<const _tchar*, class CComponent*>		PROTOTYPES;

	
public:
	class CComponent* Find_Component(_uint iSceneIdx, const _tchar* pPrototypeTag);

public:
	virtual void		Free();
};

END

