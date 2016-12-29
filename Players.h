#pragma once
#include "stdafx.h"
#include "blakcjack.h"

#if defined(_WIN32) || defined(__MSDOS__)
#define SPADE   "\x06"
#define CLUB    "\x05"
#define HEART   "\x03"
#define DIAMOND "\x04"
#else
#define SPADE   "\xE2\x99\xA0"
#define CLUB    "\xE2\x99\xA3"
#define HEART   "\xE2\x99\xA5"
#define DIAMOND "\xE2\x99\xA6"
#endif

#define shCards 1
#define shPoints 2
#define shBoth 3
#define HIT 1
#define STAND 0
#define DOUBLEDOWN 2
#define SPLIT 3
#define SHOWHAND 4

struct card {
	string face;
	int value;
	void set(int value);
	void print() const;
};

class game;
class player {
	string name;
	DWORD money;
	DWORD points[4];
	vector<vector<card> > cards;
	string sShowHand = "ARATA CARTILE", sStand = "STAND", sHit = "HIT", sDoubleDown = "DOUBLE DOWN", sSplit = "SPLIT";

public:
	DWORD bet[4];

	void setMoney(DWORD money);
	void resetPoints() { for(int i = 0; i < 4; i++) points[i] = 0; }
	DWORD getMoney() const;
	string getName() const;
	int getPoints(int hand) const;
	void setName(string name);
	void appendByte(BYTE ch);
	void initiate();
	void addPoints(int hand, DWORD points);
	player(string name) { this->name = name; }
	player() { this->initiate(); }
	void addCard(card Card, int hand);
	vector<vector<card> > getCards() { return this->cards; }
	void resetCards() { this->cards.clear(); }
	void showCards(int show = 0) const;
	void showHand(int hand, int show = 0) const;
	void makeDecision(game &mGame, int hand, string opt = "#", bool isComputer = false);
	void playComputer(game &mGame);

	bool canHit(int hand);
	bool canDoubleDown(int hnad);
	bool canSplit(int hand);

	int split(int hand);
	int doubleDown(game &mGame, int hand);
	int hit(game &mGame, int hand);

	void setPoints();
	void setPointsComputer();
	void ShowPoints();

	int getWinProfit(player &computer);
	DWORD getBet();
	
	DWORD chooseBet(string opt = "#") const;
};

struct compPlayer {
	bool operator() (const player &a, const player &b) const {
		return a.getName() < b.getName();
	}
};

class game {
	set<player, compPlayer> mPlayers, playersOn;
	bool cards[43];
	HANDLE hFileWr;
	string withoutEndCh(string str);
	void changePlayer(player&, int wich = 1);
	void saveFile();
	int readPlayer(BYTE* buffer, int size, int &poz, player &mPlayer); 
	void readPlayers(BYTE *buffer, int size);
	void printPlayers(int wich = 1, int show = 0);
	int fullMenu(string comm = "#");
	
	int toMenu(bool firstTime = true);

	void openGame(string comm = "#");
	void signUpMenu(int step, string name = "#", string money = "#");
	void SignUp();
	void UpdatePlMoney(player &mPlayer, string money = "#");
	void UpdatePlayer(string name = "#");

	void playHand(player &mPlayer, bool isComputer = false);
	void play(int nrOfPlayers, bool firstTime = true);
	int playAgain(string option = "#");
	void initiate();
	int loginPl(string comm = "#");
	void startGame(int nrOfPlayer);

	void loginPlayers(int nrOfPlayers);
	
	
public:
	player computer;
	int getRandom();
	game();
};

game::game() {
	HANDLE hFile;
	BYTE *buffer;
	int size;

	hFile = getHandleForRead(".\\stats");

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
	free(buffer);

	openGame();

	saveFile();
	
}

// citirea din fisier si salvarea
void game::saveFile() {
	string bff, nm;
	char nr[10];
	DeleteFileA(".\\stats");
	hFileWr = getHandleForWrite(".\\stats", true);

	if (hFileWr == INVALID_HANDLE_VALUE) {
		printf("salvare fisier nereusita - %d\n", GetLastError());
		return;
	}

	set<player, compPlayer>::iterator it;
	for (it = mPlayers.begin(); it != mPlayers.end(); ++it) {
		sprintf_s(nr, 10, "%u", it->getMoney());
		bff += "#" + withoutEndCh(it->getName()) + "##" + nr + "#";
	}

	Write(hFileWr, (void *)bff.c_str(), bff.length(), 0);
	bff.clear();
	CloseHandle(hFileWr);
}

