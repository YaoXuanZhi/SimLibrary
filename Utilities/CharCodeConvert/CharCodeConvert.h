/*****************************************************************************
*  @file     CharCodeConvert.h
*  @brief    �ַ�������ת��
*  @note
*  ͨ��MultiByteToWideChar()��WideCharToMultiByte()��Windows API
*  ��ʵ��UIF16��MBCS��UTF8�ַ���������໥ת����
*  ��չ���ӣ�
*  �ļ������⣺https://github.com/BYVoid/uchardet
*  �ļ�����ת����http://www.gnu.org/software/libiconv/
*****************************************************************************/
#ifndef __CHARCODECONVERT_H
#define __CHARCODECONVERT_H
#include <string>

namespace CharCodeConvert {

/////////////////////////ת���ַ�����/////////////////////////
/** 
* @brief: Mbcs����תΪUtf8����
* @param[in] const char*  szMbcs  ��ת����Mbcs�ַ������ַ���
* @param[in] int  mbcsLen  ��ת�����ַ����ĳ���
* @return  ����ת������ַ���
* @note   
* 
*/
std::string MBCSToUTF8(const char* szMbcs, int mbcsLen);

/** 
* @brief: Mbcs����תΪUtf16����
* @param[in] const char*  szMbcs  ��ת����Mbcs�ַ������ַ���
* @param[in] int  mbcsLen  ��ת�����ַ����ĳ���
* @return  ����ת������ַ���
* @note   
*/
std::wstring MBCSToUTF16(const char* szMbcs, int mbcsLen);

/** 
* @brief: Utf8����תΪMbcs����
* @param[in] const char*  szUtf8  ��ת����szUtf8�ַ������ַ���
* @param[in] int  utf8Len  ��ת�����ַ����ĳ���
* @return  ����ת������ַ���
* @note   
*/
std::string UTF8ToMBCS(const char* szUtf8, int utf8Len);

/** 
* @brief: Utf8����תΪUtf16����
* @param[in] const char*  szUtf8  ��ת����Utf8�ַ������ַ���
* @param[in] int  utf8Len  ��ת�����ַ����ĳ���
* @return  ����ת������ַ���
* @note   
*/
std::wstring UTF8ToUTF16(const char* szUtf8, int utf8Len);

/** 
* @brief: Utf16����תΪMbcs����
* @param[in] const wchar_t*  szUtf16  ��ת����Utf16�ַ������ַ���
* @param[in] int  utf16Len  ��ת�����ַ����ĳ���
* @return  ����ת������ַ���
* @note 
*/
std::string UTF16ToMBCS(const wchar_t* szUtf16, int utf16Len);

/** 
* @brief: Utf16����תΪUtf8����
* @param[in] const wchar_t*  szUtf16  ��ת����Utf16�ַ������ַ���
* @param[in] int  utf16Len  ��ת�����ַ����ĳ���
* @return  ����ת������ַ���
* @note 
*/
std::string UTF16ToUTF8(const wchar_t* szUtf16, int utf16Len);

/** 
* @brief: Mbcs����תΪUtf8����
* @param[in] std::string  szMbcs  ��ת����Mbcs�ַ������ַ���
* @return  ����ת������ַ���
* @note 
*/
std::string MBCSToUTF8(std::string szMbcs);

/** 
* @brief: Mbcs����תΪUtf16����
* @param[in] std::string  szMbcs  ��ת����Mbcs�ַ������ַ���
* @return  ����ת������ַ���
* @note 
*/
std::wstring MBCSToUTF16(std::string szMbcs);

/** 
* @brief: Utf8����תΪMbcs����
* @param[in] std::string  szUtf8  ��ת����Utf8�ַ������ַ���
* @return  ����ת������ַ���
* @note 
*/
std::string UTF8ToMBCS(std::string szUtf8);

/** 
* @brief: UTF8����תΪUtf16����
* @param[in] std::string  szUtf8  ��ת����Utf8�ַ������ַ���
* @return  ����ת������ַ���
* @note 
*/
std::wstring UTF8ToUTF16(std::string szUtf8);

/** @brief: Utf16����תΪMbcs����
* @param[in] std::string  szUtf16  ��ת����Utf16�ַ������ַ���
* @return  ����ת������ַ���
* @note 
*/
std::string UTF16ToMBCS(std::wstring szUtf16);

/** 
* @brief: Utf16����תΪUTF8
* @param[in] std::string  szUtf16  ��ת����Utf16�ַ������ַ���
* @return  ����ת������ַ���
* @note 
*/
std::string UTF16ToUTF8(std::wstring szUtf16);

/////////////////////////����ַ�����/////////////////////////
/** 
* @brief: �ж��ַ������Ƿ����utf8�����ַ���
*		  �Դ��жϴ��ַ����Ƿ�Ϊutf8����
* @param[in] const  char*  �������ַ���
* @return  �������Ϊtrue������ַ�������utf8���룬
*		   ��������gbk���루Ҳ����mbcs��
* @note 
* �˺�����ȡ��licecap��Ŀ
*/
bool HasUTF8(const char *szSource);

}
#endif

/*********************************������ʾ************************************
void main()
{
	std::string szSrc1 = "��ã����磡";
	std::string szDst_Utf8 = CharCodeConvert::MBCSToUTF8(szSrc1);
	std::wstring szDst_Utf16 = CharCodeConvert::MBCSToUTF16(szSrc1);

	std::wstring szSrc2 = L"��ã����磡";
	std::string szDst_Mbcs = CharCodeConvert::UTF16ToMBCS(szSrc2);
	std::string szDst_Utf82 = CharCodeConvert::UTF16ToUTF8(szSrc2);

	if (szDst_Utf8 == szDst_Utf82)
	{
		OutputDebugStringA("Utf8ƥ��\n");
	}

	if (szSrc1 == szDst_Mbcs)
	{
		OutputDebugStringA("Mbcsƥ��\n");
	}

	if (szDst_Utf16 == szSrc2)
	{
		OutputDebugStringA("Utf16ƥ��\n");
	}
}
*****************************************************************************/