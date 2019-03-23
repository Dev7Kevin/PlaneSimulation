/////////////////////////////////////////////////////////////////////////////
//
// Camera.h: ī�޶� Ŭ���� ���� ��� ����.
//
//
/////////////////////////////////////////////////////////////////////////////
 
#pragma once

//#include "../../Engine/B3X.h"
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL ���� ���� �ʼ����. 


/////////////////////////////////////////////////////////////////////////////
//
// 'ī�޶� ����' ����.
//
/////////////////////////////////////////////////////////////////////////////

typedef struct {

	float zn;
	float zf;
	float fovy;
	float ratio;

}CAMERA_LENS_INFO, *LPCAMERA_LENS_INFO;
#define LENS  CAMERA_LENS_INFO




/////////////////////////////////////////////////////////////////////////////
//
// ���� ī�޶� Ÿ�� : �ϴ� 4����...
//
/////////////////////////////////////////////////////////////////////////////
// ī�޶� �߰� Ÿ��. 2004.07.01. 
// - ������� '����'�� ���� ī�޶� ó���� ���� �߰���.
//
// FCT_OUTER_	0x0100 : �ܺ� ���� ī�޶� - �ܺο��� ����⸦ ���� �����Ƿ� 
//                       ���� '����' �ʴ´�.
// FCT_INNER_	0X0200 : ���� ���� ī�޶� - ����� ���ο� 'ž��' �� ������ 
//                       ����, '�����'�� �Բ� ���� �ȴ�.
enum //FIXCAMERA_TYPE
{
	FCT_FREE_		= 0x0001,		//����� - ����ī�޶�.
 	FCT_NOFIX_		= 0x0001,		//����� ī�޶�.. - �������� �ʴ´�. : �⺻ī�޶�.

	FCT_TRACKING_	= 0x0002,		//�������  + ���� ī�޶�. : ������ ��ġ���� '��ü'�� ��� �ֽ��Ѵ�.(�̵��ʵʤ�)

	FCT_FRONT_		= 0x0004,		//��ü����	'����' �ֽ� ī�޶� 
	FCT_RIGHT_		= 0x0008,		//			'������' �ֽ� ī�޶�.
	FCT_REAR_		= 0x0010,		//			'�Ĺ�' �ֽ� ī�޶�.
	FCT_LEFT_		= 0x0020,		//			'������' �ֽ� ī�޶�.
	
	FCT_PILOT_		= 0x0040,		//          ������ ī�޶�.(��������)

	//2004.07.01.
	// ī�޶� �߰� Ÿ��. : ������� '����'�� ���� ī�޶� ó�������� �߰���.
	FCT_OUTTER_ = 0x0100,			// '�ܺ�' ī�޶�. (���� ���� ����)
	FCT_INNER_  = 0x0200,			// '����' ī�޶�. (���� ���� ����)
	//FCT_MAX_
};

typedef UINT FIXCAMERA_TYPE;


/////////////////////////////////////////////////////////////////////////////

#define LIMIT(V, L)  {if(abs((V)) > (L))  (V) = (abs((V))/(V) * (L)); }

