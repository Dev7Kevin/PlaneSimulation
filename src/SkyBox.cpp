// SkyBox.cpp: implementation of the CSkyBox class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL ���� ���� �ʼ����. 
#include "SkyBox.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CSkyBox::CSkyBox( 
		LPDEVICE	pDev,				//����̽� ������.
		CPlane*		pHero,				//���ΰ� ���� ������.
		CCamera**	ppCamera,			//ī�޶� ������.
		
		SPRDATA		pSpr[SBF_MAX_],		//��ī�� �ڽ��� �ؽ��� ����� ��������Ʈ �迭.
		VECTOR3		vPos/*=VECTOR3(0, 0, 0)*/,	//'���� ��ġ'
		VECTOR3		vScale /*=VECTOR3(512, 512, 512)*/		//�ڽ� ũ�� : �� 512 ����.
		)
:CUnit(vPos, VECTOR3(0, 0, 0), vScale, 
	   VECTOR3(0, 0, 0), VECTOR3(0, 0, 0), 0.0f, NULL, NULL, pDev)	 
,m_pHero(pHero), m_ppCamera(ppCamera)
{ 
	//�ؽ��� ���� ����.
	memcpy(m_Spr, pSpr, sizeof(SPRDATA)*SBF_MAX_);		
	
	//�ؽ��� ��� Ŭ����.
	//for(int i=SBF_TOP_; i<SBF_MAX_; i++)
	//{
	//	D3DXMatrixIdentity(&m_SprTM[i]);
	//}


	//ī�޶� �� '��'�� ��ī�� �ڽ��� '�߽�'�� �ȴ�.
	m_vPos = (*m_ppCamera)->GetEye(); 


	Init();
}


//////////////////////////////////////////////////////////////////////////////
//
CSkyBox::~CSkyBox()
{
	Release();
}



//////////////////////////////////////////////////////////////////////////////
//
// ���� �޼ҵ� ����.
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//
//  �ϴû��ڿ� �޽� ���� : ũ��  1x1x1 ¥�� �޽�.
//  UV ��ǥ�� '����' ���� �ٶ󺼶��� �������� ó���ȴ�.
//  ������ -Z  ������ -X ��δ� +Y
HRESULT CSkyBox::_CreateVB()
{ 

	SBOX_VTX	vertices[] = {
	
	// ������ ����->����(-Z) �� ���� �������� ����/uv ����.
	// �̶�, ������ -x ��δ� +Y �� �ȴ�. ����
	//0�� �� : ����.
	{  0.5f, 0.5f, -0.5f,   0.0f, 0.0f },		//v0	- ���� ���.
	{ -0.5f, 0.5f, -0.5f,   1.0f, 0.0f },		//v1	- ���� ���.
	{  0.5f,-0.5f, -0.5f,   0.0f, 1.0f },		//v2	- ���� �ϴ�.

	{  0.5f,-0.5f, -0.5f,   0.0f, 1.0f },		//v3	- ���� �ϴ�.
	{ -0.5f, 0.5f, -0.5f,   1.0f, 0.0f },		//v4	- ���� ���.
	{ -0.5f,-0.5f, -0.5f,   1.0f, 1.0f },		//v5	- ���� �ϴ�.


	//1�� �� : ������
	{  -0.5f, 0.5f, -0.5f,   0.0f, 0.0f },		//v6	 
	{  -0.5f, 0.5f,  0.5f,   1.0f, 0.0f },		//v7	 
	{  -0.5f,-0.5f, -0.5f,   0.0f, 1.0f },		//v8	 

	{  -0.5f,-0.5f, -0.5f,   0.0f, 1.0f },		//v9	 
	{  -0.5f, 0.5f,  0.5f,   1.0f, 0.0f },		//v10	 
	{  -0.5f,-0.5f,  0.5f,   1.0f, 1.0f },		//v11	 



	//2�� �� : �޸�
	{ -0.5f, 0.5f, 0.5f,   0.0f, 0.0f },		//v12 
	{  0.5f, 0.5f, 0.5f,   1.0f, 0.0f },		//v13 
	{ -0.5f,-0.5f, 0.5f,   0.0f, 1.0f },		//v14 

	{ -0.5f,-0.5f, 0.5f,   0.0f, 1.0f },		//v15 
	{  0.5f, 0.5f, 0.5f,   1.0f, 0.0f },		//v16 
	{  0.5f,-0.5f, 0.5f,   1.0f, 1.0f },		//v17 


	//3�� �� : ������
	{ 0.5f, 0.5f,  0.5f,   0.0f, 0.0f },		//v18 
	{ 0.5f, 0.5f, -0.5f,   1.0f, 0.0f },		//v19 
	{ 0.5f,-0.5f,  0.5f,   0.0f, 1.0f },		//v20 

	{ 0.5f,-0.5f,  0.5f,   0.0f, 1.0f },		//v21 
	{ 0.5f, 0.5f, -0.5f,   1.0f, 0.0f },		//v22 
	{ 0.5f,-0.5f, -0.5f,   1.0f, 1.0f },		//v23 


	//4�� �� : �� �Ѳ�.: ����->���� �ٶ󺸴� ����..
	{  0.5f, 0.5f,  0.5f,   0.0f, 0.0f },		//v24 
	{ -0.5f, 0.5f,  0.5f,   1.0f, 0.0f },		//v25 
	{  0.5f, 0.5f, -0.5f,   0.0f, 1.0f },		//v26 

	{  0.5f, 0.5f, -0.5f,   0.0f, 1.0f },		//v27 
	{ -0.5f, 0.5f,  0.5f,   1.0f, 0.0f },		//v28 
 	{ -0.5f, 0.5f, -0.5f,   1.0f, 1.0f },		//v29 

	//5�� �� : �Ʒ� �Ѳ�.����->�Ʒ��� �ٶ󺸴� ����.
	{  0.5f, -0.5f, -0.5f,   0.0f, 0.0f },		//v30 
	{ -0.5f, -0.5f, -0.5f,   1.0f, 0.0f },		//v31 
	{  0.5f, -0.5f,  0.5f,   0.0f, 1.0f },		//v32 

	{  0.5f, -0.5f,  0.5f,   0.0f, 1.0f },		//v33 
	{ -0.5f, -0.5f, -0.5f,   1.0f, 0.0f },		//v34 
	{ -0.5f, -0.5f,  0.5f,   1.0f, 1.0f },		//v35 
	};
 


	//
	// ���� ���� ����.
	//
	if(FAILED(m_pDev->CreateVertexBuffer(sizeof(vertices),  
		0, FVF_SKYBOX, D3DPOOL_MANAGED, &m_pVBSBox, NULL)))
	{
		return E_FAIL;
	}

	//���� ä���.
	VOID*	pVB = NULL;

	if(FAILED(m_pVBSBox->Lock(0, NULL, (void**)&pVB, 0 )))
	{
		return E_FAIL;
	}

	memcpy(pVB, vertices, sizeof(vertices));

	m_pVBSBox->Unlock();

	return S_OK;

}


