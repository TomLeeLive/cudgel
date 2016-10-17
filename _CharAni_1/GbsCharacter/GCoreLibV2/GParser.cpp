#include "gparser.h"

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
		case MESH_FACE_DATA:
		{
			int iTemp; int iTemp2 = -1;
			//ST_INT_FOUR stTemp;
			//memset(&stTemp, 0, sizeof(stTemp));

			//%s %d %s %d %s %d %s %d %s %d
			_stscanf(m_pBuffer, _T("%s %s%s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d"),
				m_pString, m_pString, m_pString,
				&((ST_INT_FOUR*)pData)->index1, m_pString, &((ST_INT_FOUR*)pData)->index2, m_pString, &((ST_INT_FOUR*)pData)->index3,
				m_pString, &iTemp,
				m_pString, &iTemp,
				m_pString, &iTemp,
				m_pString, &iTemp,
				m_pString, &iTemp2
				);

			if (iTemp2 == -1) {
				_stscanf(m_pBuffer, _T("%s %s%s %d %s %d %s %d %s %d %s %d %s %d %s %s %d"),
					m_pString, m_pString, m_pString,
					&((ST_INT_FOUR*)pData)->index1, m_pString, &((ST_INT_FOUR*)pData)->index2, m_pString, &((ST_INT_FOUR*)pData)->index3,
					m_pString, &iTemp,
					m_pString, &iTemp,
					m_pString, &iTemp,
					m_pString, 
					m_pString, &iTemp2
					);

				((ST_INT_FOUR*)pData)->index4 = iTemp2;
			}
			else {
				((ST_INT_FOUR*)pData)->index4 = iTemp2;
			}
			
		}
		break;
		case MESH_VERTEX_DATA:
		{
			INT iData;
			_stscanf(m_pBuffer, _T("%s %d %f %f %f"),
				m_pString,
				&iData,
				&((D3DXVECTOR3*)pData)->x,
				&((D3DXVECTOR3*)pData)->z,
				&((D3DXVECTOR3*)pData)->y);
		}
		break;
		case ANI_SCL_DATA:
		{
			_stscanf(m_pBuffer, _T("%s %d %f %f %f %f %f %f %f"),
				m_pString,
				&((ST_ANI_SCL*)pData)->iTick,
				&((ST_ANI_SCL*)pData)->vecVec.x,
				&((ST_ANI_SCL*)pData)->vecVec.z,
				&((ST_ANI_SCL*)pData)->vecVec.y,
				&((ST_ANI_SCL*)pData)->vecRot.x,
				&((ST_ANI_SCL*)pData)->vecRot.z,
				&((ST_ANI_SCL*)pData)->vecRot.y,
				&((ST_ANI_SCL*)pData)->vecRot.w
				);
		}
		break;
		case ANI_ROT_DATA:
		{
			_stscanf(m_pBuffer, _T("%s %d %f %f %f %f"),
				m_pString,
				&((ST_ANI_ROT*)pData)->iTick,
				&((ST_ANI_ROT*)pData)->vecRot.x,
				&((ST_ANI_ROT*)pData)->vecRot.z,
				&((ST_ANI_ROT*)pData)->vecRot.y,
				&((ST_ANI_ROT*)pData)->vecRot.w
				);
		}
		break;
		case ANI_POS_DATA:
		{
			_stscanf(m_pBuffer, _T("%s %d %f %f %f"),
				m_pString,
				&((ST_ANI_POS*)pData)->iTick,
				&((ST_ANI_POS*)pData)->vecPos.x,
				&((ST_ANI_POS*)pData)->vecPos.z,
				&((ST_ANI_POS*)pData)->vecPos.y);
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
		{
			INT iData;
			_stscanf(m_pBuffer, _T("%s %d %d %d %d"),
				m_pString,
				&iData,
				&((ST_INT_THREE*)pData)->index1,
				&((ST_INT_THREE*)pData)->index2,
				&((ST_INT_THREE*)pData)->index3);
		}
		break;
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

GParser::GParser(void)
{
}

GParser::~GParser(void)
{
}
