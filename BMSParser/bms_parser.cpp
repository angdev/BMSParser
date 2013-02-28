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

    std::wstringstream wss;
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

    //header 섹션이 시작되면 활성화
    bool isHeaderBegun = false;
    int headerIndex = -1;
    int invalidNumber = 0;

    wchar_t* currentPos = buffer;
    while(currentPos != NULL) {
        
        currentLine.clear();
        prefix.clear();
        wss.clear();

        currentPos = GetLineFromBuffer(buffer, bufLength, currentPos, currentLine);
        wss.str(currentLine);

		if(currentLine.length() != 0) {
			wss >> prefix;
        }
		else {
			continue;
        }

        UpperWideString(prefix);
        headerIndex = getHeaderIndex(prefix);
        if(headerIndex > -1) {
            isHeaderBegun = true;
        }
        
        //헤더 섹션을 벗어났다고 판단
        if(invalidNumber > 15) {
            break;
        }

        switch(headerIndex) {
        case kHeaderInvalid:
            if(isHeaderBegun) {
                ++invalidNumber;
            }
            break;
        case kHeaderPlayer:
            wss >> bmsInfo->player;
            break;
        case kHeaderTitle:
            bmsInfo->title = GetRemainingWideString(wss);
            break;
        case kHeaderArtist:
            bmsInfo->artist = GetRemainingWideString(wss);
            break;
        case kHeaderGenre:
            bmsInfo->genre = GetRemainingWideString(wss);
            break;
        case kHeaderBPM:
            wss >> bmsInfo->bpm;
            break;
        case kHeaderPlayLevel:
            wss >> bmsInfo->playlevel;
            break;
        case kHeaderRank:
            wss >> bmsInfo->rank;
            break;
        case kHeaderTotal:
            wss >> bmsInfo->total;
            break;
        case kHeaderVolWav:
            wss >> bmsInfo->volwav;
            break;
        case kHeaderStageFile:
            wss >> bmsInfo->stagefile;
            break;
        case kHeaderVideoFile:
            wss >> bmsInfo->videofile;
            break;
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

int BMSParser::getHeaderIndex(const std::wstring& str) {
    static const wchar_t *headerKey[] = {
        L"#PLAYER",
        L"#TITLE",
        L"#ARTIST",
        L"#GENRE",
        L"#BPM",
        L"#PLAYLEVEL",
        L"#RANK",
        L"#TOTAL",
        L"#VOLWAV",
        L"#STAGEFILE",
        L"#VIDEOFILE"
    };
    
    int arrLength = sizeof(headerKey) / sizeof(headerKey[0]);
    const wchar_t **end = headerKey + arrLength;
    const wchar_t **found = std::find_if(headerKey, end, [&](const wchar_t *s) {
        if(str.compare(s) == 0) {
            return true;
        }
        return false;
    });
    if(found != end) {
        return std::distance(headerKey, found);
    }
    return -1;
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