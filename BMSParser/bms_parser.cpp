/*
* Project RBMS : Rhythm game using Be Music Script
* This file is a part of project RBMS.
* Copyright (c) 2012, FeGs.
*/

#include "stdafx.h"
#include "bms_object.h"
#include "bms_parser.h"

using namespace ParserHelper;

/* BMSInfo Functions */
void BMSInfo::Clear()
{
	player = 0; title = L""; artist = L""; genre = L""; bpm = 0; playlevel = 0;
	rank = 0; total = 0; volwav = 0; stagefile = L""; videofile = L"";
}



/* BMSParser Class Functions */
BMSParser::BMSParser() : processed_(false), fileOpened_(false), bmsInfo_(0), noteContainer_(0), wavFilesPath_(0)
{

}

bool BMSParser::OpenBMSFile(const std::string& _filePath)
{
	if(processed_ == true)
	{
		//���� �ڷ� Ŭ�����ؾ� ��.
		bmsInfo_->Clear();
		
		if(noteContainer_ != 0)
			noteContainer_->clear();

		if(wavFilesPath_ != 0)
			wavFilesPath_->clear();
	}

	try
	{
		bmsFile_.open(_filePath.c_str(), std::wifstream::in);
		bmsInfo_ = new BMSInfo();
		noteContainer_ = new Notes();
		wavFilesPath_ = new std::map<std::wstring, std::wstring>();
	}
	catch(std::exception e)
	{
		//Fail to open file
		return false;
	}

	//���� �� ������ ����. (clear �Լ� �����ؾ���.)
	fileOpened_ = true;
	processed_ = false;
	bmsFilePath_ = _filePath;

	return true;
}

BMSInfo* BMSParser::GetpBMSInfo()
{
	if(!processed_)
	{
		//TODO: �ϴ� parseHeader����.
		parseHeader();
	}

	return bmsInfo_;
}

void BMSParser::parseHeader()
{
	if(fileOpened_ != true)
		return;

	std::wstring lCurrentLine = L"";
	std::wstring lPrefix = L"";
	std::wstring lParam = L"";
	BMSInfo* lBmsInfo = bmsInfo_;

    //get file length
    bmsFile_.seekg(0, std::ios::end);
    int bufLength = bmsFile_.tellg();
    
	bmsFile_.clear();
	bmsFile_.seekg(0, std::ios::beg);

    //file -> buffer
    //�ϴ� �� ���� �Ҵ��غ���.
    wchar_t *buffer = new wchar_t[bufLength];
    bmsFile_.read(buffer, bufLength);
    bmsFile_.close();

    wchar_t* currentPos = buffer;
    while(currentPos != NULL) {
        
        lCurrentLine.clear();
        lPrefix.clear();
        
        currentPos = GetLineFromBuffer(buffer, bufLength, currentPos, lCurrentLine);
        
        std::wstringstream lStringStream(lCurrentLine);
		if(lCurrentLine.length() != 0)
        {
			lStringStream >> lPrefix;
        }
		else
			continue;
		
		UpperWString(lPrefix);

		//#PLAYER [1~4], 1:Player 1, 2:Player 1 + Player 2, 3: 1P DP, 4: ==2?
		if(lPrefix.compare(L"#PLAYER") == 0)
		{
			lStringStream >> lParam;
			lBmsInfo->player = atoi(GetStringFromWString(lParam).c_str());
		}
		//#TITLE [string]
		else if(lPrefix.compare(L"#TITLE") == 0)
		{
			lBmsInfo->title = GetRemainingWstrFromSS(lStringStream);
		}
		//#ARTIST [string]
		else if(lPrefix.compare(L"#ARTIST") == 0)
		{
			lBmsInfo->artist = GetRemainingWstrFromSS(lStringStream);
		}
		//#GENRE [string]
		else if(lPrefix.compare(L"#GENRE") == 0)
		{
			lBmsInfo->genre = GetRemainingWstrFromSS(lStringStream);
		}
		//#BPM [float]
		else if(lPrefix.compare(L"#BPM") == 0)
		{
			lStringStream >> lParam;
			lBmsInfo->bpm = (float)(atof(GetStringFromWString(lParam).c_str()));
		}
		//#PLAYLEVEL [int], difficulty
		else if(lPrefix.compare(L"#PLAYLEVEL") == 0)
		{
			lStringStream >> lParam;
			lBmsInfo->playlevel = atoi(GetStringFromWString(lParam).c_str());
		}
		//#RANK [int]
		else if(lPrefix.compare(L"#RANK") == 0)
		{
			lStringStream >> lParam;
			lBmsInfo->rank = atoi(GetStringFromWString(lParam).c_str());
		}
		//#TOTAL [int], number of notes
		else if(lPrefix.compare(L"#TOTAL") == 0)
		{
			lStringStream >> lParam;
			lBmsInfo->total = atoi(GetStringFromWString(lParam).c_str());
		}
		//#VOLWAV [int], std 100, sound volume.
		else if(lPrefix.compare(L"#VOLWAV") == 0)
		{
			lStringStream >> lParam;
			lBmsInfo->volwav = atoi(GetStringFromWString(lParam).c_str());
		}
		//#STAGEFILE [string(filepath)], print out this file while loading.
 		else if(lPrefix.compare(L"#STAGEFILE") == 0)
		{
			lBmsInfo->stagefile = GetRemainingWstrFromSS(lStringStream);
		}
		//#VIDEOFILE [string(filepath)], play this video while playing.
		else if(lPrefix.compare(L"#VIDEOFILE") == 0)
		{
			lBmsInfo->videofile = GetRemainingWstrFromSS(lStringStream);
		}
        //������� �پ��ִٰ� �����ϰ� 10ȸ �̻� �������� ������ �Ľ� ����
        else {

        }
	}
	
	processed_ = true;
	//bmsInfo_ = lBmsInfo;
}

