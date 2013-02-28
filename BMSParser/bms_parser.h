// Ŭnicode please
/*
* Project RBMS : Rhythm game using Be Music Script
* This file is a part of project RBMS.
* Copyright (c) 2012, FeGs.
*/

#ifndef __BMS_PARSER_H__
#define __BMS_PARSER_H__

#include "bms_object.h"

typedef std::pair<std::wstring, std::wstring> WavPath;

/**
BMS Header에 대한 정보를 담는다.
구조체의 멤버 변수명은 헤더의 지시자와 같다.
*/
struct BMSInfo
{
	int player;
	std::wstring title;
	std::wstring artist;
	std::wstring genre;
	float bpm;
	int playlevel;
	int rank;
	int total;
	int volwav;
	/* Deprecated 
	std::string _midifile;
	*/
	std::wstring stagefile;
	std::wstring videofile;
	/*
	enum LongNoteType
	{
	LNTYPE1 = 1,
	LNTYPE2,
	LNOBJ
	}
	*/

	void Clear();
};

class BMSParser
{
    enum HeaderKey {
        kHeaderInvalid = -1,
        kHeaderPlayer = 0,
        kHeaderTitle,
        kHeaderArtist,
        kHeaderGenre,
        kHeaderBPM,
        kHeaderPlayLevel,
        kHeaderRank,
        kHeaderTotal,
        kHeaderVolWav,
        kHeaderStageFile,
        kHeaderVideoFile
    };

public:

	/**
	\remarks
	Constructor
	*/
	BMSParser();

	/**
	\remarks
	BMS, BME 파일 등을 여는 함수이다.
	\return
	파일 열기에 성공하면 true, 그렇지 않으면 false를 반환한다.
	*/
	bool OpenBMSFile(
		const std::string& filePath_	///<BMS 파일 경로
		);

	std::string GetCurrentFilePath() const
	{
		if(fileOpened_)
		{
			return bmsFilePath_;
		}

		return "";
	}

	/**
	\remarks
	BMS 파일 정보를 주는 함수이다. BMSInfo 구조체 참조.
	\return
	BMSInfo 포인터
	*/
	BMSInfo* GetpBMSInfo();

	/**
	\remarks
	BMS 키음 위치를 불러오는 함수이다. (파일을 불러오는 것이 아니다!)
	클래스 변수가 가지고 있는다.
	\return
	없음.
	void LoadKeySound();
    */

public:
	void TestDataParse()
	{
		loadWavFilesPath();
	}

private:
	void parseHeader();
	void parseDataSection();
	void loadWavFilesPath();

private:
    int getHeaderIndex(const std::wstring& str);

private:
	/** BMS 파일 핸들 */
	std::wifstream bmsFile_;
	std::string bmsFilePath_;
	//TODO : 파일줄 정렬할 필요가 있음.

	/** BMS Infomation */
	BMSInfo *bmsInfo_;
	std::map<std::wstring, std::wstring> wavFilesPath_;
	Notes noteContainer_;
	

	/** Command List */


	/** 처리 되었는지 확인 */
	bool processed_;
	/** 파일이 열렸는지 확인 */
	bool fileOpened_;

};


namespace ParserHelper {
    /**
	Helper Function.
	\remarks
	wstringstream에서 현재 자기가 가리키는 부분부터 남은 문자열을 반환하는 함수.
	\return
	tellg()로부터 str()의 끝의 문자열.
	*/
	inline static std::wstring GetRemainingWideString(std::wstringstream& wss);
	inline static void SplitWideString(const std::wstring &ws, const wchar_t* delim, std::vector<std::wstring> &elems);
    inline static std::string GetStringFromWideString(std::wstring &ws);
    
	template<class FROM, class TO>
	static bool NumberStringConvert(const FROM& from, TO& toOut)
	{
		std::wstringstream wss;
		wss << from;
		wss >> toOut;

		if(wss.fail() || wss.bad() || !wss.eof())
		{
			return false;
		}

		return true;
	}

    static wchar_t *GetLineFromBuffer(wchar_t *buffer, int length, wchar_t* begin, std::wstring &str);

    /** 
    std::string형을 대문자로 바꿔준다
    */
    inline static void UpperWideString(std::wstring &str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    }
};

#endif