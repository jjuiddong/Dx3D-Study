
#ifndef __LIGHT_H__
#define __LIGHT_H__


class CLight
{
public:
	CLight();
	virtual ~CLight();

protected:
	D3DLIGHT9 m_light;
	Vector3 m_pos;
	Vector3 m_dir;
	Vector3 m_LookAt;
	int m_valueLight;
	float m_valuePower;

public:
	void LightOn();
	void LightOff();

	void SetPointLight(D3DXCOLOR c);
	void SetSpotLight(D3DXCOLOR c);
	void SetDirectionalLight( D3DXCOLOR c ); // color: D3DXCOLOR(r,g,b,a) format
	void SetDiffuse( DWORD color); // color: D3DXCOLOR(r,g,b,a) format
	void SetAmbient( DWORD color ); // color: D3DXCOLOR(r,g,b,a) format
	void SetSpecular( DWORD color ); // colo : D3DXCOLOR(r,g,b,a) format
	DWORD GetAmbient();
	DWORD GetDiffuse();

	void SetPosition( const Vector3& pos);
	void SetDirection( const Vector3& dir );
	void SetLookAt( const Vector3& lookAt );
	void SetRange( float range );

	const Vector3& GetPosition() { return m_pos; }
	const Vector3& GetDirection()  { return m_dir; }
	void GetViewMatrix( Matrix44 *pOut );
	void GetViewProjectMatrix( Matrix44 *pOut );

};

#endif
