#include "gameManager.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <set>
#include <string>

using namespace std;

//gameManager::gameManager(){}

gameManager::gameManager(string dictionaryfilename, string boardfilename, string bagfilename){
	mDict = new Dictionary(dictionaryfilename);
	mBoard = new Board(boardfilename);
	mBag = new Bag(bagfilename, 10);
	mSquare = mBoard->getSquare();
	mMadeMove = 0;
}

gameManager::~gameManager(){
	delete mDict;
	delete mBoard;
	delete mBag;
	for (unsigned int i = 0; i<mPlayers.size(); i++){
	 	delete [] mPlayers.at(i);
	}
}

vector<Player*> gameManager::createPlayers(int numberUsers){
	string name;
	//creates a new player with a name and set of tiles
	//and pushes them onto a vector
	for(int i = 0; i<numberUsers; i++){
		mUI.getNameInput(i);
		cin>>name;
		Player* player = new Player(name, mBag->drawTiles(7));//, mBag);
		mPlayers.push_back(player);
	}
	cout<<endl;
	return mPlayers;
}

string gameManager::validTiles(string tilesToGive, string fullSet, bool &equiv){
	for(unsigned int i = 0; i<tilesToGive.size(); i++){
		for (unsigned int j = 0; j<fullSet.size(); j++){
			if(tilesToGive[i]==fullSet[j]){
				tilesToGive.erase(i,1);
				//cout<<"did it even erase the curr letter?"<<tilesToGive[0]<<endl;
				return tilesToGive;
			}
		}
		//cout<<"did it make bool 0 ?? "<<endl;
		equiv = 0;
		return tilesToGive;
	}
	return tilesToGive;
}

string gameManager::editPlayerTiles(string tilesToGive, vector<Tile*> &tilesGiving, set<Tile*> &currPlayerTiles){
	set<Tile*>::iterator it;
	for (it = currPlayerTiles.begin(); it!=currPlayerTiles.end(); it++){//put all their curr tiles in a vector
		Tile* tile = *it;
		for(unsigned int i = 0; i<tilesToGive.size();i++){
			if(tile->getLetter()==tilesToGive[i]){
				tilesGiving.push_back(tile);
				tilesToGive.erase(i,1);
				currPlayerTiles.erase(it);
				return tilesToGive;
			}
		}
	}
	return tilesToGive;
}

string gameManager::addTilestoVector(string tilesPlaced, vector<Tile*> &tilesPlacing, set<Tile*> &currPlayerTiles){
	set<Tile*>::iterator it;
	for (it = currPlayerTiles.begin(); it!=currPlayerTiles.end(); it++){//put all their curr tiles in a vector
		Tile* tile = *it;
		for(unsigned int i = 0; i<tilesPlaced.size();i++){
			if(tile->getLetter()==tilesPlaced[i]){
				tilesPlacing.push_back(tile);
				tilesPlaced.erase(i,1);
			}
		}
	}
	return tilesPlaced;
}

void gameManager::exchangeTiles(string tilesToGive){
	vector<Tile*> tilesGiving;
	string currTileLetters;
	bool equiv = 1;
	set<Tile*> currPlayerTiles = mPlayers.at(mTurn)->getTiles();
	set<Tile*>::iterator it;
	//search through all player's tiles and add the tile letters to a string
	for (it = currPlayerTiles.begin(); it!=currPlayerTiles.end(); it++){//put all their curr tiles in a vector
		Tile* tile = *it;
		currTileLetters += tile->getLetter();
	}

	//make a copy of the string of playerTiles just in case pointers get affected
	string copyTileLetters = currTileLetters;

	//make a copy of the string of tilestoGive
	string tempTilesToGive = tilesToGive;

	//check if the tiles they want to exchange are actually in their set of tiles
	while(!tilesToGive.empty()){
		tilesToGive = validTiles(tilesToGive, copyTileLetters, equiv);
		if(equiv == 0){//bool value becomes 0 if they lied about a letter
			cout<<"NOT EQUIVALENT. RETRY"<<endl;
			break;
		}
	}
	if(equiv==0){}
	else{
		//now we know tiles are valid so we 
		//can now edit the playertiles so we go through
		//all their tiles one by one searching for the 
		//match to the tile they want to rid of and 
		//once match found, that tile is added to a vector
		//that index in the string of tilestogive is deleted
		//and the iterator pointing to the tile they want to
		//exchange is deleted. Now, we can add the vector
		//to the bag addTiles and then draw tiles = # they rid of
		//and then update their tiles 
		tilesToGive = tempTilesToGive;
		while(!tilesToGive.empty()){
			tilesToGive = editPlayerTiles(tilesToGive, tilesGiving, currPlayerTiles);
		}
		mBag->addTiles(tilesGiving);
		set<Tile*> newSet = mBag->drawTiles(tilesGiving.size());
		for(it=newSet.begin(); it!=newSet.end(); it++){
			Tile* tile = *it;
			currPlayerTiles.insert(tile);
		}
		getPlayer(mTurn)->updatePlayerTiles(currPlayerTiles);

	}
}