typedef VECTOR3 DIR3;		//��(����) ����.
typedef VECTOR3 POS3;		//��ġ.��ǥ.
typedef VECTOR3 ANG3;		//�ະ ȸ����.����.
//#define DIR3  VECTOR3			
//#define POS3  VECTOR3			
class CUnit;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
//
// class CCamera : 
//
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class CCamera  
{
protected:

	
	LPDEVICE	m_pDev;
	
	char			m_Name[80];	 
	FIXCAMERA_TYPE	m_FCType;		// ī�޶� Ÿ�� ����. 


	//
	// ī�޶� ���. 
	//
	MATRIX m_mView;			//�� ��ȯ ���.
	MATRIX m_mProj;		    //���� ���.


	//
	// ī�޶� ��ġ ���� ����.
	//
	VECTOR3 m_vEye,
		    m_vLookAt,
			m_vUp;				//���� ����.
 
	VECTOR3 m_vNView;			//�ü� (����) ����
	VECTOR3 m_vHorz;			//����  ����.
   

	//
	// ī�޶� '����' �� ���� ��������.
	//
	VECTOR3 m_vOrgEye,
		    m_vOrgLookAt,
			m_vOrgUp;			 

	VECTOR3 m_vOrgNView;		 
	VECTOR3 m_vOrgHorz;		 


	// 
	// ���� or ������ ������ ������.: 2004.06.11. 
	//
 	CUnit*			m_pUnit;			
 
	//
	// �������� ���� ��� ������ : 2004.06.17.
	//
	MATRIX m_mInverseView;		//ī�޶��� �����.
	MATRIX m_mBillMtx;			//�������� 'ȸ��' ��� 
	//-> ī�޶� ���ϵ��� �ϴ� �����.(�̵��� ����)

	
	//�þ� �� 
	float m_fFovY;			
	float m_fOrgFovY;
	float m_fRatio;
	float m_fZfar, m_fZnear;


protected:

 
	//ī�޶� ����� ���.
	void _MakeInverseMtx();

 		
public:

	CCamera(LPDEVICE pDev);
	CCamera(LPDEVICE pDev,
			char* name,
			FIXCAMERA_TYPE type,	//
			POS3 eye,					//ī�޶� ��ġ.
			DIR3 view = DIR3(0, 0, 1),	//ī�޶� �ü����⺤��			
			DIR3 up = DIR3(0, 1, 0),
			float fovy = D3DX_PI/4, 
			float ratio = 640.0f/480.0f, 
			float zn = 0.1f, 
			float Zf = 100000.0f		//@fix.:���̴�-�������Ұ��� �ȼ��浹�ذ��.
 			);
 
	virtual ~CCamera();

 
	// '�ʱⰪ' ����.
 	virtual void Reset();
 
	//ī�޶� ������Ʈ.
	virtual int Update(float dTime = 0.0f) PURE;
	//virtual void Update( CUnit* pObj = NULL, float dTime = 0.0f, int* pVal = NULL ){}


	//
	// ���� �������� ������ ó��.
	//
  	void WMove(VECTOR3 mov);			 
	//void WRotateX(float angle);		 
	//void WRotateY(float angle);
	//void WRotateZ(float angle);


	//
	// ī�޶� ���� ȸ��. 
	// angle : ����.	
	//void LRotate(VECTOR3 rot);		 
	virtual void LRotateX(float angle);			
	virtual void LRotateY(float angle);
	virtual void LRotateZ(float angle);
	
	//
	// ī�޶� ���� �̵�.
	//
	void LMoveX(float mov);
	void LMoveY(float mov);
	void LMoveZ(float mov);

	//ī�޶� ���� ���� ���.
	void SetLocalVectors();

	//ī�޶� �� ������ ����ó��. 	
	void OrthoNormalized();
 
	//void RotateLocal(VECTOR3 rot);
	//void MoveLocal(VECTOR3 rot);
 
 	//ī�޶� ������  Dev �� ����.
	void SetView(LPDEVICE pDev = NULL);

	//ī�޶� ���� ��ȯ ��Ű�� : ���� ��� ����(�ɼ�)
 	void SetProject( float fovy = D3DX_PI/4, 
					 float ratio = 640.0f/480.0f,
					 float Zn = 0.1f, 
					 float Zf = 100000.0f	//@fix. 
 					 );
	
	//�Ϲ� Accessor.
	const POS3& GetEye() const { return m_vEye; }
	const POS3& GetLookAt() const { return m_vLookAt; }
	const DIR3& GetUp() const { return m_vUp; }
	const DIR3& GetHorz() const { return m_vHorz; }
	const DIR3& GetNView() const { return m_vNView; } 
	const char* GetName() const { return m_Name; }			
	float GetFovY() const { return m_fFovY; }				
	float GetRatio() const { return m_fRatio; }		
	float GetZnear() const { return m_fZnear; }		
	float GetZfar() const { return m_fZfar; }		

	MATRIX* GetViewTM(){ return &m_mView; }		
	const MATRIX& GetViewMtx() const { return m_mView; }		
	const MATRIX& GetProjectMtx() const { return m_mProj; }			//@add. 2009.07.03.
	//#define GetViewMtx    GetViewTM				//ȣȯ���� ���� �ڵ�. 2009.07.01.

	void SetLookAt(const POS3& rvPos) { m_vLookAt = rvPos; }	
	void SetTarget(const CUnit& rUnit);						


	//ī�޶� Ÿ�� ���ϱ�.
	const FIXCAMERA_TYPE& GetType() const { return m_FCType; }		 
	inline BOOL IsType(FIXCAMERA_TYPE type){ return ((m_FCType & type) == type); }	 

	

	//ī�޶� ����� ���ϱ�.
	const MATRIX& GetInverseMtx() const { return m_mInverseView; }		
	//������ ��� ���.
	const MATRIX& GetBillMtx() const { return m_mBillMtx; }		

	
 
};







