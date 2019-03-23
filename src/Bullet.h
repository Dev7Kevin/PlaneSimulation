// Bullet.h: interface for the CBullet class.
//
//
////////////////////////////////////////////////////////////////////////////// 
 
#pragma once

#include "Unit.h"

class CCamera;

//
// class CBullet : �Ѿ��� �����ϴ� Ŭ���� (ADT)
//
class CBullet : public CUnit  
{	 
protected:
	//
	// �Ѿ� ���� ������.
	//
	UINT m_uDmg;				//�Ѿ� ������.

	//��ݽ� �߻� �ӵ�.: �� �ӵ����� ���� �����Ѵ�..(*)
	float m_fFireVelo;		
	float m_fResistance;		//�ӵ� ���װ�.(*) 
	float m_fGravity;			//�߷� ���װ�.(*)

	VECTOR3 m_vFireDir;			//��� ����.	
	 
	VECTOR3 m_vMaxRange;		//�ִ� ��Ÿ�(����)
	float	m_fMaxRange;		//�ִ� ��Ÿ�.(��Į��)	2004.06.21.
	VECTOR3 m_vMoveAccu;		//�Ѿ� �̵� �Ÿ� ���� ����.(�ִ� ��Ÿ� ����)
	 

	//
	// '���ϰ�', �̻��� ���� '�Ҳ�' ȿ���� ���� ��������Ʈ ���� : 2004.06.21.  
	//
	SPRDATA		m_Spr;			//�Ѿ� �޽��� 3D ��������Ʈ ��ü. �ܺο��� �����Ǵ� ���� ������ ����Ѵ�. 2004.06.21+
	COLOR		m_Color;		//�Ѿ� ����.

	// - �̻����� '�Ҳ�'�� ó���ϱ� ���� CRailGun ���� ����Ŭ������ �ø�. 
	// ī�޶� 2�� ������. :  pCamera = CameraSet[x] ���� ���Ǵ� pCamera �� ���� �Ҵ�� 
	// ī�޶��� �������� ���纻�̹Ƿ�  �ٸ� ī�޶�� ����Ǿ��� ��� 
	// ���ư��� �ִ� �Ѿ��� ī�޶��� ������ ����� ���� ���� ����.   
	// ���� '����'-CameraSet[x]- �� ���ϰ� Ŭ�������� �˾ƾ��ϹǷ� �������ΰ� �ʿ��ϴ�. 
	CCamera**	m_ppCamera;		



protected:
	
	CBullet(
		 LPDEVICE	pDev,
		 CB3Mesh*	pMesh,
 		 UINT		Dmg,				//�Ѿ� ������.
 		 float		fFireVelo,			//�ѱ� �߻� ���ӵ�
		 VECTOR3	vFireDir,			//�߻� ����.
		 float		fMaxRange,			//�ִ� ��Ÿ�...(�߻� ��ġ������ '��밪')
		 
		 SPRDATA*	pSpr,				//�Ѿ˿� ��Ʈ����Ʈ ������..  
		 //pMesh, pSpr ���߿� �ϳ��� �־�� �Ѵ�.
		 CCamera**	ppCam,				//ī�޶� ���� ������. 2004.06.19. 
		 COLOR		Color,				//�Ѿ� �÷�. 
 
		 float		fRadius,			//�浹�� �ݰ�.
		 LPANIINFO	pBoomAniInfo,		//���� �ִϸ��̼� ���� ����ü.04.06.23+ 
										//NULL �̸� ���õȴ�.
		 
		 VECTOR3	vPos,				//�Ѿ� �߻� ��ġ.
		 VECTOR3	vRot = VECTOR3(0.0f, 0.0f, 0.0f),	 
		 VECTOR3	vScale = VECTOR3(1.0f, 1.0f, 1.0f),	 
 		 VECTOR3	vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		 VECTOR3	vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
 		 );
public:

	//�Ѿ� �߰� �޼ҵ�.
 	const VECTOR3& GetFirDir() const { return m_vFireDir; }
	const VECTOR3& GetMaxRange() const { return m_vMaxRange; }
	float GetMaxRangeF() const { return m_fMaxRange; }				
	float GetMoveAccu() { return D3DXVec3Length(&m_vMoveAccu); }		
 	
	const UINT&	GetDmg() const { return m_uDmg; }	

	//'Bullet'�� �ִ� ��Ÿ� ���� �޼ҵ�. 
	void SetMaxRange(float fMaxRange);

	//'Bullet'�ʱ�ȭ �޼ҵ�. 
	void InitBullet(float fMaxRange);


	virtual ~CBullet();

	// �⺻ �޼ҵ�.
	virtual int Move(float dTime=0) = 0;
	virtual int Draw(float dTime=0) = 0;   
	virtual void Release() = 0;

};



/************** "end of file "Bullet.h" ***************************/ 