int game::readPlayer(BYTE *buffer, int size, int &poz, player &mPlayer) {
	int playerStSz = 0;
	string mn;
	mPlayer.initiate();
	//ajung pana la inceputul numelui
	while (poz < size && buffer[poz] != '#') {
		poz++;
	}
	if (poz >= size)
		return 0;
	poz++;
	//ajung pana la sfarsitul numelui
	while (poz < size && buffer[poz] != '#') {
		if (buffer[poz])
			mPlayer.appendByte(buffer[poz]);
		poz++;
	}
	if (poz >= size)
		return 0;
	poz++;
	mPlayer.appendByte(0);
	//ajung pana la suma de bani
	while (poz < size && buffer[poz] != '#')
		poz++;
	if (poz >= size)
		return 0;
	poz++;

	while (poz < size && buffer[poz] != '#') {
		if (buffer[poz])
			mn += buffer[poz];
		poz++;
	}

	poz++;
	mPlayer.setMoney(atoi(mn.c_str()));
	return 1;

}

void game::readPlayers(BYTE *buffer, int size) {
	int poz = 0;
	player mPlayer;
	while (readPlayer(buffer, size, poz, mPlayer)) {
		mPlayers.erase(mPlayer);
		mPlayers.insert(mPlayer);
	}
	printPlayers();
}
// deschiderea jocului
void game::openGame(string comm) {
	int option, opt2;
	string opt;
	option = fullMenu(comm);
	while (option == 0 || option > 5)
		option = fullMenu("Selectati o optiune valida\n");
	while (true) {
		switch (option) {
		case 1: SignUp();
			break;
		case 2: UpdatePlayer();
			break;
		case 3: startGame(1);
			break;
		case 4: startGame(2);
			break;
		default:
			return;
		}
		if (option == 3 || option == 4)
			return;
		opt2 = toMenu();
		if (opt2 == 2) {
			openGame();
			return;
		}
	}
}
//meniul principal
int game::fullMenu(string comm) {
	int err, option;
	string opt;
	system("cls");
	if (comm != "#")
		printf("%s\n\n", comm.c_str());
	printf("1. Inregistraare\n2. Update jucator\n3. Joc cu calculatorul\n4. Joc in doi\n5. Quit\n");
	cin >> opt;

	option = atoi(opt.c_str());
	return option;
}
// inregistrarea
void game::SignUp() {
	signUpMenu(1);
}

void game::signUpMenu(int step, string name, string money) {
	int value;
	string buff;
	player mPlayer;
	char nr[10];
	set<player, compPlayer>::iterator it;
	system("cls");
	printPlayers();
	printf("\n");
	if (step == 1) {
		if (name != "#")
			printf("'%s' nu este un nume valid\n\n", name.c_str());
		printf("Pasul 1/2 - Selecteaza numele: ");
		cin >> name;
		name += '\0';
		if (string::npos != name.find("#")) {
			signUpMenu(1, name);
			return;
		}
		mPlayer.setName(name);
		mPlayer.setMoney(15);
		it = mPlayers.find(mPlayer);
		if (mPlayers.end() != it) {
			signUpMenu(1, name);
			return;
		}
		signUpMenu(2, name);
		return;
	}
	else if (step == 2) {
		if (money != "#")
			printf("Scrieti un numar valid\n");
		printf("%Pasul 2/2 - Selecteaza suma de bani: ");
		cin >> money;
		value = atoi(money.c_str());
		if (value == 0 && money != "0") {
			signUpMenu(2, name, money);
			return;
		}
	}

	mPlayer.setMoney(value);
	mPlayer.setName(name);
	mPlayers.erase(mPlayer);
	mPlayers.insert(mPlayer);

	sprintf_s(nr, 10, "%u", mPlayer.getMoney());
	buff = "#" + mPlayer.getName() + "##" + nr + "#";

	Write(hFileWr, (void *)buff.c_str(), buff.length());

	return;
}
// update jucator
void game::UpdatePlayer(string name) {
	set<player, compPlayer>::iterator it;

	player mPlayer;
	system("cls");
	printPlayers();
	printf("\n");
	if (name != "#")
		printf("%s nu exista\n", name.c_str());
	printf("Selectati un jucator : ");
	cin >> name;
	name += '\0';
	mPlayer.setName(name);

	it = mPlayers.find(mPlayer);
	if (it == mPlayers.end()) {
		UpdatePlayer(name);
		return;
	}
	UpdatePlMoney(mPlayer);

}

