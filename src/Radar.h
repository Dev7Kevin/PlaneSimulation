//////////////////////////////////////////////////////////////////////////
//
// Radar.h
//
// 3D ���̴� ���� 
//
//
//////////////////////////////////////////////////////////////////////////

#pragma once
 
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL ���� ���� �ʼ����. 

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
	typedef struct {				// ���̴ٿ� �ﰢ��
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


	CEnemy*	 m_pTarget;			//Lock-On �� �� ����� ������. (��������)

	//
	// ���̴� '��ĳ�� ����' + ���� ���� ���� �����͵�.
	//
	list<CEnemy*>	m_ScanList;				//'Lock-on' ������ ����Ʈ. : �þ߿� ���� ���� ����Ʈ.
	CFrustum*		m_pScanArea;			//���̴��� ��ĳ�� ���� 
	MATRIX			m_mScanView;			//���̴� ��ĳ�� ���� ��ȯ ���. : ���� ��ġ�� ��ĳ�� �������� ��ȯ ��Ű�� ���� ���.
											//ī�޶� ���� ��ȯ ����� ������ ����.
	int				m_TargetNum;			//���� Lock-On �� ��ǥ���� ��ȣ. : ScanList ������ ����..
											//'���� Ÿ��'�� ������ ����ϸ� 1���� ������. (��ü ������ ���� ��.)


protected:

	BOOL	Init();
	void	Release();
 	void	DrawCircle();

	VECTOR3	CheckCollision(VECTOR3 vTargetPos,VECTOR3 vPos, float fTargetRadius);

	//���ΰ� ������� ��ġ/������ �������� ��ĳ�� ���� ��ȯ ����� �����. 
	//CCamera::SetView �� ����.
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
	// �̻����� ��Ƽ�� ȣ�� �޼ҵ�. 2004.11.15.	    //	
	// ->FCS ���� ó���ϴ� ���� �ù����̼� �� ����! //
	//----------------------------------------------//
	int		SelectTarget(BOOL bNext = FALSE);			//��									
	const CEnemy* GetTarget() const { return m_pTarget; }	
	void		  ClearTarget() { m_pTarget = NULL; }		//��ǥ�� Ŭ����.


	//���̴� ����. 2004.11.16.
	void ResetTargets(list<CEnemy*> &EnemyList);	//���� ����Ʈ �缳��.

	//��ǥ�� Ž��. 2005.02.25.
	void SearchTargets();				//���� ��ĳ�� 
	int	 GetScanCnt();				//��ĳ�׵� �� ������ ����.	
	
	//���� ����Ʈ ���.��
	//list<CEnemy*>& GetScanList() { return m_ScanList; }
	list<CEnemy*>* GetScanList() { return &m_ScanList; }


	enum {
		SELECT_NEXT_ = TRUE,
		LOCK_ON_ = TRUE,
		LOCK_OFF_ = FALSE,
	};

};


/******************** end of file "Radar.h" ******************************/ 
