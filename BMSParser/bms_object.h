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
	// �ش� ���𿡼��� ��ġ�� ��Ÿ���� ����
	int Position;
	int TotalNumber;
	//ex) TotalNumber = 16, Position = 3�̸� �ش� ���𿡼� 3/16 ��ġ�� �ִ� ��.
};

class NoteObject
{
public:
	NoteObject();
	NoteInfo Info;
private:

};
typedef std::vector<NoteObject> Notes;


//Temp. �����̳ʸ� ������ ��� ��. �ϴ� �Ľ��ؼ� ������ �� ������ �ִ°ɷ� ����. �� �Ʒ��� ���� ������ �ʴ´�.
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