void game::UpdatePlMoney(player &mPlayer, string money) {
	DWORD moneyi;
	system("cls");
	if (money != "#")
		printf("Alegeti un numar\n");
	printf("Selectati suma de bani : ");
	cin >> money;
	money += '\0';
	moneyi = atoi(money.c_str());
	if (moneyi == 0 && money[0] != '0') {
		UpdatePlMoney(mPlayer, money);
		return;
	}
	mPlayers.erase(mPlayer);
	mPlayer.setMoney(moneyi);
	mPlayer.resetPoints();
	mPlayers.insert(mPlayer);
}

// incep un joc(logarea)
void game::startGame(int nrOfPlayers) {
	string comm, opt;
	int option;
	char nr[10];
	if (nrOfPlayers == 0 || nrOfPlayers > 2) {
		openGame("S-a selectat un joc cu un numar nepotrivit de jucatori\n");
		return;
	}
	if (mPlayers.size() < nrOfPlayers) {
		sprintf_s(nr, 10, "%d", mPlayers.size());
		comm = "\nNu sunt destui jucatori, mai adauga, acum sunt - "; comm += nr; comm += " - jucatori\n";
		openGame(comm);
		return;
	}
	play(nrOfPlayers); 
}

void game::play(int nrOfPlayers, bool firstTime) {
	int option, profit = 0;
	DWORD bt;
	string go;
	set<player, compPlayer>::iterator it;
	set<player, compPlayer> aux;
	player mPlayer;

	//logarea jucatorilor
	if (firstTime)
		loginPlayers(nrOfPlayers);
	
	for(it = playersOn.begin(); it != playersOn.end(); ++it)
		if (it->getMoney() < 5) {
			printf("unul din jucatori are prea putini bani, adauga bani jucatorilor si revino\nPentru a continua scrie orice");
			cin >> go;
			playersOn.clear();
			return;
		}
	system("cls");
	printPlayers(2);

	
	// fiecare isi selecteazaz pariul
 	aux.clear();

	for (it = playersOn.begin(); it != playersOn.end(); ++it) {
		bt = it->chooseBet();
		if (bt == -1)
			return;
		mPlayer = *it;
		mPlayer.bet[0] = bt;
		aux.insert(mPlayer);
	}
	playersOn = aux;

	// fiecare jucator va primi cate 2 carti
	initiate();
	printPlayers(2, shBoth);

	// fiecare jucator isi joaca mainile
	aux.clear();
	for (it = playersOn.begin(); it != playersOn.end(); ++it) {
		mPlayer = *it;
		playHand(mPlayer);
		aux.insert(mPlayer);
	}
	playersOn.clear();
	playersOn = aux;
	aux.clear();

	//randul calculatorului
	playHand(computer, 1);

	computer.showHand(0, shBoth);

	printf("\n\n______________________\n\nRezultat:\n\n");
	// stabilirea castigatorilor si calcularea noilor sume de bani
	for (it = playersOn.begin(); it != playersOn.end(); ++it) {
		mPlayer = *it;
		printf("%s\n", it->getName().c_str());
		profit = mPlayer.getWinProfit(computer);
		mPlayer.setMoney(mPlayer.getMoney() + profit);
		aux.insert(mPlayer);
	}

	playersOn = aux;
	printf("\nScrie orice pentru a continua... ");
	cin >> go;

	// salvarea in lista principala a jucatorilor 
	for (it = playersOn.begin(); it != playersOn.end(); ++it) {
		mPlayers.erase(*it);
		mPlayers.insert(*it);
	}

	saveFile();
	option = playAgain();
	if (option == 1)
		play(nrOfPlayers, false);
	else if (option == 2)
		play(nrOfPlayers);
	else openGame();
}

void game::loginPlayers(int nrOfPlayers) {
	int option;
	playersOn.clear();
	for (int i = 0; i < nrOfPlayers; ++i) {
		option = loginPl();
		if (option == -1) {
			playersOn.clear();
			return;
		}
	}
}

