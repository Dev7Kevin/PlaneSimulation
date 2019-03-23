///////////////////////////////////////////////////////////////////////////////
//
// PlasmaCannon.h : �ö�� �ɳ� ��� ����.
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
	//�ö���� ���� ����ü.
	struct VTX_PLASMACANNON
	{
		VECTOR3 vPos;
 		float	U,V;
	};
	
	enum {
		FVF_PLASMACANNON_ = D3DFVF_XYZ | D3DFVF_TEX1,

		PC_SHOW_ = 0,			//plasma cannon ��¸��.
		PC_DISAPPEAR_,			// �������. (fade-out)
 		PC_DELETE_,				// ��ü �����ϱ�.

	};


protected:
	
	float	m_fRadius;				// ���� ������ - '����'�� ������ �ȴ�.
	float	m_fLength;				// ���� ����. 
 	//DWORD	m_Color;				// ���� ����. -> CBullet �� ���� ���.

	UINT	m_State;				// ���� '������' ���� : show -> disappear -> delete.

	DWORD	m_ShowTime;				// '�����ֱ�' �ð�: �̽ð��� ���ϸ�, '�������.'
	DWORD	m_OldShowTime;
	float	m_fAlpha;				// fade-out �� ���İ�.

	// CBullet �ǰ��� ���.
	//SPRDATA		m_sprPC;		//��������Ʈ  
 	//CCamera**	m_ppCam;			//ī�޶� ���������� (������)

	// ���� '����' �� ����. : ��������
	VECTOR3 m_vNView,
			m_vUp,
			m_vHorz;

	// ���� '���κ�' �� ����. : ��������
	VECTOR3 m_vEndNView,
			 //m_vEndUp, -> m_vUp �� ����.
			 m_vEndHorz;
			
	 

	VTX_PLASMACANNON* m_pVB;			//���� ���� : '���� ��ȯ' �� ������.(��¿�)
 	static VTX_PLASMACANNON m_OrgVB[6];	//���� �������� : '���� ��'.

	//static list<CPlasmaCannon*> m_PlasmaCannonList;
	//typedef list<CPlasmaCannon*>::iterator PC_Itor;

	static int m_PCCount;		//PC ���� ����.

protected:

	//�� ���¿� ���� ������ ���...
	int Show(float dTime);		
	int Disappear(float dTime);


public:
	CPlasmaCannon(
 		LPDEVICE pDev,
		//CB3Mesh* pMesh,		//�Ѿ˿� �޽� ������. -> ������� ����
		SPRDATA* pSpr,			//�Ѿ˿� ��Ʈ����Ʈ ������.. 
								//pMesh, pSpr ���߿� �ϳ��� �־�� �Ѵ�.
		CCamera** ppCam,		//ī�޶� ���� ������. 2004.06.19. ����.

		float	fRadius,		// ���� ������ - '����'�� ������ �ȴ�.
		float	fLength,		// ���� ����. 
		UINT	Dmg,			// ������.
		COLOR	Color,			// �÷�.

		DWORD	ShowTime,		// '�����ֱ�' �ð�: �̽ð��� ���ϸ�, '�������.'
 		float	fAlpha,			// fade-out �� ���İ�.

		float	fFireSpeed,		//�߻� �ӵ�.
		VECTOR3 vFireDir,		//�߻� ����.
		VECTOR3 vUp,			//�߻��, '��ü�� 'up' ����.
		VECTOR3 vHorz,			//�߻��  '��ü�� ������'
		float	fMaxRange,		//�ִ� ��Ÿ�...

		VECTOR3 vPos,			//�Ѿ� �߻� ��ġ.
		VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		VECTOR3 vScale = VECTOR3(1.0f, 1.0f, 1.0f),
		VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f) 
		);

	virtual ~CPlasmaCannon(void);


	//�������̵� �޼ҵ�.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();


	//���� �޼ҵ�.
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




