vector<Tile*> gameManager::adjustVector(string tilesPlaced, vector<Tile*> &tilesPlacing){
	vector<Tile*> tilesUpdatedPlacing;
	string tempTilesPlaced = tilesPlaced;
	for (unsigned int i = 0; i<tempTilesPlaced.size(); i++){
		cout<<"tilesplaced in adjustvector is size: "<<tilesPlaced.size()<<"and is hopefully word SPRAYS: "<<tilesPlaced<<endl;
		cout<<"tilesplacing in adjustVector shoulbe be same size as tilesplaced and is: "<<tilesPlacing.size()<<endl;
		for(unsigned int j = 0; j<tilesPlacing.size(); j++){
			if(tilesPlaced[i]==tilesPlacing.at(j)->getLetter()){
				tilesUpdatedPlacing.push_back(tilesPlacing.at(j));
				tilesPlacing.erase(tilesPlacing.begin()+j);
			}
		}
	}
	return tilesUpdatedPlacing;
}

bool gameManager::tileCheckCovered(string dir, int rowArrayView, int colArrayView, vector<Tile*> &tilesPlacing){
	if(dir=="-"){
		for(unsigned int i = 0; i<tilesPlacing.size(); i++){
			if(mSquare[rowArrayView][colArrayView+i]->isCovered()==true){
				return false;
			}
			else{return true;}
		}
	}
	else if(dir == "|"){
		for(unsigned int i = 0; i<tilesPlacing.size();i++){
			if(mSquare[rowArrayView+i][colArrayView]->isCovered()==true){
				return false;
			}
			else{return true;}
		}
	}
	else{return false;}
	return false;
}

bool gameManager::isThereAdjacent(string dir, int rowArrayView, int colArrayView, vector<Tile*> &tilesPlacing){
	cout<<"tilesplacing size is: "<<tilesPlacing.size()<<endl;
	if(dir=="-"){
		for(unsigned int i = 0; i<tilesPlacing.size(); i++){
			cout<<"are we in the - adjacent shit?"<<endl;
			if(atStartPlease(rowArrayView,colArrayView)==true){return true;}
			if(mSquare[rowArrayView+1][colArrayView]->isCovered()==true||
				mSquare[rowArrayView-1][colArrayView]->isCovered()==true||
				mSquare[rowArrayView][colArrayView-1]->isCovered()==true){
				return true;
			}
			else if(mSquare[rowArrayView][colArrayView+tilesPlacing.size()]->isCovered()==true){
				return true;
			}
			else{return false;}
		}
	}
	else if(dir == "|"){
		for(unsigned int i = 0; i<tilesPlacing.size(); i++){
			cout<<"are we in the | adjacent shit?"<<endl;
			if(atStartPlease(rowArrayView,colArrayView)==true){return true;}
			if(mSquare[rowArrayView-1][colArrayView]->isCovered()==true||
				mSquare[rowArrayView][colArrayView+1]->isCovered()==true||
				mSquare[rowArrayView][colArrayView-1]->isCovered()==true){
				return true;
			}
			else if(mSquare[rowArrayView+tilesPlacing.size()][colArrayView]->isCovered()==true){
				return true;;
			}
			else{return false;}
		}
	}
	else{return false;}
	return false;
}

