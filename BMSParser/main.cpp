// Ŭnicode please
/*
 * Project RBMS : Rhythm game using Be Music Script
 * This file is a part of project RBMS.
 * Copyright (c) 2012, FeGs.
 */

#include "stdafx.h"
#include <iostream>
#include "bms_parser.h"

int main(void)
{
	BMSParser bp;
	BMSInfo* pBmsInfo;

	bp.OpenBMSFile("raw.txt");
	pBmsInfo = bp.GetpBMSInfo();

    
	std::wcout << L"title : " << pBmsInfo->title << std::endl;
	std::wcout << L"bpm : " << pBmsInfo->bpm << std::endl;
	std::wcout << L"genre : " << pBmsInfo->genre << std::endl;
	std::wcout << L"playlevel : " << pBmsInfo->playlevel << std::endl;
	std::wcout << L"total : " << pBmsInfo->total << std::endl;
    
    
	//bp.TestDataParse();
    
    //std::cout << "!";
    getchar();

	return 0;
}