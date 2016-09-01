// WQL.cpp: implementation of the CWQL class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "WQL.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWQL::CWQL() : m_pWmiServices(NULL), m_pWmiLocator(NULL), m_bIsInit(false) 
{

}
	
CWQL::~CWQL()
{
	close();
}

bool CWQL::connect(const wchar_t *szHost, const wchar_t* szName, const wchar_t* szPwd)
{
	close();

	HRESULT            result;

	result = CoInitialize( NULL );
	if( FAILED( result ) ) 
	{
		return false;
	}

	// 보안 컨텍스트를 초기화 (setup process-wide security context)
	result = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL ); 
	if( FAILED( result ) ) {
		CoUninitialize();
		return false;
	}

	// connect to WMI
	result = CoCreateInstance( CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, ( LPVOID * )&m_pWmiLocator );
	if( FAILED( result ) ) {
		CoUninitialize();
		return false;
	}

	if(  szHost == WMI_LOCALHOST || szName == WMI_DEFAULTNAME || szPwd == WMI_DEFAULTPW )
	{
		result = m_pWmiLocator->ConnectServer( _bstr_t( L"ROOT\\CIMV2" ), NULL, NULL, 0, NULL, 0, 0, &m_pWmiServices );
		if( FAILED( result ) ) {
			CoUninitialize();
			return false;
		}
	}
	else
	{
		wchar_t buffer[100];
		swprintf( buffer, L"\\\\%s\\ROOT\\CIMV2", szHost ); 
		result = m_pWmiLocator->ConnectServer( _bstr_t( buffer ), _bstr_t(szName), _bstr_t(szPwd), 0, NULL, 0, 0, &m_pWmiServices );
		if( FAILED( result ) ) {
			CoUninitialize();
			return false;
		}
	}

	result = CoSetProxyBlanket( m_pWmiServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );
	if( FAILED( result ) ) {
		CoUninitialize();
		return false;
	}

	m_bIsInit = true;

	return true;
}

void CWQL::close() 
{
	if( !m_bIsInit )
		return ;

	if( m_pWmiServices ) {
		m_pWmiServices->Release();
		m_pWmiServices = NULL;
	}

	if( m_pWmiLocator ) {
        m_pWmiLocator->Release();
		m_pWmiLocator = NULL;
	}
	// closes the OLE Component Object Model (COM) library
	CoUninitialize();
}

bool CWQL::runWql( const wchar_t* query, CWQL::RowSet& rs )
{
	if( !m_bIsInit ) {
		return false;
	}
	rs.clear();
	HRESULT                 ret;
	IEnumWbemClassObject *  pEnumerator = NULL;
	// 쿼리를 실행한다. ( execute a query )
	ret = m_pWmiServices->ExecQuery( L"WQL", _bstr_t(query), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator );
	if( FAILED( ret ) ) {
		return false;
	}
	if( pEnumerator )
	{
		IWbemClassObject *      clsObj[MAX_INSTANCE];
		ULONG                   uReturn = 0;

		ret = pEnumerator->Next( WBEM_INFINITE, MAX_INSTANCE, clsObj, &uReturn );
		if( uReturn > 0 ) 
		{
			rs.resize( uReturn );
			for( ULONG i=0;i<uReturn;++i )
			{
				// classobj의 모든 Properties를 enumerating 해서 값을 Map으로 세팅
				CWQL::Row& r = rs[i];
				VARIANT vtProp;
				VariantInit( &vtProp );
				
				ret = clsObj[i]->BeginEnumeration( WBEM_FLAG_NONSYSTEM_ONLY );
				if( FAILED( ret ) )
				{
					clsObj[i]->Release();
					continue;
				}
				CIMTYPE	pvtType; VARIANT pVal; BSTR pstrName;
				// 각각의 Property들을 Map에 할당
				while( clsObj[i]->Next( 0, &pstrName, &pVal, &pvtType, NULL ) == WBEM_S_NO_ERROR ) 
				{
					// 백터&배열 지원 안함
					if( (pVal.vt & 0xf000) == 0 )
					{
						if( pVal.vt != VT_NULL ) 
						{
							_bstr_t str = pVal;
							r[ pstrName ] = str;
						}
						else
						{
							r[ pstrName ] = L"";
						}
					}
					else
					{
						r[ pstrName ] = L"";
					}
					SysFreeString( pstrName );
					VariantClear( &pVal );
				}
				clsObj[i]->Release();
			}
		}
		pEnumerator->Release();
		if( uReturn == 0 )
			return false;
	}        
	return true;
}

bool CWQL::getClassProperties( const wchar_t * wmiClass, CWQL::RowSet& rs )
{
	std::wstring wquery;

	wquery += L"SELECT * FROM ";
	wquery += wmiClass;

	return runWql( wquery.c_str(), rs );
}
