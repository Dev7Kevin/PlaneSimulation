/////////////////////////////////////////////////////////////////////////////
//
// Camera.h: 카메라 클래스 관련 헤더 파일.
//
//
/////////////////////////////////////////////////////////////////////////////
 
#pragma once

//#include "../../Engine/B3X.h"
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL 버전 엔진 필수헤더. 


/////////////////////////////////////////////////////////////////////////////
//
// '카메라 렌즈' 정보.
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
// 고정 카메라 타입 : 일단 4방향...
//
/////////////////////////////////////////////////////////////////////////////
// 카메라 추가 타입. 2004.07.01. 
// - 비행기의 '떨림'에 따른 카메라 처리를 위해 추가됨.
//
// FCT_OUTER_	0x0100 : 외부 조망 카메라 - 외부에서 비행기를 보고 있으므로 
//                       같이 '떨지' 않는다.
// FCT_INNER_	0X0200 : 내부 조망 카메라 - 비행기 내부에 '탑승' 한 것으로 
//                       보고, '비행기'와 함께 떨게 된다.
enum //FIXCAMERA_TYPE
{
	FCT_FREE_		= 0x0001,		//비고정 - 자유카메라.
 	FCT_NOFIX_		= 0x0001,		//비고정 카메라.. - 추적하지 않는다. : 기본카메라.

	FCT_TRACKING_	= 0x0002,		//월드고정  + 추적 카메라. : 임의의 위치에서 '물체'를 계속 주시한다.(이동않됨ㄴ)

	FCT_FRONT_		= 0x0004,		//물체고정	'전방' 주시 카메라 
	FCT_RIGHT_		= 0x0008,		//			'우측방' 주시 카메라.
	FCT_REAR_		= 0x0010,		//			'후방' 주시 카메라.
	FCT_LEFT_		= 0x0020,		//			'좌측방' 주시 카메라.
	
	FCT_PILOT_		= 0x0040,		//          조종사 카메라.(시점제한)

	//2004.07.01.
	// 카메라 추가 타입. : 비행기의 '떨림'에 따른 카메라 처리때문에 추가됨.
	FCT_OUTTER_ = 0x0100,			// '외부' 카메라. (떨림 영향 없음)
	FCT_INNER_  = 0x0200,			// '내부' 카메라. (떨림 영향 받음)
	//FCT_MAX_
};

typedef UINT FIXCAMERA_TYPE;


/////////////////////////////////////////////////////////////////////////////

#define LIMIT(V, L)  {if(abs((V)) > (L))  (V) = (abs((V))/(V) * (L)); }

typedef VECTOR3 DIR3;		//뷰(전방) 벡터.
typedef VECTOR3 POS3;		//위치.좌표.
typedef VECTOR3 ANG3;		//축별 회전각.라디안.
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
	FIXCAMERA_TYPE	m_FCType;		// 카메라 타입 정보. 


	//
	// 카메라 행렬. 
	//
	MATRIX m_mView;			//뷰 변환 행렬.
	MATRIX m_mProj;		    //투영 행렬.


	//
	// 카메라 위치 벡터 관련.
	//
	VECTOR3 m_vEye,
		    m_vLookAt,
			m_vUp;				//수직 벡터.
 
	VECTOR3 m_vNView;			//시선 (단위) 벡터
	VECTOR3 m_vHorz;			//수평  벡터.
   

	//
	// 카메라 '복구' 시 사용될 원점값들.
	//
	VECTOR3 m_vOrgEye,
		    m_vOrgLookAt,
			m_vOrgUp;			 

	VECTOR3 m_vOrgNView;		 
	VECTOR3 m_vOrgHorz;		 


	// 
	// 고정 or 추적할 유닛의 포인터.: 2004.06.11. 
	//
 	CUnit*			m_pUnit;			
 
	//
	// 빌보딩을 위한 행렬 데이터 : 2004.06.17.
	//
	MATRIX m_mInverseView;		//카메라의 역행렬.
	MATRIX m_mBillMtx;			//빌보드의 '회전' 행렬 
	//-> 카메라를 향하도록 하는 행렬임.(이동값 제외)

	
	//시야 값 
	float m_fFovY;			
	float m_fOrgFovY;
	float m_fRatio;
	float m_fZfar, m_fZnear;


protected:

 
	//카메라 역행렬 계산.
	void _MakeInverseMtx();

 		