bool gameManager::atStartPlease(int rowArrayView, int colArrayView){
	int checkStartX = mBoard->getStartRow();
	cout<<"checkstarx is: "<<checkStartX<<endl;
	int checkStartY = mBoard->getStartCol();
	cout<<"checkstary is: "<<checkStartY<<endl;
	if (mSquare[checkStartX][checkStartY]->isCovered()==0 && (rowArrayView!=checkStartX || colArrayView!=checkStartY)){
		return false;
	}
	if(mSquare[checkStartX][checkStartY]->isCovered()==0 && (rowArrayView==checkStartX && colArrayView==checkStartY)){
		cout<<"this should be activated! and then cover the damn square"<<endl;
		return true;
	}
	else if(mSquare[checkStartX][checkStartY]->isCovered()==1 && (rowArrayView==checkStartX && colArrayView==checkStartY)){
	return false;
	}
	return true;
}

void gameManager::copyVector(vector<Tile*> &tilesAdjacent, vector<Tile*> &tilesPlacing){
	for(unsigned int i= 0; i<tilesPlacing.size(); i++){
		tilesAdjacent.push_back(tilesPlacing.at(i));
	}
}

string gameManager::grabRightSideWords(int rowArrayView, int colArrayView, string &fullWordToRightStr, vector<Tile*> &tilesPlacing){
	unsigned int vectorCounter = 0;
	unsigned int counter = 0;
	cout<<"size of vector is: "<<tilesPlacing.size()<<endl;
	cout<<"item in vector initial and next is: "<<tilesPlacing.at(0)->getLetter()<<endl;
	for(int i = colArrayView; i<mBoard->getCols(); i++){
		if(mSquare[rowArrayView][colArrayView+counter]->isCovered()==false){
			cout<<"should be here twice: "<<counter<<endl;
			if(vectorCounter==tilesPlacing.size()){return fullWordToRightStr;}
			else{
				fullWordToRightStr += tilesPlacing.at(vectorCounter)->getLetter();
				cout<<"fullWordToRight is: "<<fullWordToRightStr<<endl;
				vectorCounter++;
				counter++;
			}
		}
		else if(mSquare[rowArrayView][colArrayView+counter]->isCovered()==true){
			fullWordToRightStr = fullWordToRightStr + mSquare[rowArrayView][colArrayView+counter]->getSquareLetters();
			cout<<"what is fullWordToRightStr when it grabs board letter if it even does: "<<fullWordToRightStr<<endl;
			counter++;
		}
	}
	return fullWordToRightStr;
}

string gameManager::grabWordsBelow(int rowArrayView, int colArrayView, string &fullWordBelowStr, vector<Tile*> &tilesPlacing){
	unsigned int vectorCounter = 0;
	unsigned int counter = 0;

	for(int i = colArrayView; i<mBoard->getRows(); i++){
		if(mSquare[rowArrayView+counter][colArrayView]->isCovered()==false){
			cout<<"should be here twice: "<<counter<<endl;
			if(vectorCounter==tilesPlacing.size()){return fullWordBelowStr;}
			else{
				fullWordBelowStr += tilesPlacing.at(vectorCounter)->getLetter();
				cout<<"fullWordBelowStr is: "<<fullWordBelowStr<<endl;
				vectorCounter++;
				counter++;
			}
		}
		else if(mSquare[rowArrayView+counter][colArrayView]->isCovered()==true){
			fullWordBelowStr = fullWordBelowStr + mSquare[rowArrayView+counter][colArrayView]->getSquareLetters();
			cout<<"what is fullWordBelowStr when it grabs board letter if it even does: "<<fullWordBelowStr<<endl;
			counter++;
		}
	}
	return fullWordBelowStr;
}

bool gameManager::validWord(string checkWord){
	cout<<"word is: "<<checkWord<<endl;
	if(mDict->find(checkWord)==true){return true;}
	return false;
}

