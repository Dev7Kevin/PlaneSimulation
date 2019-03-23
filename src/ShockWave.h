// ShockWave.h: interface for the CShockWave class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#pragma once
 

#include "Unit.h"

//////////////////////////////////////////////////////////////////////////////
//
// class CShockWave 
//
// '�̻���' ���� '������' ����� ȿ���� ����Ѵ�.
//  2004.06.30. mad_dog
//
class CShockWave : public CUnit  
{	
 	float	m_fSWRadius;			//����� ���� �ݰ�.
	//float	m_fSWMaxRadius;			//����� �ְ� �ݰ�.
	float	m_fSWThick;				//����� '��'�� �β�.
	COLOR	m_Color;				//����� '����'

	typedef struct {				//����Ŀ� ���� ����ü.
		VECTOR3 vPos;
	 	FLOAT	tu, tv;
	}VERTEX_SHOCKWAVE;

	enum {
	  FVF_SHOCKWAVE_ = D3DFVF_XYZ | D3DFVF_TEX1,
 	};
	

 	LPVERTEXBUFFER	m_pVB;			//����� '��' ������ ���� ����..
	int				m_VertexCnt;	//�� ������ ����.

	//���̵� �ƿ�..
	BOOL	m_bFadeOut;
	FLOAT	m_fAlpha;
		 
public:
	CShockWave(
		 LPDEVICE	pDev,			//����̽� ������.
 		 float		fSWRadius,		//����� ���� �ݰ�. 
 		 float		fSWThick,		//�����  �β�.
		 COLOR		Color,			//���� ����. 
 		 LPANIINFO	pBoomAniInfo,	//���� �ִϸ��̼� ���� ����ü.	04.06.23+ 
									//: NULL �̸� ���õȴ�.
 		 VECTOR3	vPos			//'���� ��ġ'-> ������� �߽���ǥ.

 		 //���� -> �ʿ� ����.
		 //VECTOR3 vScale = VECTOR3(1, 1, 1),			//������. -> �����ϰ�...
  		 //VECTOR3 movespeed = VECTOR3(0, 0, 0),		//�̵��ӵ�.
		 //VECTOR3 anglespeed = VECTOR3(0, 0, 0)		//ȸ�� �ӵ�.
		 //CB3Mesh* pMesh,								//�޽� ������.
 		);


	virtual ~CShockWave();

	// �⺻ �޼ҵ�.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);		
	virtual void Release();

	//�߰� �޼ҵ�.
	BOOL Init();

	void SetAlpha(float alpha){ m_fAlpha = alpha; }
	void SetFadeOutState(BOOL bSet) { m_bFadeOut = bSet; }

	const float& GetSWRadius(){ return m_fSWRadius;}
	const float& GetAlpha(){ return m_fAlpha; }
	const BOOL&  GetFadeOutState() const { return m_bFadeOut; } 
	
	
	

};


/***************** end of file "ShockWave.h" ****************************/ 