public:

	CCamera(LPDEVICE pDev);
	CCamera(LPDEVICE pDev,
			char* name,
			FIXCAMERA_TYPE type,	//
			POS3 eye,					//카메라 위치.
			DIR3 view = DIR3(0, 0, 1),	//카메라 시선방향벡터			
			DIR3 up = DIR3(0, 1, 0),
			float fovy = D3DX_PI/4, 
			float ratio = 640.0f/480.0f, 
			float zn = 0.1f, 
			float Zf = 100000.0f		//@fix.:레이더-프러스텀과의 픽셀충돌해결용.
 			);
 
	virtual ~CCamera();

 
	// '초기값' 복구.
 	virtual void Reset();
 
	//카메라 업데이트.
	virtual int Update(float dTime = 0.0f) PURE;
	//virtual void Update( CUnit* pObj = NULL, float dTime = 0.0f, int* pVal = NULL ){}


	//
	// 월드 공간에서 움직임 처리.
	//
  	void WMove(VECTOR3 mov);			 
	//void WRotateX(float angle);		 
	//void WRotateY(float angle);
	//void WRotateZ(float angle);


	//
	// 카메라 로컬 회전. 
	// angle : 라디안.	
	//void LRotate(VECTOR3 rot);		 
	virtual void LRotateX(float angle);			
	virtual void LRotateY(float angle);
	virtual void LRotateZ(float angle);
	
	//
	// 카메라 로컬 이동.
	//
	void LMoveX(float mov);
	void LMoveY(float mov);
	void LMoveZ(float mov);

	//카메라 로컬 벡터 계산.
	void SetLocalVectors();

	//카메라 각 벡터의 직교처리. 	
	void OrthoNormalized();
 
	//void RotateLocal(VECTOR3 rot);
	//void MoveLocal(VECTOR3 rot);
 
 	//카메라 정보를  Dev 에 설정.
	void SetView(LPDEVICE pDev = NULL);

	//카메라 렌즈 교환 시키기 : 투영 행렬 설정(옵션)
 	void SetProject( float fovy = D3DX_PI/4, 
					 float ratio = 640.0f/480.0f,
					 float Zn = 0.1f, 
					 float Zf = 100000.0f	//@fix. 
 					 );
	
	//일반 Accessor.
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
	//#define GetViewMtx    GetViewTM				//호환성을 위한 코드. 2009.07.01.

	void SetLookAt(const POS3& rvPos) { m_vLookAt = rvPos; }	
	void SetTarget(const CUnit& rUnit);						


	//카메라 타입 구하기.
	const FIXCAMERA_TYPE& GetType() const { return m_FCType; }		 
	inline BOOL IsType(FIXCAMERA_TYPE type){ return ((m_FCType & type) == type); }	 

	

	//카메라 역행렬 구하기.
	const MATRIX& GetInverseMtx() const { return m_mInverseView; }		
	//빌보드 행렬 얻기.
	const MATRIX& GetBillMtx() const { return m_mBillMtx; }		

	
 
};







//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 
// 
// 
// class CCamFree : 자유카메라, 1인칭 시점
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
			POS3 eye,						//월드 좌표임.주의.
			DIR3 view = VECTOR3(0, 0, 1),	//시선벡터. 
			DIR3 up = VECTOR3(0, 1, 0),
			float fovy = D3DX_PI/4, 
			float ratio = 640.0f/480.0f, 
			float zn = 0.1f, 
			float Zf = 100000.0f
		);
 	virtual ~CCamFree();

	//카메라 업데이트.
	virtual int Update(float dTime = 0.0f);

};









//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 
// 
// 
// class CCamModelFix  : 모델 고정 카메라
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
				const CUnit* pUnit,				//고정될 모델.
				POS3 eye,						//모델공간의 로컬좌표기준임.주의.
				DIR3 view,						//모델공간에서의 시선벡터.  
			 	DIR3 up = VECTOR3(0, 1, 0),
				float fovy = D3DX_PI/4, 
				float ratio = 640.0f/480.0f, 
				float zn = 0.1f, 
				float Zf = 100000.0f
				);

	virtual ~CCamModelFix();	


	virtual void Reset();

	//카메라를 특정 위치에 고정시키기.
	//모든 좌표값은 '유닛' 모델공간 좌표이다.주의.
	void SetFixPosition( VECTOR3 vEye,		 
						 VECTOR3 vView,		 //시선벡터.
						 //VECTOR3 vLookAt,	  
						 VECTOR3 vUp,		  
						 const CUnit* pUnit	 //목표 포인터.  
						); 

	//카메라 유닛추적하기.
	void Tracking(const CUnit* pPlane = NULL);  


	//카메라 업데이트.
	virtual int Update(float dTime = 0.0f);


};









//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 
// 
// 
// class CCamTracking  : 월드 고정 + 모델 주시 카메라.
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
		const CUnit* pUnit,				//추적할 모델.
		POS3 eye,						//월드 고정좌표 
  		DIR3 up = VECTOR3(0, 1, 0),		//상방벡터는 거의 고정..
		float fovy = D3DX_PI/4, 
		float ratio = 640.0f/480.0f, 
		float zn = 0.1f, 
		float Zf = 100000.0f
		);

	virtual ~CCamTracking();
	
	 
	//카메라 업데이트.
	virtual int Update(float dTime = 0.0f);


};









//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 
// 
// 
// class CCamPilot  : 파일럿 카메라 : 위치고정(캇핏) + 시점 제한 카메라.
// 
// 
// 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CCamPilot : public CCamModelFix	
{
protected:

	 
	VECTOR3 m_vRotLimit;	//회전 최대값.
	VECTOR3 m_vRot;			//현재 회전누계.

public:
	CCamPilot(LPDEVICE pDev, 
		char* name,
		FIXCAMERA_TYPE type,	//
		const CUnit* pUnit,				//고정될 모델.
		VECTOR3 vRotLimit,				//회전 최대값.
		//float angleLimit,				//회전각 제한 최대값.
		POS3 eye,						//모델공간의 로컬좌표기준임.주의.
		DIR3 view = VECTOR3(0, 0, -1),	//모델공간에서의 시선벡터
		DIR3 up = VECTOR3(0, 1, 0),
		float fovy = D3DX_PI/4, 
		float ratio = 640.0f/480.0f, 
		float zn = 0.1f, 
		float zf = 10000.0f		
		);

	virtual ~CCamPilot();	

	virtual void Reset();

	//회전값 제한최대값.
	void SetAngleLimit(float limit);

	//카메라 유닛추적하기.
	//void Tracking(const CUnit* pPlane = NULL);  

	//시야 회전하기.
	void LRotate(VECTOR3 vRot);
	//virtual void LRotateX(float angle);			
	//virtual void LRotateY(float angle);
	//virtual void LRotateZ(float angle);

	//줌 인/아웃.
	virtual float ZoomIO(float v);


	//카메라 업데이트.
	virtual int Update(float dTime = 0.0f);


};
 
/******************* end of file "Camera.h" ********************************/
