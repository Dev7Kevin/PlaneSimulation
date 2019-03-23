// Frustum.h: interface for the CFrustum class.
//
//
////////////////////////////////////////////////////////////////////////////// 

 
#pragma once

#include <d3d9.h>
#include <d3dx9.h>

typedef D3DXVECTOR3			VECTOR3;
typedef D3DXVECTOR4			VECTOR4;
typedef D3DXMATRIX			MATRIX;
typedef D3DMATERIAL9		MATERIAL;
typedef D3DXCOLOR			COLOR;
typedef LPDIRECT3DDEVICE9	LPDEVICE;


//////////////////////////////////////////////////////////////////////////////
//
// struct PLANE : 절두체 컬링용 육면체의 각 평면을 구성할 구조체.
//
struct PLANE
{
	float _A, _B, _C, _D;  

	PLANE::PLANE():_A(0),_B(0),_C(0),_D(0){}
	void PlaneFromPoint(D3DXVECTOR3& vP0, D3DXVECTOR3& vP1, D3DXVECTOR3& vP2);

};


//////////////////////////////////////////////////////////////////////////////
//
// class CFrustum : 경계구 방식의 절두체 컬링 을 위한 클래스.
class CFrustum  
{
private:
	PLANE	m_Left ;
	PLANE	m_Right;
	PLANE	m_Top;
	PLANE	m_Bottom;
	float	m_fFar;
	float	m_fNear;

	float		m_fDist;		//평면과의 거리..
	//D3DXVECTOR3 m_vViewPos;	//카메라 위치.

	//float		m_fFactor;	사용않합니다.
	
protected:
	//디버깅용.
	VECTOR3 m_vFPos[5];
	USHORT	m_pFIndex[18]; 
	void _InitMesh(VECTOR3 p[5]);


public:
	CFrustum();
	CFrustum(float fovY, float ratio, float n, float f);
 	~CFrustum();

	float GetDist() { return m_fDist; }		

 	void Init(float fovY, float ratio, float n, float f);

	//뷰공간 처리용.
	int	 IsVisible(D3DXMATRIX* pViewTM, D3DXVECTOR3* pTarget, float TargetRadius);	

	//월드 공간 기준 처리용.@fix. 2009.06.24.
	//int  Update(D3DXMATRIX* pViewTM);
	//int  IsVisible(D3DXVECTOR3* pTarget, float TargetRadius);		 

	//디버깅용. 2009.07.03.	
	int Draw(LPDEVICE pDev, COLOR col, MATRIX* pMtx = NULL, BOOL wire = TRUE);
	
};



/************************** end of file "Frustum.h" ************************/ 