//////////////////////////////////////////////////////////////////////////////
//
// �޼ҵ� ����.
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
int CSkyBox::Init()
{
	_CreateVB();

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////////
//
void CSkyBox::Release()
{
	//N/A
	SAFE_RELEASE(m_pVBSBox);		//@Update
}







//////////////////////////////////////////////////////////////////////////////
//
int CSkyBox::Move(float dTime /* = 0 */)
{
	//ī�޶� �� '��'�� ��ī�� �ڽ��� '�߽�'�� �ȴ�.
	m_vPos = (*m_ppCamera)->GetEye(); 

	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);
	m_mTM = m_mScale * m_mTranslation;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
int CSkyBox::Draw(float dTime /* = 0 */)
{
	//���� �ɼ� ���� ���.
	//..... 

	//�ؽ��� ���͸� �ɼ� ����.

	//�ɼ� ����.
	m_pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);	//���� ���� ����.
	m_pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//�Ȱ�..���� �ɼǵ� ����...
	m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);			//�ø� ����.
	m_pDev->SetRenderState(D3DRS_LIGHTING, FALSE); 
	m_pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Z-Buffer Write ����. 
	m_pDev->SetRenderState(D3DRS_ZENABLE, FALSE);					// Z-Buffer ����.

	//���ø� �ɼ� ����.
	m_pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	//�ؽ��� ���͸�.
	m_pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
 
	//Clamp Address ����. : �ؽ��Ŀ� ���� ���� �ʿ�.
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	

	//���� ��Ʈ�� ����.
	//B3DSprite_SetStream();		 
	m_pDev->SetStreamSource(0, m_pVBSBox, 0, sizeof(SBOX_VTX));
	m_pDev->SetTransform(D3DTS_WORLD, &m_mTM); 
	m_pDev->SetFVF(FVF_SKYBOX);

	//6���� ���� �������Ѵ�.
	for(int i=SBF_FRONT_; i<SBF_MAX_; i++)
	{
		//�ؽ��� ����... 
  		m_pDev->SetTexture(0, m_Spr[i].pTexture);
 		
		//������... 
 		m_pDev->DrawPrimitive(D3DPT_TRIANGLELIST, i*6, 2 ); 
	}

 	//�ɼ� ����.
	m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//�ø� ����.
	m_pDev->SetRenderState(D3DRS_LIGHTING, TRUE); 
	m_pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);	//-> �ʿ�� �����ϵ���.. 
	m_pDev->SetRenderState(D3DRS_ZENABLE, TRUE);		// Z-Buffer ����.


	//���ø� �ɼ� ����.: ������ ������ ����.
	m_pDev->SetSamplerState(0, D3DSAMP_MINFILTER, B3D::g_B3TexFilterMin);
	m_pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, B3D::g_B3TexFilterMag);
	m_pDev->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, B3D::g_B3TexFilterMaxAnisotropy);

	//Clamp Address ����.
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
 
	return TRUE;
}







/****************** end of file "SkyBox.cpp" ********************************/