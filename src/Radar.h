//////////////////////////////////////////////////////////////////////////
//
// Radar.h
//
// 3D 레이더 구현 
//
//
//////////////////////////////////////////////////////////////////////////

#pragma once
 
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL 버전 엔진 필수헤더. 

//#include "Camera.h"
class CCamera;
class CPlane;
class CEnemy;
class CUnit;
class CFrustum;	


#include "list"
using namespace std;


///////////////////////////////////////////////////////////////////////////////
//  
// class CRadar  
// 
class CRadar		
{
	typedef struct {				// 레이다용 삼각형
		VECTOR3 vPos;
		//DWORD	dwColor;
		FLOAT	tu, tv;
	}VERTEX_RADAR;
	#define D3DFVF_RADAR_ (D3DFVF_XYZ | D3DFVF_TEX1)
	//#define D3DFVF_SHOCKWAVE_ (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

 	LPVERTEXBUFFER	m_pVB;		 
	int				m_VertexCnt;	 
	int				m_PolyCnt;	 

protected:

	LPDEVICE		m_pDev;
	D3DVIEWPORT9	m_Viewport;
	list<CEnemy*>	&m_pEnemyList; 
	typedef list<CEnemy*>::iterator itor;

	CCamera**	m_ppCamera;

	CPlane*		m_pHero;
	SPRDATA*	m_pSprRadarv;
	SPRDATA*	m_pParticle;

	MATRIX	m_mView;
	MATRIX	m_mRotate;;
	MATRIX	m_mScale;
	MATRIX	m_mTM;
	MATRIX	m_mInverseView;


	CEnemy*	 m_pTarget;			//Lock-On 된 적 비행기 포인터. (유동적임)

	//
	// 레이더 '스캐닝 영역' + 수동 선택 관련 데이터들.
	//
	list<CEnemy*>	m_ScanList;				//'Lock-on' 가능한 리스트. : 시야에 들어온 적기 리스트.
	CFrustum*		m_pScanArea;			//레이더의 스캐닝 영역 
	MATRIX			m_mScanView;			//레이더 스캐닝 영역 변환 행렬. : 적기 위치를 스캐닝 영역으로 변환 시키기 위한 행렬.
											//카메라 공간 변환 행렬의 목적과 유사.
	int				m_TargetNum;			//현재 Lock-On 된 목표물의 번호. : ScanList 에서의 순서..
											//'다음 타겟'의 선정시 사용하며 1부터 시작함. (전체 개수에 따른 것.)


protected:

	BOOL	Init();
	void	Release();
 	void	DrawCircle();

	VECTOR3	CheckCollision(VECTOR3 vTargetPos,VECTOR3 vPos, float fTargetRadius);

	//주인공 비행기의 위치/방향을 기준으로 스캐닝 영역 변환 행렬을 만든다. 
	//CCamera::SetView 와 유사.
	void	MakeScanViewMatrix();	

public:

	CRadar(LPDEVICE pDevice,CCamera** pCamera, D3DVIEWPORT9 viewPort,
		   list<CEnemy*> &EnemyList, CPlane* pHero, SPRDATA* pSprRadarV, 
		SPRDATA* pParticle);
	~CRadar();

	int				Update();
	int				Render(BOOL bShowAzimuth);
 
	
public:
	//----------------------------------------------//
	// 미사일의 액티브 호밍 메소드. 2004.11.15.	    //	
	// ->FCS 에서 처리하는 것을 시물레이션 해 보자! //
	//----------------------------------------------//
	int		SelectTarget(BOOL bNext = FALSE);			//★									
	const CEnemy* GetTarget() const { return m_pTarget; }	
	void		  ClearTarget() { m_pTarget = NULL; }		//목표물 클리어.


	//레이더 리셋. 2004.11.16.
	void ResetTargets(list<CEnemy*> &EnemyList);	//적기 리스트 재설정.

	//목표물 탐색. 2005.02.25.
	void SearchTargets();				//적기 스캐닝 
	int	 GetScanCnt();				//스캐닝된 적 유닛의 개수.	
	
	//적기 리스트 얻기.★
	//list<CEnemy*>& GetScanList() { return m_ScanList; }
	list<CEnemy*>* GetScanList() { return &m_ScanList; }


	enum {
		SELECT_NEXT_ = TRUE,
		LOCK_ON_ = TRUE,
		LOCK_OFF_ = FALSE,
	};

};


/******************** end of file "Radar.h" ******************************/ 
