// Tornado.cpp: implementation of the CTornado class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#include "Tornado.h"

 

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//

CTornado::CTornado(
		LPDEVICE pDev,
		CB3Mesh* pMesh,			//총알용 메쉬 포인터.
		B3SPRITE* pSpr,			//총알용 스트라이트 포인터.. 둘중에 하나는 있어야 한다.
		CCamera** ppCam,		//카메라 이중 포인터. 2004.06.19. 수정.

		UINT	 Dmg,			//총알 데미지.
		COLOR	 Color,			//총알 컬러.
		float	fFireVelo,		//총구 발사 가속도
		VECTOR3 vFireDir,		//발사 방향.  
		MATRIX	mFireRot,		//발사시, 유닛의 회전행렬... 
		float	fMaxRange,		//최대 사거리...

		float	 fRadius,		//충돌구 반경.

		VECTOR3 vPos,			//총알 발사 위치.		
		VECTOR3 vScale,			//총알 '크기'
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
:CRailGun(pDev, pMesh, pSpr, ppCam, 
		  Dmg, Color, fFireVelo, vFireDir, fMaxRange, 
		  fRadius,		
		 vPos, vScale, vRot, vMoveVelo, vRotVelo ) 
{ 

	//나선형 코드. 초기화.
	m_vSpiralPos = VECTOR3(0, 0, 0); 
	m_fSpiralVal = 0.0f;
	//m_vMoveVelo = 0.8f;  

	m_mFireRot = mFireRot;		
	 

}//end of CTornado::CTornado()





//////////////////////////////////////////////////////////////////////////////
//
CTornado::~CTornado()
{ 
	Release();	
}




//////////////////////////////////////////////////////////////////////////////
//
// 토네이도 이동처리 : 로컬에서 계산후, 월드로 전환한다.
//
int CTornado::Move(float dTime/* =0 */)
{
	// [로컬 이동] 
 	// 나선형으로 궤적 운동을 한다. (x, y 축 이동만 생각하자!) 
	//
	m_fSpiralVal += D3DX_PI * 2 * dTime;  

	POS3 vPos(0, 0, 0);
	vPos.x = (float)sin(m_fSpiralVal) * 3.f;
	vPos.y = (float)cos(m_fSpiralVal) * 3.f;
	vPos.z =  0;  
 
 
	//[월드이동]
	// 발사시 '비행기'의  회전값으로 회전후,
	// 발사방향으로 이동. 
	D3DXVec3TransformCoord(&vPos, &vPos, &m_mFireRot);
	m_vPos += vPos +  m_vFireDir * m_fFireVelo * dTime;

	//------------------------------------------------------------------
	// 사거리 계산.
	// 부모 메소드를 사용하지 않고 직접 이동처리한다. 
	//------------------------------------------------------------------	
	m_vMoveAccu.z += m_fFireVelo * dTime;			//이동거리 누계. 
	
	// 사거리 계산의 속도향상을 위해 스칼라로 계산한다. 
	// CBullet::Move 와 비교해 볼것.
	if( m_vMoveAccu.z > m_fMaxRange)  
 	{
		// 최대 사거리를 벗어난후, '제거'의 의미로 false 를 리턴한다. 
		return FALSE;
	} 	


	// 
	// TM 만들기.														
	// 
	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);

	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);

	return TRUE;	  
	//return CGun::Move(dTime); -> 사용않합니다.
}




//////////////////////////////////////////////////////////////////////////////
//
int CTornado::Draw(float dTime/*=0*/)   
{
	// 토네이도 코드 추가 
	// ...
 
	
	// 나머지는 부모에서 처리.	 
	return CRailGun::Draw(dTime);     
	 
}




//////////////////////////////////////////////////////////////////////////////
//
void CTornado::Release()
{	 
	// 토네이도 코드 추가 
	// ...	 
	 
}

 

/********************  end of file "Tornado.cpp" ****************************/
