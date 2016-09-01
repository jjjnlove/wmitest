// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__93C24A6B_DAF3_4875_AF73_77A8677DA48B__INCLUDED_)
#define AFX_STDAFX_H__93C24A6B_DAF3_4875_AF73_77A8677DA48B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// TODO: reference additional headers your program requires here




#pragma once
#include <vector>
#include <map>
#include <string>


#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#define MAX_INSTANCE	32		//한번의 쿼리로 가져올 레코드 셋의 최대 크기
#define WMI_LOCALHOST	0
#define WMI_DEFAULTNAME	0
#define WMI_DEFAULTPW	0	


#pragma comment( lib, "wbemuuid.lib" )
#pragma comment( lib, "comsupp.lib" )

#include <comdef.h>
#include <wbemidl.h>



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__93C24A6B_DAF3_4875_AF73_77A8677DA48B__INCLUDED_)