int game::loginPl(string comm) {
	string opt;
	set<player, compPlayer>::iterator it;
	player mPlayer;
	char name[100];
	int option;
	system("cls");
	printPlayers();
	printPlayers(2);
	printf("\n---------------------------------\n");
	if (comm != "#")
		printf("%s\n", comm.c_str());

	printf("Pentru a iesi apasa '#'\nSelecteaza un jucator\n\n");
	cin >> opt;
	if (opt == "#")
		return -1;
	comm = opt;
	opt += '\0';
	mPlayer.setName(opt);

	it = playersOn.find(mPlayer);
	if (it != playersOn.end()) {
		comm += " este logat deja\n\n";
		return loginPl(comm);
	}
	else {
		it = mPlayers.find(mPlayer);
		if (it == mPlayers.end()) {
			comm += " nu exista\n\n";
			return loginPl(comm);
		}
		if (it->getMoney() <= 5) {
			comm += "nu are destui bani\n\n";
			return loginPl(comm);
		}
		playersOn.insert(*it);
		return 1;
	}
}

void game::initiate() {
	set<player, compPlayer>::iterator it;
	set<player, compPlayer> aux;
	player mPlayer;
	card mCard;
	DWORD bt;
	int j = 0;
	//toate cartile sunt disponibile
	for (int i = 1; i <= 42; ++i)
		cards[i] = false;
	// fiecaare jucator primeste 2 carti
	
	for (it = playersOn.begin(); it != playersOn.end(); ++it) {
		j++;
		mPlayer = *it;
		mPlayer.resetPoints();
		mPlayer.resetCards();
		for (int i = 1; i <= 2; ++i) {
			mCard.set(getRandom());
			//mCard.set(j * 13 + 10 + i);
			mPlayer.addCard(mCard, 0);
		}
		mPlayer.setPoints();
		aux.insert(mPlayer);
	}
	playersOn = aux;

	aux.clear();
	computer.resetCards();
	for (int i = 1; i <= 2; ++i) {
		mCard.set(getRandom());
		computer.addCard(mCard, 0);
	}
}

void game::playHand(player &mPlayer, bool isComputer) {
	system("cls");
	string opt;
	// incepe cu prima mana
	mPlayer.makeDecision(*this, 0, "#", isComputer);

	if (isComputer)
		return;
	printf("%s :\n", mPlayer.getName().c_str());
	mPlayer.ShowPoints();

	printf("\n\n scrie orice pentru a trece mai departe");
	cin >> opt;

}

int game::playAgain(string option) {
	int opt;
	string opt1;
	system("cls");
	if (option != "#")
		printf("%s - nu este o optiune valida\n", option.c_str());
	printf("1. Jucati din nou cu aceeasi jucatori\n2. Jucati din nou, dar cu jucatori diferiti\n3. Reveniti la meniu\n");
	cin.get();
	cin >> opt1;
	printf("%s\n", opt1.c_str());
	opt = atoi(opt1.c_str());
	if (opt == 0 || opt > 3)
		return playAgain(opt1);
	return opt;
}

//revenirea la meniu 
int game::toMenu(bool firstTime) {
	string opt;
	if (!firstTime)
		printf("Selectati o optiune valida\n");
	printf("\n1. Din nou\n2. Meniu\n");
	cin >> opt;
	if (opt == "1")
		return 1;
	if (opt == "2")
		return 2;
	return toMenu(false);
}

void game::printPlayers(int wich, int show) {
	int i = 1;
	set<player, compPlayer> wPlayers;
	if (wich == 1) {
		printf("jucatori logati\n");
		wPlayers = mPlayers;
	}
	else {
		printf("jucatori inregistrati\n");
		wPlayers = playersOn;
	}
	if (wich == 1) 
		printf("number of players : %d\n", wPlayers.size());
	set<player, compPlayer>::iterator it;
	for (it = wPlayers.begin(); it != wPlayers.end(); ++it, ++i) {
		printf("%d.%s - %d ", i, it->getName().c_str(), it->getMoney());
		if (show == 0)
			printf("\n");
		if (show & shCards) {
			if (!(show & shPoints)) {
				it->showCards();
				printf("\n");
			}
			else {
				it->showCards(1);
				printf("\n");
			}
		}
	}
}

