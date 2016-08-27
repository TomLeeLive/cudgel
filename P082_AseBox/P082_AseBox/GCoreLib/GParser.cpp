#include "GParser.h"

bool	GParser::ErrorCloseStream(TCHAR* pMsg)
{
	if (!m_pStream) return false;
	fclose(m_pStream);
	if (pMsg != NULL)
	{
		DEBUGMSG(pMsg);
	}
	return true;
}
BOOL	GParser::OpenStream(const TCHAR* pFileName)
{
	m_pStream = _tfopen(pFileName, _T("rt"));
	if (m_pStream == NULL)
	{
		return ErrorCloseStream(_T("파일을 찾지 못했습니다."));
	}
	return TRUE;
}

BOOL GParser::ComparisonData(TCHAR* pDestString, TCHAR* pSrcString)
{
	if (pSrcString == NULL)
	{
		pSrcString = m_pString;
	}
	if (!_tcsicmp(pSrcString, pDestString))
	{
		return TRUE;
	}
	return FALSE;
}
BOOL GParser::GetData(VOID* pData, INT DataType)
{
	if (pData != NULL)
	{
		switch (DataType)
		{
		case INT_DATA:
			_stscanf(m_pBuffer, _T("%s%d"), m_pString, pData);
			break;
		case FLOAT_DATA:
			_stscanf(m_pBuffer, _T("%s%f"), m_pString, pData);
			break;
		case STRING_DATA:
		{
			TCHAR szTexPath[MAX_PATH] = TEXT("");
			_stscanf(m_pBuffer, _T("%s%s"), m_pString, szTexPath);
			if (szTexPath[0] == '"')
			{
				// 문자열의 시작과 끝에 있는 (")문자를 삭제한다.
				// 또한 문자열중간에 공백까지도 반환문자열에 포함한다.
				_tcscpy(szTexPath, _tcsstr(m_pBuffer, _T("\"")));
				size_t filelen = _tcslen(szTexPath);
				szTexPath[filelen - 2] = 0;
				_tcscpy((TCHAR*)pData, &szTexPath[1]);
			}
			else
			{
				_tcscpy((TCHAR*)pData, &szTexPath[0]);
			}
		}
		break;
		case VERTEX_DATA:
		{
			_stscanf(m_pBuffer, _T("%s %f %f %f"),
				m_pString,
				&((D3DXVECTOR3*)pData)->x,
				&((D3DXVECTOR3*)pData)->z,
				&((D3DXVECTOR3*)pData)->y);
		}
		break;
		case FACE_DATA:
		case MATRIX_DATA:
		case NULL_DATA:
			break;
		}
	}
	return TRUE;
}
BOOL GParser::GetDataFromFileNext(TCHAR* pString, VOID* pData, INT DataType)
{
	//LPCSTR pBuffer = NULL;	

	if (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);
		if (!_tcsicmp(m_pString, pString))
		{
			return GetData(pData, DataType);
		}
	}

	return FALSE;
}

