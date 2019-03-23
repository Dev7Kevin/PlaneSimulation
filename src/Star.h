// Star.h: interface for the CStar class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#pragma once
 
#include "../../Engine/include/B3X.h"
#include "Unit.h"
#include "list"
using namespace std;
 
class CPlane;


//////////////////////////////////////////////////////////////////////////////
//
// class CStar : '별' 클래스. : Point sprite 를 적용한다.
//
class CStar : public CUnit  
{
protected:

 	int			m_Cnt;			//'별'의 개수.
	float		m_fSize;		//'별 가루' 사이즈 : 해상도 마다 다르다.
 	CPlane*		m_pHero;		//'주인공' 정보...
	

	//정점 포멧 관련.
	typedef struct {
		VECTOR3 vPos;			//위치.
		//float	fSize;			//크기.
		DWORD	dwColor;		//색상.
	}VERTEX_STAR;

	enum {
		FVF_STARS_ = D3DFVF_XYZ | D3DFVF_DIFFUSE,
		//DFVF_STARS_ = D3DFVF_XYZ|D3DFVF_PSIZE|D3DFVF_DIFFUSE, 

		//DEBUG_STARDUST_ = 1000,	//별가루 디버깅용.
	};
	 	 
	LPVERTEXBUFFER	m_pVBuffer;		//정점 버퍼 포인터.

 	SPRDATA		m_Spr;				//'별' 출력 사용될 텍스쳐.
 	CCamera**	m_ppCamera;			//'현재' 활성화된 카메라의 2중 포인터. 
									//(테스트용)	

	//
	// '별가루' 관리 리스트.. 관련.
	//
 	list<VERTEX_STAR*> m_StarDustList;					//'우주 먼지'...
	typedef list<VERTEX_STAR*>::iterator itor;

	
 
public:

	CStar(
	 LPDEVICE	pDev,			//디바이스 포인터.
	 int		Cnt,			//별 개수.
 	 SPRDATA*	pSpr,			//'별' 스프라이트 포인터.
 	 CPlane*	pHero,			//주인공 포인터.
	 CCamera**	ppCamera,		//'현재' 활성화된 카메라의 2중 포인터.
	 int		Width			//'디스플레이' 모드 : '별가루'의 크기에 관여한다.	 
  	 );


 	virtual ~CStar();

	// 기본 메소드.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);	 
	virtual void Release();

	// 추가 메소드.
	BOOL Init();
	BOOL ResetVBuffer();					//정점 버퍼 리셋.
	BOOL StarDustUpdate(VECTOR3 vPos);		//별가루 업데이트..


};


/****************** end of file "Star.h" **********************************/ 