//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 
// 
// 
// class CCamFree : ����ī�޶�, 1��Ī ����
// 
// 
// 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CCamFree : public CCamera
{
protected:


public:
	CCamFree(LPDEVICE pDev, 	
			char* name,
			FIXCAMERA_TYPE type,	//
			POS3 eye,						//���� ��ǥ��.����.
			DIR3 view = VECTOR3(0, 0, 1),	//�ü�����. 
			DIR3 up = VECTOR3(0, 1, 0),
			float fovy = D3DX_PI/4, 
			float ratio = 640.0f/480.0f, 
			float zn = 0.1f, 
			float Zf = 100000.0f
		);
 	virtual ~CCamFree();

	//ī�޶� ������Ʈ.
	virtual int Update(float dTime = 0.0f);

};









//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 
// 
// 
// class CCamModelFix  : �� ���� ī�޶�
// 
// 
// 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CCamModelFix : public CCamera
{
protected:

	 
public:
	CCamModelFix(LPDEVICE pDev, 
				char* name,
				FIXCAMERA_TYPE type,	//
				const CUnit* pUnit,				//������ ��.
				POS3 eye,						//�𵨰����� ������ǥ������.����.
				DIR3 view,						//�𵨰��������� �ü�����.  
			 	DIR3 up = VECTOR3(0, 1, 0),
				float fovy = D3DX_PI/4, 
				float ratio = 640.0f/480.0f, 
				float zn = 0.1f, 
				float Zf = 100000.0f
				);

	virtual ~CCamModelFix();	


	virtual void Reset();

	//ī�޶� Ư�� ��ġ�� ������Ű��.
	//��� ��ǥ���� '����' �𵨰��� ��ǥ�̴�.����.
	void SetFixPosition( VECTOR3 vEye,		 
						 VECTOR3 vView,		 //�ü�����.
						 //VECTOR3 vLookAt,	  
						 VECTOR3 vUp,		  
						 const CUnit* pUnit	 //��ǥ ������.  
						); 

	//ī�޶� ���������ϱ�.
	void Tracking(const CUnit* pPlane = NULL);  


	//ī�޶� ������Ʈ.
	virtual int Update(float dTime = 0.0f);


};









//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 
// 
// 
// class CCamTracking  : ���� ���� + �� �ֽ� ī�޶�.
// 
// 
// 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CCamTracking : public CCamera
{
protected:



public:
	CCamTracking(LPDEVICE pDev, 
		char* name,
		FIXCAMERA_TYPE type,	//
		const CUnit* pUnit,				//������ ��.
		POS3 eye,						//���� ������ǥ 
  		DIR3 up = VECTOR3(0, 1, 0),		//��溤�ʹ� ���� ����..
		float fovy = D3DX_PI/4, 
		float ratio = 640.0f/480.0f, 
		float zn = 0.1f, 
		float Zf = 100000.0f
		);

	virtual ~CCamTracking();
	
	 
	//ī�޶� ������Ʈ.
	virtual int Update(float dTime = 0.0f);


};









//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 
// 
// 
// class CCamPilot  : ���Ϸ� ī�޶� : ��ġ����(ı��) + ���� ���� ī�޶�.
// 
// 
// 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CCamPilot : public CCamModelFix	
{
protected:

	 
	VECTOR3 m_vRotLimit;	//ȸ�� �ִ밪.
	VECTOR3 m_vRot;			//���� ȸ������.

public:
	CCamPilot(LPDEVICE pDev, 
		char* name,
		FIXCAMERA_TYPE type,	//
		const CUnit* pUnit,				//������ ��.
		VECTOR3 vRotLimit,				//ȸ�� �ִ밪.
		//float angleLimit,				//ȸ���� ���� �ִ밪.
		POS3 eye,						//�𵨰����� ������ǥ������.����.
		DIR3 view = VECTOR3(0, 0, -1),	//�𵨰��������� �ü�����
		DIR3 up = VECTOR3(0, 1, 0),
		float fovy = D3DX_PI/4, 
		float ratio = 640.0f/480.0f, 
		float zn = 0.1f, 
		float zf = 10000.0f		
		);

	virtual ~CCamPilot();	

	virtual void Reset();

	//ȸ���� �����ִ밪.
	void SetAngleLimit(float limit);

	//ī�޶� ���������ϱ�.
	//void Tracking(const CUnit* pPlane = NULL);  

	//�þ� ȸ���ϱ�.
	void LRotate(VECTOR3 vRot);
	//virtual void LRotateX(float angle);			
	//virtual void LRotateY(float angle);
	//virtual void LRotateZ(float angle);

	//�� ��/�ƿ�.
	virtual float ZoomIO(float v);


	//ī�޶� ������Ʈ.
	virtual int Update(float dTime = 0.0f);


};
 
/******************* end of file "Camera.h" ********************************/