bool gameManager::validVertAdjacency(int rowArrayView, int colArrayView, vector<Tile*> &tilesPlacing){
	int counter = 0;
	string checkWordUp = "";
	string vectorToString = "";
	string fullWordBelowStr = "";
	vector<Tile*> tilesAdjacent;
	vector<Tile*> fullWordToRight;
	copyVector(tilesAdjacent, tilesPlacing);
	fullWordBelowStr = grabWordsBelow(rowArrayView, colArrayView, fullWordBelowStr, tilesPlacing);
	stringstream ss;
	ss<<fullWordBelowStr;
	ss>>fullWordBelowStr;
	cout<<"what is in fullWordBelowStr?" <<fullWordBelowStr<<endl;
	if(mSquare[rowArrayView-1][colArrayView]->isCovered()==true){
		while(mSquare[rowArrayView-counter-1][colArrayView]->isCovered()==true){
			checkWordUp = checkWordUp + mSquare[rowArrayView-counter-1][colArrayView]->getSquareLetters();
			counter++;
			cout<<"checkwordUp is: "<<checkWordUp<<endl;
		}
		string tempCheckWordUp = "";
		for(unsigned int i = 0; i<checkWordUp.size(); i++){
			tempCheckWordUp += checkWordUp[checkWordUp.size()-i-1];
		}
		checkWordUp = tempCheckWordUp;
		cout<<"reversed word is now: "<<checkWordUp<<endl;
		checkWordUp = checkWordUp+fullWordBelowStr;
		cout<<"now checkwordUp after full concatenation is: "<<checkWordUp<<endl;
		if(validWord(checkWordUp)==true){
			completeVertTile = checkWordUp;
			return true;
		}
	}
	else if(validWord(fullWordBelowStr)==true){
		completeVertTile = fullWordBelowStr;
		cout<<"does it get here? and completeVertTile is: "<<completeVertTile<<endl;
		return true;
	}
	else if(mSquare[rowArrayView-1][colArrayView]->isCovered()==false && 
	mSquare[rowArrayView+1][colArrayView]->isCovered()==false){return true;}
	else if(validWord(fullWordBelowStr)==false){
		cout<<"did it return false for vert word?"<<endl;return false;}
	return true; ///should this be here......????????????????????????????????
}
 
bool gameManager::validHorizAdjacency(int rowArrayView, int colArrayView, vector<Tile*> &tilesPlacing){//to get the letter at a square, use getSquareLetters
	int counter = 0;
	string checkWordLeft = "";
	string vectorToString = "";
	string fullWordToRightStr = "";
	vector<Tile*> tilesAdjacent;
	vector<Tile*> fullWordToRight;
	copyVector(tilesAdjacent, tilesPlacing);
	//grabrightsidewords tries to go through where you want to place and checks all the tiles to the right
	//and if a square is not covered, it adds your letter to a vector. if a square is full, it adds that square's letter
	//to the same vector fullWordToRight. Then, there's a point when your letters are out AND the square is not covered
	//this means that you return your fullwordtoright vector b/c you have completed the word to the right
	fullWordToRightStr = grabRightSideWords(rowArrayView, colArrayView, fullWordToRightStr, tilesPlacing);
	stringstream ss;
	ss<<fullWordToRightStr;
	ss>>fullWordToRightStr;
	cout<<"what is in fullWordToRightStr?"<<fullWordToRightStr<<endl;
	if(mSquare[rowArrayView][colArrayView-1]->isCovered()==true){
		while(mSquare[rowArrayView][colArrayView-counter-1]->isCovered()==true){
			checkWordLeft = checkWordLeft + mSquare[rowArrayView][colArrayView-counter-1]->getSquareLetters();
			counter++;
			cout<<"checkwordLeft is: "<<checkWordLeft<<endl;
		}
		string tempCheckWordLeft = "";
		for(unsigned int i = 0; i<checkWordLeft.size(); i++){
			tempCheckWordLeft += checkWordLeft[checkWordLeft.size()-i-1];
		}
		checkWordLeft = tempCheckWordLeft;
		cout<<"reversed word is now: "<<checkWordLeft<<endl;
		checkWordLeft = checkWordLeft+fullWordToRightStr;
		cout<<"now checkword after full concatenation is: "<<checkWordLeft<<endl;
		if(validWord(checkWordLeft)==true){
			completeTile = checkWordLeft;
			return true;
		}
	}
	else if(validWord(fullWordToRightStr)==true){
		completeTile = fullWordToRightStr;
		cout<<"does it get here? and completeTile is: "<<completeTile<<endl;
		return true;
	}
	else if(mSquare[rowArrayView][colArrayView-1]->isCovered()==false && 
	mSquare[rowArrayView][colArrayView+1]->isCovered()==false){return true;}
	else if(validWord(fullWordToRightStr)==false){
		cout<<"did it return false for DIE?"<<endl;return false;}
	return true; ///should this be here......????????????????????????????????
}