void BMSParser::parseDataSection()
{
	if(!fileOpened_)
		return;

	std::wstring lCurrentLine = L"";
	//std::wstringstream lStringStream;

	bmsFile_.clear();
	bmsFile_.seekg(0, std::ios::beg);

	while(!bmsFile_.eof())
	{
		std::getline(bmsFile_, lCurrentLine);
		//lStringStream = std::wstringstream(lCurrentLine);

		if(lCurrentLine.length() == 0)
			continue;

		int	outDataPrefix = 0;
		std::wstring dataPrefixString = lCurrentLine.substr(1, 5);
		
		if(!NumberStringConvert<std::wstring, int>(dataPrefixString, outDataPrefix))
		{
			continue;
		}

		//dataPrefix���� ���� ������ ����ִ�.
		//�� ������ ������ �����̴�.
		std::vector<std::wstring> dataInfo;

		SplitWString(lCurrentLine, L":", dataInfo);
		//dataInfo[0] :	����, ä�� ��ȣ
		//dataInfo[1] : ���� ����
		//std::wcout << dataInfo[0] << L" - " << dataInfo[1] << std::endl;
	
		//���⼭���� �����͸� ��� ����
		NoteInfo commonInfo;

		//���� ������ ���� �ʴ´�.
        std::wstring substrTemp = dataInfo[0].substr(1, 3);
		commonInfo.NodeNumber = atoi(GetStringFromWString(substrTemp).c_str());
        
        substrTemp = dataInfo[0].substr(4, 1);
		commonInfo.ChannelNumber = atoi(GetStringFromWString(substrTemp).c_str());
        
        substrTemp = dataInfo[0].substr(5, 1);
		commonInfo.KeyNumber = atoi(GetStringFromWString(substrTemp).c_str());

		int nodeSize = dataInfo[1].size() / 2;
		commonInfo.TotalNumber = nodeSize;

		for(int i=0; i < nodeSize; i++)
		{
			NoteObject note;
			note.Info = commonInfo;
			note.Info.Position = i;
			note.Info.WavNumber = dataInfo[1].substr(i*2, 2);

			noteContainer_->push_back(note);
		}
		
	}
	/*
	for(std::vector<NoteObject>::iterator iter = noteContainer_->begin();
		iter != noteContainer_->end(); iter++)
	{
		std::wcout << _T("Note : ") << (*iter).Info.NodeNumber << _T(" ") << (float)(iter->Info.Position)/(float)(iter->Info.TotalNumber) << _T(" ") << iter->Info.WavNumber << std::endl;
	}
	*/
}

void BMSParser::loadWavFilesPath()
{
	std::wstring lCurrentLine = L"";
	std::wstring lPrefix;
	
	bmsFile_.clear();
	bmsFile_.seekg(0, std::ios::beg);

	while(!bmsFile_.eof())
	{
		std::getline(bmsFile_, lCurrentLine);
        std::wstringstream lWStringStream(lCurrentLine);

		std::wstring lFileName = L"";

		lWStringStream >> lPrefix;
		lWStringStream >> lFileName;

		if(lPrefix.substr(0, 4) == L"#WAV")
		{
			wavFilesPath_->insert(WavPath(lPrefix.substr(4, 2), lFileName));
		}
	}

	for(std::map<std::wstring, std::wstring>::iterator iter = wavFilesPath_->begin(); iter != wavFilesPath_->end(); iter++)
	{
		std::wcout << iter->first << L" : " << iter->second << std::endl;
	}
}

void ParserHelper::SplitWString(const std::wstring &ws, const wchar_t* delim, std::vector<std::wstring> &elems)
{
	std::wstringstream wss(ws);
	std::wstring item;
	while(std::getline(wss, item, *delim)){
		elems.push_back(item);
	}
}


std::wstring ParserHelper::GetRemainingWstrFromSS(std::wstringstream& wss)
{
	return wss.str().substr(wss.tellg());
}
    
std::string ParserHelper::GetStringFromWString(std::wstring &ws)
{
    std::string str(ws.begin(), ws.end());
    str.assign(ws.begin(), ws.end());
    
    return str;
}

wchar_t *ParserHelper::GetLineFromBuffer(wchar_t *buffer, int length, wchar_t* begin, std::wstring &str) {
    wchar_t *end = buffer + length;
    //�ٹٲ� ���� ��ġ
    wchar_t *lfPos = std::find(begin, end, '\n');

    if(lfPos != end) {
        str.resize(lfPos - begin);
        std::copy(begin, lfPos, str.begin());
        return ++lfPos;
    }

    return NULL;
}