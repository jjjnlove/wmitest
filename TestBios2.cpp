// TestBios2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "WQL.h"

#include <iostream>
using namespace std;


int main(int argc, char* argv[])
{
	CWQL myWQL;
	
	if(!myWQL.connect(WMI_LOCALHOST, WMI_DEFAULTNAME, WMI_DEFAULTPW))
	{
		//Connection failed!
		return -1;
	}

	CWQL::RowSet rs;
	if(!myWQL.getClassProperties(  L"Win32_Bios", rs ))
	{
		//WQL Execute failed!
		return -1;
	}

	char szBiosSN[32 + 1];
	ZeroMemory(szBiosSN, sizeof(szBiosSN));
	sprintf(szBiosSN,"%ls", (rs[0][L"SerialNumber"]).c_str());
	printf("[BIOS SerialNumber] : %s\n", szBiosSN);

	if(!myWQL.getClassProperties(  L"Win32_BaseBoard", rs ))
	{
		//WQL Execute failed!
		return -1;
	}
	
//	char szBiosSN[32 + 1];
	ZeroMemory(szBiosSN, sizeof(szBiosSN));
	sprintf(szBiosSN,"%ls", (rs[0][L"SerialNumber"]).c_str());
	printf("[Win32_BaseBoard] : %s\n", szBiosSN);
	return 0;
}
