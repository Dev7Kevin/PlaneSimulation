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
// class CStar : '��' Ŭ����. : Point sprite �� �����Ѵ�.
//
class CStar : public CUnit  
{
protected:

 	int			m_Cnt;			//'��'�� ����.
	float		m_fSize;		//'�� ����' ������ : �ػ� ���� �ٸ���.
 	CPlane*		m_pHero;		//'���ΰ�' ����...
	

	//���� ���� ����.
	typedef struct {
		VECTOR3 vPos;			//��ġ.
		//float	fSize;			//ũ��.
		DWORD	dwColor;		//����.
	}VERTEX_STAR;

	enum {
		FVF_STARS_ = D3DFVF_XYZ | D3DFVF_DIFFUSE,
		//DFVF_STARS_ = D3DFVF_XYZ|D3DFVF_PSIZE|D3DFVF_DIFFUSE, 

		//DEBUG_STARDUST_ = 1000,	//������ ������.
	};
	 	 
	LPVERTEXBUFFER	m_pVBuffer;		//���� ���� ������.

 	SPRDATA		m_Spr;				//'��' ��� ���� �ؽ���.
 	CCamera**	m_ppCamera;			//'����' Ȱ��ȭ�� ī�޶��� 2�� ������. 
									//(�׽�Ʈ��)	

	//
	// '������' ���� ����Ʈ.. ����.
	//
 	list<VERTEX_STAR*> m_StarDustList;					//'���� ����'...
	typedef list<VERTEX_STAR*>::iterator itor;

	
 
public:

	CStar(
	 LPDEVICE	pDev,			//����̽� ������.
	 int		Cnt,			//�� ����.
 	 SPRDATA*	pSpr,			//'��' ��������Ʈ ������.
 	 CPlane*	pHero,			//���ΰ� ������.
	 CCamera**	ppCamera,		//'����' Ȱ��ȭ�� ī�޶��� 2�� ������.
	 int		Width			//'���÷���' ��� : '������'�� ũ�⿡ �����Ѵ�.	 
  	 );


 	virtual ~CStar();

	// �⺻ �޼ҵ�.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);	 
	virtual void Release();

	// �߰� �޼ҵ�.
	BOOL Init();
	BOOL ResetVBuffer();					//���� ���� ����.
	BOOL StarDustUpdate(VECTOR3 vPos);		//������ ������Ʈ..


};


/****************** end of file "Star.h" **********************************/ 
