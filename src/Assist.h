/////////////////////////////////////////////////////////////////////////////
//
// Hero.h:  ���ΰ� Ŭ���� ����� ���� ��� ����.
//
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
 
#include "Plane.h"

//
// ���ΰ� Ŭ���� 
//
class CAssist : public CPlane  
{
protected:

	
public:
	//CHero();				 
	CAssist(
		 LPDEVICE pDev,
		 CB3Mesh* pMesh,
 		 UINT hp,
		 
		 float		fRadius,			//�浹�� �ݰ�.
		 LPANIINFO	pBoomAniInfo,		//���� �ִϸ��̼� ���� ����ü.04.06.23+ 
										// NULL �̸� ���õȴ�. 
		 VECTOR3 vPos, 
		 VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		 VECTOR3 vScale = VECTOR3(0.1f, 0.1f, 0.1f),
 		 VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		 VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
		 );

	virtual ~CAssist();

 	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

};