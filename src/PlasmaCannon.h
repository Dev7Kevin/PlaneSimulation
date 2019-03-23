///////////////////////////////////////////////////////////////////////////////
//
// PlasmaCannon.h : 플라즈마 케논 헤더 파일.
//
//
#pragma once
 
 
#include "Bullet.h"
///////////////////////////////////////////////////////////////////////////////
//
// class CPlasmaCannon 
//
///////////////////////////////////////////////////////////////////////////////

class CPlasmaCannon : public CBullet
{
	//플라즈마용 정점 구조체.
	struct VTX_PLASMACANNON
	{
		VECTOR3 vPos;
 		float	U,V;
	};
	
	enum {
		FVF_PLASMACANNON_ = D3DFVF_XYZ | D3DFVF_TEX1,

		PC_SHOW_ = 0,			//plasma cannon 출력모드.
		PC_DISAPPEAR_,			// 사라지기. (fade-out)
 		PC_DELETE_,				// 객체 삭제하기.

	};


protected:
	
	float	m_fRadius;				// 빔의 반지름 - '굵기'의 기준이 된다.
	float	m_fLength;				// 빔의 길이. 
 	//DWORD	m_Color;				// 빔의 색상. -> CBullet 의 것을 사용.

	UINT	m_State;				// 빔의 '렌더링' 상태 : show -> disappear -> delete.

	DWORD	m_ShowTime;				// '보여주기' 시간: 이시간이 다하면, '사라진다.'
	DWORD	m_OldShowTime;
	float	m_fAlpha;				// fade-out 用 알파값.

	// CBullet 의것을 사용.
	//SPRDATA		m_sprPC;		//스프라이트  
 	//CCamera**	m_ppCam;			//카메라 이중포인터 (빌보딩)

	// 빔의 '몸통' 축 벡터. : 빌보딩用
	VECTOR3 m_vNView,
			m_vUp,
			m_vHorz;

	// 빔의 '끝부분' 축 벡터. : 빌보딩用
	VECTOR3 m_vEndNView,
			 //m_vEndUp, -> m_vUp 과 공유.
			 m_vEndHorz;
			
	 

	VTX_PLASMACANNON* m_pVB;			//정점 버퍼 : '월드 변환' 된 정점들.(출력용)
 	static VTX_PLASMACANNON m_OrgVB[6];	//원본 정점버퍼 : '원본 모델'.

	//static list<CPlasmaCannon*> m_PlasmaCannonList;
	//typedef list<CPlasmaCannon*>::iterator PC_Itor;

	static int m_PCCount;		//PC 생성 개수.

protected:

	//빔 상태에 따른 렌더링 모드...
	int Show(float dTime);		
	int Disappear(float dTime);


public:
	CPlasmaCannon(
 		LPDEVICE pDev,
		//CB3Mesh* pMesh,		//총알용 메쉬 포인터. -> 사용하지 않음
		SPRDATA* pSpr,			//총알용 스트라이트 포인터.. 
								//pMesh, pSpr 둘중에 하나는 있어야 한다.
		CCamera** ppCam,		//카메라 이중 포인터. 2004.06.19. 수정.

		float	fRadius,		// 빔의 반지름 - '굵기'의 기준이 된다.
		float	fLength,		// 빔의 길이. 
		UINT	Dmg,			// 데미지.
		COLOR	Color,			// 컬러.

		DWORD	ShowTime,		// '보여주기' 시간: 이시간이 다하면, '사라진다.'
 		float	fAlpha,			// fade-out 用 알파값.

		float	fFireSpeed,		//발사 속도.
		VECTOR3 vFireDir,		//발사 방향.
		VECTOR3 vUp,			//발사시, '기체의 'up' 벡터.
		VECTOR3 vHorz,			//발사시  '기체의 수평벡터'
		float	fMaxRange,		//최대 사거리...

		VECTOR3 vPos,			//총알 발사 위치.
		VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		VECTOR3 vScale = VECTOR3(1.0f, 1.0f, 1.0f),
		VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f) 
		);

	virtual ~CPlasmaCannon(void);


	//오버라이딩 메소드.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();


	//정적 메소드.
	//BOOL AddPlasmaCannon(PCINFO* pInfo);
	//BOOL RemovePlasmaCannon()

	static UINT GetPCCount(){ return m_PCCount;  }

//#define PLASMACANNON_TEST_
#ifdef PLASMACANNON_TEST_
	UINT m_PlasmaCannonSN;
	static UINT m_PlasmaCannonTot;
#endif


};



//////////////////////////////////////////////////////////////////////////
//
// namespace GP 
//
namespace GP
{
	BOOL PlasmaCannonInit();
	void PlasmaCannonCreate(CHero* pHero, CCamera** ppCamera);
	void PlasmaCannonMove(float dTime);
	void PlasmaCannonDraw(float dTime);		
	void PlasmaCannonRelease();
};



/*************** end of file "PlasmaCannon.h " ***************************/




