//modificarea unui jucator
void game::changePlayer(player &mPlayer, int wich) {
	if (wich == 1) {
		mPlayers.erase(mPlayer);
		mPlayers.insert(mPlayer);
	}
	else {
		playersOn.erase(mPlayer);
		playersOn.insert(mPlayer);
	}
}

int game::getRandom() {
	int val;
	srand(time(NULL));
	do {
		val = rand() % 42 + 1;
	} while (cards[val]);
	cards[val] = true;
	return val;
}

//Player
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
	this->resetPoints();
}

void player::addPoints(int hand, DWORD points) {
	this->points[hand] += points;
}

void player::ShowPoints() {
	for (int i = 0; i < cards.size(); ++i) {
		printf("Mana %d - %d puncte; ", i + 1, points[i]);
	}
	printf("\n");
}

string game::withoutEndCh(string name) {
	string str;
	
	for (int i = 0; i < name.length() - 1; ++i)
		str += name[i];
	if (name[name.length() - 1])
		str += name[name.length()];

	return str;
}

void player::showHand(int hand, int show ) const{
	printf("\nMana %d : ", hand + 1);
	for (int i = 0; i < cards[hand].size(); ++i) {
		cards[hand][i].print();
		printf(" ");
	}
	if (show) {
		printf("%d ", points[hand]);
	}
}
void player::showCards(int show) const{
	for (int i = 0; i < this->cards.size(); ++i) {
		if (cards[i].size())
			showHand(i, show);
	}
}

int player::getPoints(int hand) const { 
	return this->points[hand]; 
}

void player::addCard(card Card, int hand) {
	vector<card> cr;
	if (cards.size() <= hand) {
		cr.push_back(Card);
		cards.push_back(cr);
	}
	else {
		this->cards[hand].push_back(Card);
	}
}

int player::split(int hand) {
	card mCard;

	mCard = cards[hand][1];
	addCard(mCard, cards.size());
	cards[hand].pop_back();
	
	return 0;
}

int player::hit(game &mGame, int hand) {
	card mCard;
	int value;

	value = mGame.getRandom();
	mCard.set(value);
	addCard(mCard, hand);

	return 0;
}

int player::doubleDown(game &mGame, int hand) {
	bet[hand] *= 2;
	hit(mGame, hand);

	return 0;
}

void player::setPoints() {
	int nrAs;
	for (int i = 0; i < cards.size(); ++i) {
		points[i] = 0;
		nrAs = 0;
		for (int j = 0; j < cards[i].size(); ++j) {
			if (cards[i][j].value == 11)
				nrAs++;
			else
				points[i] += cards[i][j].value;
		}
		for (int k = nrAs; k >= 0; k--) {
			if (points[i] + 11 * k + 1 * (nrAs - k) <= 21) {
				points[i] += 11 * k + 1 * (nrAs - k);
				break;
			}
		}
	}
}

void player::setPointsComputer() {
	bool ace = false;
	points[0] = 0;
	for (int i = 0; i < cards[0].size(); ++i) {
		if (cards[0][i].value == 11) {
			if (ace)
				points[0] += 1;
			else {
				points[0] += 11;
				ace = true;
			}
		}
		else points[0] += cards[0][i].value;
	}
}

void player::playComputer(game &mGame) {
	setPointsComputer();

	if (points[0] < 17) {
		hit(mGame, 0);
		playComputer(mGame);
	}
	return;

}

