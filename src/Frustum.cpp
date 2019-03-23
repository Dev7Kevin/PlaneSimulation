// Frustum.cpp: implementation of the CFrustum class.
// 
//
//
////////////////////////////////////////////////////////////////////////////// 


#include "Frustum.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// 주어진 3개의 점(vP0, vP1, vP2) 를 통해 '평면'을 구성. 
//
void PLANE::PlaneFromPoint(VECTOR3& vP0, VECTOR3& vP1, VECTOR3& vP2)
{
	VECTOR3 V0, V1, N;
	V0 = vP1 - vP0;
	V1 = vP2 - vP0;
	D3DXVec3Cross(&N, &V0, &V1);
	D3DXVec3Normalize(&N, &N);

	_A = N.x;
	_B = N.y;
	_C = N.z;
	_D = 0;  
}




//////////////////////////////////////////////////////////////////////////////
//
// class CFrustum  Interfaces. 
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
CFrustum::CFrustum()
{
	m_fDist = 0.0f;
	//N/A
}
 
//////////////////////////////////////////////////////////////////////////////
//
CFrustum::~CFrustum()
{ 
	//...
}

//////////////////////////////////////////////////////////////////////////////
//
CFrustum::CFrustum(float fovY, float ratio, float n, float f)
{
	m_fDist = 0.0f;

	Init( fovY, ratio, n, f);
}




//////////////////////////////////////////////////////////////////////////////
//
// 시야 절두체 컬링을 위한 6면체를 구성합니다.
// 단, 현재 셈플에서는 원평면, 근평면은 무시합니다.
//
void CFrustum::Init(float fovY, float ratio, float n, float f)
{	
	
	// 수평 시야각 계산.
	float fovX = fovY * ratio;


	// 프러스텀 구성 정점 5개.
	// 프러스텀의 'p0' 에서 바라볼경우의 정점구성도
	/*
    p1┌─────┐p2 
	  │          │  
	  │    p0    │ 
	  │          │ 
	p3└─────┘p4
	*/	
	VECTOR3 P[5];
	P[0] = VECTOR3(0, 0, 0);		//프러스텀의 원점.
   

	
 

	//--------------------------//
	// 근평면의 컬링값 설정.	//
	//--------------------------//
	m_fNear = n;		

	//--------------------------//
	// 원평면의 컬링값 설정.	//
	//--------------------------//
	m_fFar = f;
	

	//--------------------------//
	// 왼쪽 평면 생성.			//
	//--------------------------//
 	P[1] = VECTOR3(-f * ((float)tan(fovX * 0.5f)), 
		  		    f * (float)tan(fovY * 0.5f), 
				    f );
	P[3] = VECTOR3(P[1].x, -P[1].y, P[1].z);
	m_Left.PlaneFromPoint(P[0], P[3], P[1]);	//방향주의

	//--------------------------//
	// 오른족 평면 생성.		//	
	//--------------------------// 
	P[2] = VECTOR3(f * ((float)tan(fovX * 0.5f)), 
		  		   f * (float)tan(fovY * 0.5f), 
				   f);
	P[4] = VECTOR3(P[2].x, -P[2].y, P[2].z);
	m_Right.PlaneFromPoint(P[0], P[2], P[4]);	//방향주의


	//--------------------------//
	// 상부 평면 생성.			//
	//--------------------------// 
	m_Top.PlaneFromPoint(P[0], P[1], P[2]);		//방향주의

	//--------------------------//
	// 하부 평면 생성.			//
	//--------------------------// 
	m_Bottom.PlaneFromPoint(P[0], P[4], P[3]);	//방향주의



	//프러스텀 가시화 준비.
	_InitMesh( P );

}