bool gameManager::doCheck(bool &fullCheck, string &dir, int rowArrayView, int colArrayView, vector<Tile*> &tilesPlacing, string &tilesPlaced){
	//if anytime something should not work, fullcheck should become FALSE but if things are good, fullCheck is TRUE

	if(rowArrayView>=0 && rowArrayView<15 && colArrayView>=0 && colArrayView<15){
		fullCheck = atStartPlease(rowArrayView, colArrayView);
		cout<<"fullCheck after atstartplease here is: "<<fullCheck<<endl;
		if(fullCheck==false){mUI.placeTileCheck(); return false;}

		fullCheck = isThereAdjacent(dir, rowArrayView, colArrayView, tilesPlacing);
		if(fullCheck == false){mUI.placeTileCheck(); return false;}

		fullCheck = tileCheckCovered(dir, rowArrayView, colArrayView, tilesPlacing);
		cout<<"fullcheck after tilecovered here is: "<<fullCheck<<endl;
		if(fullCheck==false){mUI.placeTileCheck(); return false;}

//this is to just place the first word so not both adjacency functions are called
		if((dir=="-"||dir=="|") && rowArrayView==mBoard->getStartRow() && colArrayView==mBoard->getStartCol()){
			if(dir=="-"){
				fullCheck = validHorizAdjacency(rowArrayView,colArrayView,tilesPlacing);
				cout<<"validadjacency fullcheck is: "<<fullCheck<<endl;
				if(fullCheck==false){mUI.placeTileCheck(); return false;}
			}
			if(dir=="|"){
				fullCheck = validVertAdjacency(rowArrayView, colArrayView, tilesPlacing);
				cout<<"validVertAdjacency fullcheck is: "<<fullCheck<<endl;
				if(fullCheck==false){mUI.placeTileCheck(); return false;}
			}	
		}
		//should create word horizontally and validate if real word
		else{
			fullCheck = validHorizAdjacency(rowArrayView,colArrayView,tilesPlacing);
			cout<<"validadjacency fullcheck is: "<<fullCheck<<endl;
			if(fullCheck==false){mUI.placeTileCheck(); return false;}
			
			//should create word vertically and validate if real word
			fullCheck = validVertAdjacency(rowArrayView, colArrayView, tilesPlacing);
			cout<<"validVertAdjacency fullcheck is: "<<fullCheck<<endl;
			if(fullCheck==false){mUI.placeTileCheck(); return false;}
		}
	}
	else{mUI.placeTileCheck(); return false;}
	return true;
}