void player::makeDecision(game &mGame, int hand, string opt, bool isComputer) {
	string option;
	int optg;
	int valid[6], options = 2;
	char buff[100];

	for (int i = 0; i < 6; ++i)
		valid[i] = 0;

	system("cls");

	if (isComputer) {
		printf("Computer\n");
		playComputer(mGame);
		return;
	}

	setPoints();

	printf("%s\nBani : %u\nPariu : %u\n", getName().c_str(), getMoney(), getBet());

	if (points[hand] >= 20)
		return;

	if (opt != "#") {
		if (opt == sShowHand) {
			showHand(hand, 1);
		}
		else if (opt == sStand) {
			return;
		}
		else if (opt == sHit)
			;
		else if (opt == sDoubleDown) {
			return;
		}
		else if (option[0] != '0')
			printf("%s nu este o optiune valida\n", opt.c_str());
	}

	// setez optiuni valabile
	if (canHit(hand))
		valid[HIT] = valid[DOUBLEDOWN] = 1;
	if (canSplit(hand))
		valid[SPLIT] = 1;

	printf("\n1. Arata cartile\n2. Stand\n");
	if (canHit(hand)) {
		printf("3. Hit\n");
		options++;
		if (canDoubleDown(hand)) {
			printf("4. DoubleDown\n");
			options++;
		}
	}
	if (canSplit(hand)) {
		printf("5. Split\n");
		options++;
	}

	// citirea optiunii
	cin >> option;
	optg = atoi(option.c_str());

	if (optg < 1 || optg > options) {
		makeDecision(mGame, hand, option);
		return;
	}

	//tratarea optiunii
	switch (optg) {
	case 1:
		makeDecision(mGame, hand, sShowHand);
		return;
	case 2:
		makeDecision(mGame, hand, sStand);
		return;
	case 3: hit(mGame, hand);
		makeDecision(mGame, hand, sHit);
		return;
	case 4: doubleDown(mGame, hand);
		makeDecision(mGame, hand, sDoubleDown);
		return;
	case 5: split(hand);
		makeDecision(mGame, hand);
		makeDecision(mGame, cards.size() - 1);
		return;
	default:
		return;
	}
}

bool player::canHit(int hand) {
	return getPoints(hand) < 21;
}

bool player::canSplit(int hand) {
	DWORD totalBet;
	totalBet = getBet();
	return cards[hand].size() == 2 && cards[hand][0].value == cards[hand][1].value && money >= totalBet + bet[hand];
}

bool player::canDoubleDown(int hand) {
	DWORD totalBet;

	totalBet = getBet();
	return money >= totalBet + bet[hand];
}

int player::getWinProfit(player &computer) {
	int profit = 0;

	for (int i = 0; i < cards.size(); ++i) {
		printf("Mana %d - %d - %d - ", i + 1, points[i], computer.getPoints(0));
		if (points[i] > 21) {
			printf("pierdut. ");
		}
		else if (points[i] == computer.getPoints(0)) {
			printf("egalitate. ");
		}
		else if (points[i] == 21) {
			printf("castigat cu 21. ");
			profit = profit + double(1.5) * double(bet[i]);
		}
		else if (points[i] > computer.getPoints(0)) {
			printf("castigat. ");
			profit += bet[i];
		}
		else if (points[i] < computer.getPoints(0)) {
			if (computer.getPoints(0) <= 21) {
				printf("pierdut. ");
				profit -= bet[i];
			}
			else {
				printf("castigat. ");
				profit += bet[i];
			}
		}
	}
	printf("\n");
	return profit;
}

DWORD player::getBet() {
	DWORD totalBet = 0;
	for (int i = 0; i < cards.size(); ++i)
		totalBet += bet[i];

	return totalBet;
}

DWORD player::chooseBet(string opt) const{
	DWORD bt;
	printf("%s\nBani: \n", name.c_str(), money);
	if (opt != "#")
		printf("%s nu este o optiune valida\n", opt.c_str());
	printf("Selectati suma de bani dorita(minim 5). Daca doriti sa iesiti scrieti #\n");
	cin >> opt;
	if (opt == "#")
		return -1;
	bt = atoi(opt.c_str());

	if (bt < 5)
		return chooseBet(opt);
	return bt;
}

//card

void card::set(int value) {
	char nr[3];
	this->value = value % 13 + 2;

	sprintf_s(nr, 3, "%d", this->value);
	switch (this->value)
	{
	case 11:
		this->face = 'J';
		this->value = 10;
		break;
	case 12: 
		this->face = 'Q';
		this->value = 10;
		break;
	case 13:
		this->face = 'K';
		this->value = 10;
		break;
	case 14:
		this->face = 'A';
		this->value = 11;
		break;
	default:
		this->face = nr;
		break;
	}

	switch (value / 13) {
	case 0:
		this->face += HEART;
		return;
	case 1:
		this->face += DIAMOND;
		return;
	case 2:
		this->face += CLUB;
		return;
	case 3:
		this->face += SPADE;
		return;
	default:
		return;
	}
}
void card::print() const { 
	printf("%s", face.c_str()); 
}