BOOL GParser::GetDataFromFile(TCHAR* pString, VOID* pData, INT DataType)
{

	fpos_t  FilePos;
	if (fgetpos(m_pStream, &FilePos))
	{
		ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
	}
	if (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);
		if (!_tcsicmp(m_pString, pString))
		{
			return GetData(pData, DataType);
		}
		else
		{
			if (fsetpos(m_pStream, &FilePos))
			{
				ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
			}
		}
	}

	return FALSE;
}
// 원하는 문자열을 찾고 찾은 문자열 시작위치로 옮긴다
BOOL GParser::GetDataFromFileString(TCHAR* pString, VOID* pData, INT DataType)
{
	fpos_t  FilePos;

	while (!feof(m_pStream))
	{
		if (fgetpos(m_pStream, &FilePos))
		{
			ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
		}
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);
		if (!_tcsicmp(m_pString, pString))
		{
			if (GetData(pData, DataType))
			{
				if (fsetpos(m_pStream, &FilePos))
				{
					ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}
BOOL GParser::GetNodeName(TCHAR* pString, VOID* pData, INT DataType)
{

	fpos_t  FilePos;
	if (fgetpos(m_pStream, &FilePos))
	{
		ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
	}
	if (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);
		if (!_tcsicmp(m_pString, pString))
		{
			if (pData != NULL)
			{
				switch (DataType)
				{
				case STRING_DATA:
				{
					TCHAR *pStr = NULL;
					TCHAR String1[20];
					TCHAR String2[20];
					TCHAR ResultString[40];
					memset(String1, 0, sizeof(TCHAR) * 20);
					memset(String2, 0, sizeof(TCHAR) * 20);
					memset(ResultString, 0, sizeof(TCHAR) * 40);
					bool check = false;
					int j = 0;
					if (_tcsstr(m_pBuffer, _T("Bip01 R UpperArm")))
					{
						int k;
						k = 0;
					}
					for (int i = 0; i<256; i++)
					{
						if (check)
						{
							if (m_pBuffer[i] == '"')
							{
								check = false;
								String2[j++] = 0;
								break;
							}
							String2[j++] = m_pBuffer[i];
						}
						else
						{
							if (m_pBuffer[i] == '"')
								check = true;
						}

					}
					//String2[filelen-1] = 0;					
					_tcscpy((TCHAR*)pData, &String2[0]);
				}
				break;
				}
			}
			return TRUE;
		}
		else
		{
			if (fsetpos(m_pStream, &FilePos))
			{
				ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
			}
		}
	}

	return FALSE;
}

BOOL GParser::GetDataFromFileLoop(TCHAR* pString, VOID* pData, INT DataType)
{
	LPCSTR pBuffer = NULL;//사용처가 없기 때문에 없어도 됨..
						  // 왜 있느냐고 묻는다면. 예전에 수업하던 것이라. 기억이 안남.. 따지지마...!!!
	while (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);
		if (!_tcsicmp(m_pString, pString))
		{
			return GetData(pData, DataType);
		}
	}
	return FALSE;//ErrorCloseStream( "전체파일에서 검색할수 없습니다.");	
}

//-----------------------------------------------------------------------------
// Name: BYTE helper functions
// Desc: cchDestChar is the size in BYTEs of strDest.  Be careful not to 
//       pass use sizeof() if the strDest is a string pointer.  
//       eg.
//       TCHAR* sz = new TCHAR[100]; // sizeof(sz)  == 4
//       TCHAR sz2[100];             // sizeof(sz2) == 200
//-----------------------------------------------------------------------------
HRESULT GParser::ConvertAnsiStringToWideCb(WCHAR* wstrDestination, const CHAR* strSource, int cbDestChar)
{
	return ConvertAnsiStringToWideCch(wstrDestination, strSource, cbDestChar / sizeof(WCHAR));
}

HRESULT GParser::ConvertWideStringToAnsiCb(CHAR* strDestination, const WCHAR* wstrSource, int cbDestChar)
{
	return ConvertWideStringToAnsiCch(strDestination, wstrSource, cbDestChar / sizeof(CHAR));
}

HRESULT GParser::ConvertGenericStringToAnsiCb(CHAR* strDestination, const TCHAR* tstrSource, int cbDestChar)
{
	return ConvertGenericStringToAnsiCch(strDestination, tstrSource, cbDestChar / sizeof(CHAR));
}

HRESULT GParser::ConvertGenericStringToWideCb(WCHAR* wstrDestination, const TCHAR* tstrSource, int cbDestChar)
{
	return ConvertGenericStringToWideCch(wstrDestination, tstrSource, cbDestChar / sizeof(WCHAR));
}

HRESULT GParser::ConvertAnsiStringToGenericCb(TCHAR* tstrDestination, const CHAR* strSource, int cbDestChar)
{
	return ConvertAnsiStringToGenericCch(tstrDestination, strSource, cbDestChar / sizeof(TCHAR));
}

HRESULT GParser::ConvertWideStringToGenericCb(TCHAR* tstrDestination, const WCHAR* wstrSource, int cbDestChar)
{
	return ConvertWideStringToGenericCch(tstrDestination, wstrSource, cbDestChar / sizeof(TCHAR));
}

HRESULT GParser::ConvertGUIDToStringCb(const GUID* pGuidSrc, TCHAR* strDest, int cbDestChar)
{
	return ConvertGUIDToStringCch(pGuidSrc, strDest, cbDestChar / sizeof(TCHAR));
}
HRESULT GParser::ConvertStringToGUID(const TCHAR* strSrc, GUID* pGuidDest)
{
	UINT aiTmp[10];

	if (_stscanf(strSrc, TEXT("{%8X-%4X-%4X-%2X%2X-%2X%2X%2X%2X%2X%2X}"),
		&pGuidDest->Data1,
		&aiTmp[0], &aiTmp[1],
		&aiTmp[2], &aiTmp[3],
		&aiTmp[4], &aiTmp[5],
		&aiTmp[6], &aiTmp[7],
		&aiTmp[8], &aiTmp[9]) != 11)
	{
		ZeroMemory(pGuidDest, sizeof(GUID));
		return E_FAIL;
	}
	else
	{
		pGuidDest->Data2 = (USHORT)aiTmp[0];
		pGuidDest->Data3 = (USHORT)aiTmp[1];
		pGuidDest->Data4[0] = (BYTE)aiTmp[2];
		pGuidDest->Data4[1] = (BYTE)aiTmp[3];
		pGuidDest->Data4[2] = (BYTE)aiTmp[4];
		pGuidDest->Data4[3] = (BYTE)aiTmp[5];
		pGuidDest->Data4[4] = (BYTE)aiTmp[6];
		pGuidDest->Data4[5] = (BYTE)aiTmp[7];
		pGuidDest->Data4[6] = (BYTE)aiTmp[8];
		pGuidDest->Data4[7] = (BYTE)aiTmp[9];
		return S_OK;
	}
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertGUIDToStringCch()
// Desc: Converts a GUID to a string 
//       cchDestChar is the size in TCHARs of strDest.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds
//-----------------------------------------------------------------------------
HRESULT GParser::ConvertGUIDToStringCch(const GUID* pGuidSrc, TCHAR* strDest, int cchDestChar)
{
	int nResult = _sntprintf(strDest, cchDestChar, TEXT("{%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X}"),
		pGuidSrc->Data1, pGuidSrc->Data2, pGuidSrc->Data3,
		pGuidSrc->Data4[0], pGuidSrc->Data4[1],
		pGuidSrc->Data4[2], pGuidSrc->Data4[3],
		pGuidSrc->Data4[4], pGuidSrc->Data4[5],
		pGuidSrc->Data4[6], pGuidSrc->Data4[7]);

	if (nResult < 0)
		return E_FAIL;
	return S_OK;
}

HRESULT GParser::ConvertWideStringToGenericCch(TCHAR* tstrDestination, const WCHAR* wstrSource,
	int cchDestChar)
{
	if (tstrDestination == NULL || wstrSource == NULL || cchDestChar < 1)
		return E_INVALIDARG;

#ifdef _UNICODE
	wcsncpy(tstrDestination, wstrSource, cchDestChar);
	tstrDestination[cchDestChar - 1] = L'\0';
	return S_OK;
#else
	return ConvertWideStringToAnsiCch(tstrDestination, wstrSource, cchDestChar);
#endif
}

HRESULT GParser::ConvertAnsiStringToGenericCch(TCHAR* tstrDestination, const CHAR* strSource,
	int cchDestChar)
{
	if (tstrDestination == NULL || strSource == NULL || cchDestChar < 1)
		return E_INVALIDARG;

#ifdef _UNICODE
	return ConvertAnsiStringToWideCch(tstrDestination, strSource, cchDestChar);
#else
	strncpy(tstrDestination, strSource, cchDestChar);
	tstrDestination[cchDestChar - 1] = '\0';
	return S_OK;
#endif    
}


//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertWideStringToAnsi()
// Desc: This is a UNICODE conversion utility to convert a WCHAR string into a
//       CHAR string. 
//       cchDestChar is the size in TCHARs of strDestination
//-----------------------------------------------------------------------------
HRESULT GParser::DXUtil_ConvertWideStringToAnsiCch(CHAR* strDestination, const WCHAR* wstrSource,
	int cchDestChar)
{
	if (strDestination == NULL || wstrSource == NULL || cchDestChar < 1)
		return E_INVALIDARG;

	int nResult = WideCharToMultiByte(CP_ACP, 0, wstrSource, -1, strDestination,
		cchDestChar*sizeof(CHAR), NULL, NULL);
	strDestination[cchDestChar - 1] = 0;

	if (nResult == 0)
		return E_FAIL;
	return S_OK;
}

HRESULT GParser::ConvertGenericStringToAnsiCch(CHAR* strDestination, const TCHAR* tstrSource,
	int cchDestChar)
{
	if (strDestination == NULL || tstrSource == NULL || cchDestChar < 1)
		return E_INVALIDARG;

#ifdef _UNICODE
	return DXUtil_ConvertWideStringToAnsiCch(strDestination, tstrSource, cchDestChar);
#else
	strncpy(strDestination, tstrSource, cchDestChar);
	strDestination[cchDestChar - 1] = '\0';
	return S_OK;
#endif   
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertGenericStringToWide()
// Desc: This is a UNICODE conversion utility to convert a TCHAR string into a
//       WCHAR string. 
//       cchDestChar is the size in TCHARs of wstrDestination.  Be careful not to 
//       pass in sizeof(strDest) 
//-----------------------------------------------------------------------------
HRESULT GParser::ConvertGenericStringToWideCch(WCHAR* wstrDestination, const TCHAR* tstrSource,
	int cchDestChar)
{
	if (wstrDestination == NULL || tstrSource == NULL || cchDestChar < 1)
		return E_INVALIDARG;

#ifdef _UNICODE
	wcsncpy(wstrDestination, tstrSource, cchDestChar);
	wstrDestination[cchDestChar - 1] = L'\0';
	return S_OK;
#else
	return ConvertAnsiStringToWideCch(wstrDestination, tstrSource, cchDestChar);
#endif    
}







HRESULT GParser::ConvertAnsiStringToWideCch(WCHAR* wstrDestination, const CHAR* strSource,
	int cchDestChar)
{
	if (wstrDestination == NULL || strSource == NULL || cchDestChar < 1)
		return E_INVALIDARG;

	int nResult = MultiByteToWideChar(CP_ACP, 0, strSource, -1,
		wstrDestination, cchDestChar);
	wstrDestination[cchDestChar - 1] = 0;

	if (nResult == 0)
		return E_FAIL;
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertWideStringToAnsi()
// Desc: This is a UNICODE conversion utility to convert a WCHAR string into a
//       CHAR string. 
//       cchDestChar is the size in TCHARs of strDestination
//-----------------------------------------------------------------------------
HRESULT GParser::ConvertWideStringToAnsiCch(CHAR* strDestination, const WCHAR* wstrSource,
	int cchDestChar)
{
	if (strDestination == NULL || wstrSource == NULL || cchDestChar < 1)
		return E_INVALIDARG;

	int nResult = WideCharToMultiByte(CP_ACP, 0, wstrSource, -1, strDestination,
		cchDestChar*sizeof(CHAR), NULL, NULL);
	strDestination[cchDestChar - 1] = 0;

	if (nResult == 0)
		return E_FAIL;
	return S_OK;
}

GParser::GParser(void)
{
}

GParser::~GParser(void)
{
}