int gameManager::horizontalPlace(int &doubleTimeCounter, int &tripleCounter, int rowArrayView, int colArrayView, string &doubleTime, string &triple, 
	int wordScore, vector<Tile*> &tilesPlacing){
	
	cout<<"completeTile is: "<<completeTile<<"and should be PAY"<<endl;
	unsigned int vectorCounter = 0;
	for(unsigned int i = 0; i<completeTile.size(); i++){
		if(mSquare[rowArrayView][colArrayView+i]->isCovered()==0){
			if(vectorCounter<tilesPlacing.size()){
				if(mSquare[rowArrayView][colArrayView+i]->getCharMultiplier() == 'd'){
					doubleTimeCounter++;
					doubleTime = "D";
				}
				if(mSquare[rowArrayView][colArrayView+i]->getCharMultiplier() == 't'){
					tripleCounter++;
					triple = "T";
				}
				mSquare[rowArrayView][colArrayView+i]->addTileHoriz(tilesPlacing[vectorCounter]->getLetter(),tilesPlacing[vectorCounter]->getPoints());
			}
			vectorCounter++;
		}
	}
	int counter = 0;
	while(mSquare[rowArrayView][colArrayView-counter-1]->isCovered()==true){
		counter++;
	}
	cout<<"counter is: "<<counter<<"and should be 3"<<endl;
	for(unsigned int i = 0; i<completeTile.size(); i++){
		wordScore += mSquare[rowArrayView][colArrayView-counter+i]->getScore();
		cout<<"wordscore is: "<<wordScore<<endl;
	}
	return wordScore;
}
int gameManager::verticalPlace(int &doubleTimeCounter, int &tripleCounter, int rowArrayView, int colArrayView, string &doubleTime, string &triple, 
	int wordScore, vector<Tile*> &tilesPlacing){

	unsigned int vectorCounter = 0;
	for(unsigned int i = 0; i<completeVertTile.size(); i++){
		if(mSquare[rowArrayView+i][colArrayView]->isCovered()==0){
			if(vectorCounter<tilesPlacing.size()){
				if(mSquare[rowArrayView+i][colArrayView]->getCharMultiplier() == 'd'){
					doubleTimeCounter++;
					doubleTime = "D";
				}
				if(mSquare[rowArrayView+i][colArrayView]->getCharMultiplier() == 't'){
					tripleCounter++;
					triple = "T";
				}
				mSquare[rowArrayView+i][colArrayView]->addTileHoriz(tilesPlacing[vectorCounter]->getLetter(),tilesPlacing[vectorCounter]->getPoints());
			}
			vectorCounter++;
		}
	}
	int counter = 0;
	while(mSquare[rowArrayView-counter-1][colArrayView]->isCovered()==true){
		counter++;
	}
	cout<<"counter is: "<<counter<<"and should be 3"<<endl;
	for(unsigned int i = 0; i<completeVertTile.size(); i++){
		wordScore += mSquare[rowArrayView-counter+i][colArrayView]->getScore();
		cout<<"wordscore is: "<<wordScore<<endl;
	}
	return wordScore;
}
void gameManager::placeTiles(string dir, int row, int col, string tilesPlaced){
	//use same technique of checking if tiles are valid like in the Exchange call
	vector<Tile*> tilesPlacing;
	vector<Tile*> totalTiles;
	string currTileLetters;
	bool equiv = 1;
	set<Tile*> currPlayerTiles = mPlayers.at(mTurn)->getTiles();
	set<Tile*>::iterator it;
	//search through all player's tiles and add the tile letters to a string
	for (it = currPlayerTiles.begin(); it!=currPlayerTiles.end(); it++){//put all their curr tiles in a vector
		Tile* tile = *it;
		currTileLetters += tile->getLetter();
	}

	//make a copy of the string of playerTiles just in case pointers get affected
	string copyTileLetters = currTileLetters;

	//make a copy of the string of tilestoGive
	string tempTilesToPlace = tilesPlaced;

	//check if the tiles they want to exchange are actually in their set of tiles
	while(!tilesPlaced.empty()){
		//Using valid TileExch for Place rather than Exchange. just checking if valid
		tilesPlaced = validTiles(tilesPlaced, copyTileLetters, equiv);
		if(equiv == 0){//bool value becomes 0 if they lied about a letter
			mUI.exchangeTileCheck();
			break;
		}
	}
	tilesPlaced = tempTilesToPlace;
	if(equiv==0){}
	else{
		int rowArrayView = row-1;
		int colArrayView = col-1;
		cout<<"tilesplaced string should be SPRAYS and is: "<<tilesPlaced<<endl;
		tilesPlaced = addTilestoVector(tilesPlaced, tilesPlacing, currPlayerTiles); //adding tiles to tilesplacing
		tilesPlaced = tempTilesToPlace;
		tilesPlacing = adjustVector(tilesPlaced, tilesPlacing);
		tilesPlaced = tempTilesToPlace;
		cout<<"tilesplaced string please still be SPRAYS and it is: "<<tilesPlaced<<"and size: "<<tilesPlaced.size()<<endl;
		cout<<"What about vector size??? it should be 6 and is: "<<tilesPlacing.size()<<endl;
		for(unsigned int i =0; i<tilesPlacing.size(); i++){
			cout<<"WHAT about the vector tilesplacing??? well, it should be SPRAYS and is: "<<tilesPlacing.at(i)->getLetter()<<endl;
		}
		bool fullCheck = 1;
		fullCheck = doCheck(fullCheck, dir, rowArrayView, colArrayView, tilesPlacing, tilesPlaced);
		cout<<"full check is: "<<fullCheck<<endl;

		if(fullCheck==false){cout<<"fullcheck is false!"<<endl; mUI.placeTileCheck();}
		else{
			tilesPlacing.clear();
			while(!tilesPlaced.empty()){
				tilesPlaced = editPlayerTiles(tilesPlaced, tilesPlacing, currPlayerTiles);
			}
			tilesPlaced = tempTilesToPlace;
			//adjustvector is to fix order of word in vector
			tilesPlacing = adjustVector(tilesPlaced, tilesPlacing);
			int wordScore = 0;
			if(dir == "-"){
				string doubleTime = "";
				string triple = "";
				int doubleTimeCounter = 0;
				int tripleCounter = 0;
				wordScore = horizontalPlace(doubleTimeCounter, tripleCounter, rowArrayView, colArrayView, doubleTime, triple, wordScore, tilesPlacing);
				cout<<"wordscore is: "<<wordScore<<endl;
				cout<<"doubleTimeCounter is: "<<doubleTimeCounter<<" and tripleCounter is: "<<tripleCounter<<endl;
				set<Tile*> newSet = mBag->drawTiles(tilesPlacing.size());
				for(it=newSet.begin(); it!=newSet.end(); it++){
					Tile* tile = *it;
					currPlayerTiles.insert(tile);
				}
				if(doubleTime == "D"){wordScore = (wordScore*2)*doubleTimeCounter;}
				if(triple == "T"){wordScore = (wordScore*3)*tripleCounter;}
				getPlayer(mTurn)->updatePlayerTiles(currPlayerTiles);
				getPlayer(mTurn)->setScore(wordScore);
				if(tilesPlacing.size()==7){getPlayer(mTurn)->setScore(50);}
			}
			else if(dir == "|"){
				string doubleTime = "";
				string triple = "";
				int doubleTimeCounter = 0;
				int tripleCounter = 0;
				wordScore = verticalPlace(doubleTimeCounter, tripleCounter, rowArrayView, colArrayView, doubleTime, triple, wordScore, tilesPlacing);
				set<Tile*> newSet = mBag->drawTiles(tilesPlacing.size());
				for(it=newSet.begin(); it!=newSet.end(); it++){
					Tile* tile = *it;
					currPlayerTiles.insert(tile);
				}
				if(doubleTime == "D"){wordScore = (wordScore*2)*doubleTimeCounter;}
				if(triple == "T"){wordScore = (wordScore*3)*tripleCounter;}
				getPlayer(mTurn)->updatePlayerTiles(currPlayerTiles);
				getPlayer(mTurn)->setScore(wordScore);
				if(tilesPlacing.size()==7){getPlayer(mTurn)->setScore(50);}
			}
			mMadeMove = 1;
		}
	}
}
void gameManager::readMove(){
	stringstream ss;
	string totalMove;
	totalMove = mUI.getMoves(mPlayers.at(mTurn));
	ss<<totalMove;
	string moveType;
	string direction;
	string strRow;
	string strColumn;
	string tilesPlaced;
	string tilesExchanged;
	ss>>moveType;
	if(moveType == "PASS"){
		mMadeMove = 1;
	}
	else if(moveType == "EXCHANGE"){
		ss>>tilesExchanged;
		if(!tilesExchanged.empty()){
			exchangeTiles(tilesExchanged);
			mMadeMove = 1;
		}
	}
	else if(moveType == "PLACE"){
		ss>>direction;
		ss>>strRow;
		int row = atoi(strRow.c_str());
		ss>>strColumn;
		int col = atoi(strColumn.c_str());
		ss>>tilesPlaced;
		if(!tilesPlaced.empty()){placeTiles(direction, row, col, tilesPlaced);}
	}
}


Player* gameManager::getPlayer (int i){
	return mPlayers.at(i);
}

void gameManager::printBoard(Board* mBoard, Square*** mSquare){
	mUI.printBoard(mBoard, mSquare);
}

void gameManager::printCurrTiles(){
	mUI.printPlayerTiles(getPlayer(mTurn)->getTiles());
}

void gameManager::startGame(){
	mTurn=0;
	mNumUsers = mUI.getNumUsers();
	mPlayers = createPlayers(mNumUsers);
	while(mBag->tilesRemaining()>0){
		mMadeMove = 0;
		//up until the vector everyone assignment is just asking user for number of players and creating a vector of players with names
		mTurn = mTurn % mNumUsers;
		//Player* currPlayer = mPlayers.at(mTurn);
		string currPlayerName = mPlayers.at(mTurn)->getName();			
		mUI.whosTurn(currPlayerName);
		mUI.printScores(mNumUsers, mPlayers);
		printBoard(mBoard, mSquare);
		readMove();
		if(mMadeMove == 1){mTurn++;}
		else{}
	}
}