/////////////////////////////////////////////////////////////////////////////
//
// Plane.h:  ���� ���� ���� ��� ����. 
//
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
 
#include "Unit.h"

//
//  '�����' Ŭ���� : ���� �������ֵ��� �� Ŭ�������� �Ļ� �ȴ�.
//
class CPlane : public CUnit  
{
protected : 

	int		m_Hp;				//UINT -> INT �� ��ȯ.



	//2004.06.10.
	//'��'�� �߽����� �������� '����'�� ���� 3���� ���� �߰�. 
	VECTOR3	m_vUp,				//��� ����.
			m_vHorz,			//���� ����.
			m_vNView;			//�ü� ����.
	 

	//����� �ڼ� ��� ���� ������͵�. 		
	VECTOR3 m_vOrgUp, m_vOrgHorz, m_vOrgNView;		


	float m_fMaxVelo;		//�ִ���� �ӵ� : m/s.  100km/h = 27.7m/s
	float m_fThrottle;		//����Ʋ �� (0~110%) : 110% �� After-Bunner
	float m_fPower;			//Power = N * m/s... ������� �ʰ� ����.

	// ���� ���� (���� + �����ӵ�) ���� => ���� �ӵ� : �̻��� �߻翡 �ʿ�.
	//
	VECTOR3 m_vCurrDirVelo;		//ex) m_vPos += m_vCurrDirVelo * dTime;



	// 2004.07.01.
	//'����-��� ����' & '��ü �ջ� -������' �� ǥ���� ���� ������ 
	// : '��¦�� �����ش�' �̶�, 'ī�޶�' �� ��鸲�� ������ ��ģ��.
	// (������ ī�޶� ONLY!)
	VECTOR3 m_vShake;


protected:
	 
	
 	//������.: protected �� �ٲ�.
	CPlane(
		LPDEVICE pDev,
		 CB3Mesh* pMesh,
 		 UINT hp,

		 float		fRadius,			//�浹�� �ݰ�.
		 LPANIINFO	pBoomAniInfo,		//���� �ִϸ��̼� ���� ����ü.	04.06.23+ 
										//NULL �̸� ���õȴ�.
 		 VECTOR3 vPos, 
		 VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		 VECTOR3 vScale = VECTOR3(1.0f, 1.0f, 1.0f),
 		 VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		 VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
 		 );


	//�� ����/���� ���� ���ϱ�.
	void	SetVectors();
 

public:

	virtual ~CPlane();

	//���� �޼ҵ� �����ε�.
	void Reset();

	
	//�� �� ���� ȸ�� �޼ҵ�.
	MATRIX	LRotateX(float angle);
	MATRIX	LRotateY(float angle);
	MATRIX	LRotateZ(float angle);
	MATRIX  LRotate(VECTOR3 vAngle);

	//�� ȸ�� ���� ���ϱ�.
	//const VECTOR3& GetModelRotVec() const { return m_vModelRot; }
	//const MATRIX& GetModelRotMtx() const { return m_mModelRot; }
	//const VECTOR3& GetEye() const { return m_vEye; }
	//const VECTOR3& GetLookAt() const { return m_vLookAt; }
	const VECTOR3& GetUp() const { return m_vUp; }
	const VECTOR3& GetHorz() const { return m_vHorz; }
	const VECTOR3& GetNView() const { return m_vNView; } 

	float GetThrottle() { return m_fThrottle; };	 
	float GetMaxVelo() { return m_fMaxVelo; };		 
	float GetVelo();								 
	float GetSpeed();

	//����� ���� ����+�����ӵ� ��� 2004.06.21+
	const VECTOR3 GetCurrDirVelo() const { return m_vCurrDirVelo; }

	//��ü�� '��鸲' �� ���.
	const VECTOR3& GetShake() const { return m_vShake; }	


	//���� �⺻ �޼ҵ�.
	virtual int Move(float dTime=0) = 0;
	virtual int Draw(float dTime=0) = 0;   
	virtual void Release() = 0;


	//�浹 ���� �߰� �޼ҵ�.
	BOOL TakeDamage(int dmg);	//����.
	enum {
		EMY_DEAD_  = FALSE,
		EMY_ALIVE_ = TRUE
	};

};


/****************** end of file "Plane.h" *******************************/ 
