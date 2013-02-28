/*
* Project RBMS : Rhythm game using Be Music Script
* This file is a part of project RBMS.
* Copyright (c) 2012, FeGs.
*/

#ifndef __BMS_PARSER_H__
#define __BMS_PARSER_H__

typedef std::pair<std::wstring, std::wstring> WavPath;

struct NoteObject;
typedef std::vector<NoteObject> Notes;

#define _T(x) L##x

/**
BMS Header�� ���� ������ ��´�.
����ü�� ��� �������� ����� �����ڿ� ����.
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
public:

	/**
	\remarks
	Constructor
	*/
	BMSParser();

	/**
	\remarks
	BMS, BME ���� ���� ���� �Լ��̴�.
	\return
	���� ���⿡ �����ϸ� true, �׷��� ������ false�� ��ȯ�Ѵ�.
	*/
	bool OpenBMSFile(
		const std::string& filePath_	///<BMS ���� ���
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
	BMS ���� ������ �ִ� �Լ��̴�. BMSInfo ����ü ����.
	\return
	BMSInfo ������
	*/
	BMSInfo* GetpBMSInfo();

	/**
	\remarks
	BMS Ű�� ��ġ�� �ҷ����� �Լ��̴�. (������ �ҷ����� ���� �ƴϴ�!)
	Ŭ���� ������ ������ �ִ´�.
	\return
	����.
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
	/** BMS ���� �ڵ� */
	std::wifstream bmsFile_;
	std::string bmsFilePath_;
	//TODO : ������ ������ �ʿ䰡 ����.

	/** BMS Infomation */
	BMSInfo *bmsInfo_;
	std::map<std::wstring, std::wstring> *wavFilesPath_;
	Notes *noteContainer_;
	

	/** Command List */


	/** ó�� �Ǿ����� Ȯ�� */
	bool processed_;
	/** ������ ���ȴ��� Ȯ�� */
	bool fileOpened_;

};

namespace ParserHelper {
    /**
	Helper Function.
	\remarks
	wstringstream���� ���� �ڱⰡ ����Ű�� �κк��� ���� ���ڿ��� ��ȯ�ϴ� �Լ�.
	\return
	tellg()�κ��� str()�� ���� ���ڿ�.
	*/
	static std::wstring GetRemainingWstrFromSS(std::wstringstream& wss);
	static void SplitWString(const std::wstring &ws, const wchar_t* delim, std::vector<std::wstring> &elems);
    static std::string GetStringFromWString(std::wstring &ws);
    
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
    std::string���� �빮�ڷ� �ٲ��ش�
    */
    static void UpperWString(std::wstring &str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    }
};

#endif