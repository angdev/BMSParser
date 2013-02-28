// Ŭnicode please
/*
* Project RBMS : Rhythm game using Be Music Script
* This file is a part of project RBMS.
* Copyright (c) 2012, FeGs.
*/

#ifndef __BMS_OBJECT_H__
#define __BMS_OBJECT_H__
//BMS Parser Library

struct NoteInfo
{
	int NodeNumber;
	int ChannelNumber;
	int KeyNumber;
	std::wstring WavNumber;
	// 해당 마디에서의 위치를 나타내는 변수
	int Position;
	int TotalNumber;
	//ex) TotalNumber = 16, Position = 3이면 해당 마디에서 3/16 위치에 있는 것.
};

class NoteObject
{
public:
	NoteObject();
	NoteInfo Info;
private:

};
typedef std::vector<NoteObject> Notes;


//Temp. 컨테이너를 쓸건지 고민 중. 일단 파싱해서 정보를 다 가지고 있는걸로 하자. 이 아래는 아직 사용되지 않는다.
class NoteContainer
{
public:
	std::vector<NoteObject> notes;
};
typedef NoteContainer Channel;


struct ChannelContainer
{
	std::vector<Channel> channels;
};
typedef ChannelContainer Node;


struct NodeContainer
{
	std::vector<Node> nodes;
};


#endif