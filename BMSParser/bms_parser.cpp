// Ŭnicode please
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
BMSParser::BMSParser() : processed_(false), fileOpened_(false), bmsInfo_(0), noteContainer_(0)
{

}

bool BMSParser::OpenBMSFile(const std::string& _filePath)
{
	if(processed_ == true)
	{
		//이전 자료 클리어해야 함.
		bmsInfo_->Clear();
		noteContainer_.clear();
		wavFilesPath_.clear();
	}

	try
	{
		bmsFile_.open(_filePath.c_str(), std::wifstream::in);
		bmsInfo_ = new BMSInfo();
	}
	catch(std::exception e)
	{
		//Fail to open file
		return false;
	}

	//새로 연 것으로 생각. (clear 함수 도입해야함.)
	fileOpened_ = true;
	processed_ = false;
	bmsFilePath_ = _filePath;

	return true;
}

BMSInfo* BMSParser::GetpBMSInfo()
{
	if(!processed_)
	{
		//TODO: 일단 parseHeader부터.
		parseHeader();
	}

	return bmsInfo_;
}

void BMSParser::parseHeader()
{
	if(fileOpened_ != true)
		return;


	std::wstring currentLine = L"";
	std::wstring prefix = L"";
	std::wstring param = L"";
	BMSInfo* bmsInfo = bmsInfo_;

    //get file length
    bmsFile_.seekg(0, std::ios::end);
    int bufLength = bmsFile_.tellg();
    
	bmsFile_.clear();
	bmsFile_.seekg(0, std::ios::beg);

    //file -> buffer
    //일단 한 번에 할당해보자.
    wchar_t *buffer = new wchar_t[bufLength];
    bmsFile_.read(buffer, bufLength);
    bmsFile_.close();

    wchar_t* currentPos = buffer;
    while(currentPos != NULL) {
        
        currentLine.clear();
        prefix.clear();
        
        currentPos = GetLineFromBuffer(buffer, bufLength, currentPos, currentLine);
        
        std::wstringstream lStringStream(currentLine);
		if(currentLine.length() != 0)
        {
			lStringStream >> prefix;
        }
		else
			continue;
		
		UpperWideString(prefix);

		//#PLAYER [1~4], 1:Player 1, 2:Player 1 + Player 2, 3: 1P DP, 4: ==2?
		if(prefix.compare(L"#PLAYER") == 0)
		{
			lStringStream >> param;
			bmsInfo->player = atoi(GetStringFromWideString(param).c_str());
		}
		//#TITLE [string]
		else if(prefix.compare(L"#TITLE") == 0)
		{
			bmsInfo->title = GetRemainingWideString(lStringStream);
		}
		//#ARTIST [string]
		else if(prefix.compare(L"#ARTIST") == 0)
		{
			bmsInfo->artist = GetRemainingWideString(lStringStream);
		}
		//#GENRE [string]
		else if(prefix.compare(L"#GENRE") == 0)
		{
			bmsInfo->genre = GetRemainingWideString(lStringStream);
		}
		//#BPM [float]
		else if(prefix.compare(L"#BPM") == 0)
		{
			lStringStream >> param;
			bmsInfo->bpm = (float)(atof(GetStringFromWideString(param).c_str()));
		}
		//#PLAYLEVEL [int], difficulty
		else if(prefix.compare(L"#PLAYLEVEL") == 0)
		{
			lStringStream >> param;
			bmsInfo->playlevel = atoi(GetStringFromWideString(param).c_str());
		}
		//#RANK [int]
		else if(prefix.compare(L"#RANK") == 0)
		{
			lStringStream >> param;
			bmsInfo->rank = atoi(GetStringFromWideString(param).c_str());
		}
		//#TOTAL [int], number of notes
		else if(prefix.compare(L"#TOTAL") == 0)
		{
			lStringStream >> param;
			bmsInfo->total = atoi(GetStringFromWideString(param).c_str());
		}
		//#VOLWAV [int], std 100, sound volume.
		else if(prefix.compare(L"#VOLWAV") == 0)
		{
			lStringStream >> param;
			bmsInfo->volwav = atoi(GetStringFromWideString(param).c_str());
		}
		//#STAGEFILE [string(filepath)], print out this file while loading.
 		else if(prefix.compare(L"#STAGEFILE") == 0)
		{
			bmsInfo->stagefile = GetRemainingWideString(lStringStream);
		}
		//#VIDEOFILE [string(filepath)], play this video while playing.
		else if(prefix.compare(L"#VIDEOFILE") == 0)
		{
			bmsInfo->videofile = GetRemainingWideString(lStringStream);
		}
        //헤더들은 붙어있다고 가정하고 10회 이상 출현하지 않으면 파싱 종료
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

		//dataPrefix에는 마디 정보가 들어있다.
		//이 라인은 데이터 섹션이다.
		std::vector<std::wstring> dataInfo;

		SplitWideString(lCurrentLine, L":", dataInfo);
		//dataInfo[0] :	마디, 채널 번호
		//dataInfo[1] : 마디 정보
		//std::wcout << dataInfo[0] << L" - " << dataInfo[1] << std::endl;
	
		//여기서부터 데이터를 담기 시작
		NoteInfo commonInfo;

		//성능 생각은 하지 않는다.
        std::wstring substrTemp = dataInfo[0].substr(1, 3);
		commonInfo.NodeNumber = atoi(GetStringFromWideString(substrTemp).c_str());
        
        substrTemp = dataInfo[0].substr(4, 1);
		commonInfo.ChannelNumber = atoi(GetStringFromWideString(substrTemp).c_str());
        
        substrTemp = dataInfo[0].substr(5, 1);
		commonInfo.KeyNumber = atoi(GetStringFromWideString(substrTemp).c_str());

		int nodeSize = dataInfo[1].size() / 2;
		commonInfo.TotalNumber = nodeSize;

		for(int i=0; i < nodeSize; i++)
		{
			NoteObject note;
			note.Info = commonInfo;
			note.Info.Position = i;
			note.Info.WavNumber = dataInfo[1].substr(i*2, 2);

			noteContainer_.push_back(note);
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
			wavFilesPath_.insert(WavPath(lPrefix.substr(4, 2), lFileName));
		}
	}

	for(std::map<std::wstring, std::wstring>::iterator iter = wavFilesPath_.begin(); iter != wavFilesPath_.end(); iter++)
	{
		std::wcout << iter->first << L" : " << iter->second << std::endl;
	}
}

void ParserHelper::SplitWideString(const std::wstring &ws, const wchar_t* delim, std::vector<std::wstring> &elems)
{
	std::wstringstream wss(ws);
	std::wstring item;
	while(std::getline(wss, item, *delim)){
		elems.push_back(item);
	}
}


std::wstring ParserHelper::GetRemainingWideString(std::wstringstream& wss)
{
	return wss.str().substr(wss.tellg());
}
    
std::string ParserHelper::GetStringFromWideString(std::wstring &ws)
{
    std::string str(ws.begin(), ws.end());
    str.assign(ws.begin(), ws.end());
    
    return str;
}

wchar_t *ParserHelper::GetLineFromBuffer(wchar_t *buffer, int length, wchar_t* begin, std::wstring &str) {
    wchar_t *end = buffer + length;
    //줄바꿈 문자 위치
    wchar_t *lfPos = std::find(begin, end, '\n');

    if(lfPos != end) {
        str.resize(lfPos - begin);
        std::copy(begin, lfPos, str.begin());
        return ++lfPos;
    }

    return NULL;
}