//////////////////////////////////////////////////////////////////////////////
//
// int CFrustum::IsVisible(...) : 뷰 공간 기준 가시성판정
//
// 물체 ( 점 p2 와 반경 radius 를 가지는..) 가 View 변환 행렬로 구성된
// Frustum 안에 있습니다.면 1 을 리턴합니다.
//
int CFrustum::IsVisible(MATRIX* pViewTM, VECTOR3* pTargetPos, float radius) 
{
	//
	// 목표의 위치를 뷰공간으로 투영.
	//
	// * (카메라 변환 행렬) x (월드 정점) = (카메라 공간의 정점)
	// * 즉, Frustum Culling 의 주요 기술은 카메라 공간의 정점이 
	// * Frumtum 안에 있냐? 없냐? 를 검증하는 데 있습니다..
	//
	VECTOR3 vPos;
	D3DXVec3TransformCoord(&vPos, pTargetPos, pViewTM);

	m_fDist = 0.0;

	//--------------------------//
	// '원/근 평면'의 컬링...	//	
	//--------------------------//
  	if( vPos.z > (m_fFar + radius)) return FALSE;			//'뷰' 공간이므로, '절대거리'만 비교합니다.
	if( vPos.z < (m_fNear - radius)) return FALSE;
	
  
	//--------------------------//
	//'좌/ 우측 평면'의 컬링... // 
	//--------------------------// 
	//VECTOR4 pos = VECTOR4(vPos, 1);						//4성분 내적도 동일결과를 가져오기에, 3성분내적을 사용하겠습니다.

	m_fDist = D3DXVec3Dot(&vPos, (VECTOR3*)&m_Left);		//평면과의 거리계산은 '내적'과 동일합니다.
  	//m_fDist = vPos.x * m_Left._A + vPos.z * m_Left._C;	//직접계산할 경우 : Y 축 성분의 비교는 의미가 없으므로 생략할 수 있습니다.
	if(m_fDist > radius) return FALSE;						//평면과의 거리가 '반경' 보다 크다면 밖에 있는 것입니다.

	m_fDist = D3DXVec3Dot(&vPos, (VECTOR3*)&m_Right);
 	//m_fDist = vPos.x * m_Right._A + vPos.z * m_Right._C;	
 	if(m_fDist > radius) return FALSE;


	//--------------------------//
	//'상/하 평면'의 컬링...    //
	//--------------------------//
	m_fDist = D3DXVec3Dot(&vPos, (VECTOR3*)&m_Top);
 	//m_fDist = vPos.y * m_Top._B + vPos.z * m_Top._C;		//직접계산할 경우 : Y 축 성분의 비교는 의미가 없습니다.
 	if( m_fDist > radius) return FALSE;

	m_fDist = D3DXVec3Dot(&vPos, (VECTOR3*)&m_Bottom);
	//m_fDist = vPos.y * m_Bottom._B + vPos.z * m_Bottom._C;	
 	if( m_fDist > radius) return FALSE;

 
	return TRUE;

}










///////////////////////////////////////////////////////////////////////////////
//
//  프러스터 가시화 버퍼구성.
//
//
void CFrustum::_InitMesh(VECTOR3 p[5])
{
	
	memcpy(m_vFPos, p, sizeof(VECTOR3) * 5);
	 

	//인덱스 구성.	 
	USHORT index [] = {
		 0, 3, 1 ,
		 0, 1, 2 ,
		 0, 2, 4 ,
		 0, 3, 4 ,
		 1, 2, 3 ,
		 3, 2, 4 ,
	};

	memcpy(m_pFIndex, index, sizeof(index)); 
	 
}







///////////////////////////////////////////////////////////////////////////////
//
//  프러스터 가시화 : 
//
//
int CFrustum::Draw(LPDEVICE pDev, COLOR col, MATRIX* pMtx/*= NULL*/, BOOL bWire/*=TRUE*/)
{


	DWORD oldmode; 
	pDev->GetRenderState(D3DRS_FILLMODE, &oldmode);

	MATERIAL m;
	ZeroMemory(&m, sizeof(m));
	m.Diffuse = m.Ambient = col;
	pDev->SetMaterial(&m);
	pDev->SetTexture(0, NULL);
	DWORD oldlight;
	pDev->GetRenderState(D3DRS_LIGHTING, &oldlight); 
	pDev->SetRenderState(D3DRS_LIGHTING, TRUE); 
	pDev->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);	 
	if(pMtx) pDev->SetTransform(D3DTS_WORLD, pMtx);
	pDev->SetFVF( D3DFVF_XYZ );

	if(bWire == FALSE)
	{		
		pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID); 		
		pDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 6, m_pFIndex, 
									  D3DFMT_INDEX16, m_vFPos, sizeof(VECTOR3));
	}
	 	
	pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 6, m_pFIndex, 
									D3DFMT_INDEX16, m_vFPos, sizeof(VECTOR3));
	
	 
	pDev->SetRenderState(D3DRS_FILLMODE, oldmode);
	pDev->SetRenderState(D3DRS_LIGHTING, oldlight); 
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	 
 	pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);


	return TRUE;
}
 

 



/****************** end of file "Frustum.cpp" *********************************/

