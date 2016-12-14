#pragma once
#include "stdafx.h"
#include "blakcjack.h"


class player {
	string name;
	DWORD money;
public:
	void setMoney(DWORD money);
	DWORD getMoney() const;
	string getName() const;
	void setName(string name);
	void appendByte(BYTE ch);
	void initiate();
};

struct compPlayer {
	bool operator() (const player &a, const player &b) {
		return a.getName() < b.getName();
	}
};

class game {
	set<player, compPlayer> mPlayers;

	int readPlayer(BYTE* buffer, int size, int &poz, player &mPlayer); 
public:
	void readPlayers(BYTE *buffer, int size);
	game();
};

game::game() {
	HANDLE hFile;
	BYTE *buffer;
	int size;

	hFile = getHandleForRead(".\\stats.txt");

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("eroare la deschiderea fisierului - %d", GetLastError());
		return;
	}

	buffer = read(hFile, 0);
	if (buffer == NULL) {
		printf("eroare citire - %d", GetLastError());
		return;
	}
	size = GetFileSize(hFile, 0);
	CloseHandle(hFile);

	readPlayers(buffer, size);

}
int game::readPlayer(BYTE *buffer, int size, int &poz, player &mPlayer) {
	int playerStSz = 0;

	mPlayer.initiate();
	//ajung pana la inceputul numelui
	while (poz < size && buffer[poz] != '#') {
		poz++;
	}
	if (poz == size)
		return 0;
	poz++;
	//ajung pana la sfarsitul numelui
	while (poz < size && buffer[poz] != '#') {
		mPlayer.appendByte(buffer[poz]);
		poz++;
	}
	if (poz == size)
		return 0;
	poz++;
	mPlayer.appendByte(0);
	//ajung pana la suma de bani
	while (poz < size && buffer[poz] != '#')
		poz++;
	if (poz >= size - 5)
		return 0;
	poz++;

	mPlayer.setMoney(*(DWORD*)(buffer + poz));
	poz += 5;
	return 1;
	
}
void game::readPlayers(BYTE *buffer, int size) {
	int poz = 0;
	player mPlayer;
	while (readPlayer(buffer, size, poz, mPlayer)) {
		mPlayers.insert(mPlayer);
	}
	printf("number of players : %d\n", mPlayers.size());
	set<player, compPlayer>::iterator it;

	for (it = mPlayers.begin(); it != mPlayers.end(); ++it)
		printf("%s - %X\n", it->getName().c_str(), it->getMoney());
}


void player::setMoney(DWORD money) {
	this->money = money;
}

DWORD player::getMoney() const{
	return this->money;
}

string player::getName() const {
	return this->name;
}

void player::setName(string name) {
	this->name = name;
}

void player::appendByte(BYTE ch) {
	this->name += ch;
}

void player::initiate() {
	this->money = 0;
	this->name.erase();
}
