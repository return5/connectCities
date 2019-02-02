#include <ncurses.h>  //needs -lncurses gcc flag
#include <unistd.h>	 //usleep
#include <stdlib.h>	//malloc adn realloc
#include <time.h>  //time()
#include <assert.h>


//============================= typedefs, global vars, and constants ==========================
typedef struct traintrack {
	int length,max_length,k,at_station;
	int *track_x;
	int *track_y;
	void (*moveTrain)(int i,const int j, int k);
	const char* (*getCaboosePiece)(const int i,const int j);
	const char* (*getEnginePiece)(const int i,const int j);
}traintrack;

typedef struct city{
	int x,y;
}city;

#define HEIGHT 20
#define WIDTH 35
#define ENDTRACK tracks[i]->track_x[tracks[i]->length-1],tracks[i]->track_y[tracks[i]->length-1]
#define STARTTRACK tracks[i]->track_x[0],tracks[i]->track_y[0]
#define SLEEP_TIME 150000

const size_t size_track = sizeof(traintrack);
const size_t size_city = sizeof(city);
int previous_y, previous_x,x,y; 						//cursor x,y values
int track_i = -1;										//index value for tracks array
int tracks_length;										//size of tracks array
int num_cities;
int longest_track = 0;									//longest track length
int play_game = 1;   					   				//flag for running game loop. 1 = run, 0 = stop
int place_tracks = 0;									//flag for placing tracks. 0 = dont, 1 = do place track
int player_money = 200;
int prev_track_i = 0;									//holds track_i when finishing unfinished tracks
traintrack **tracks;
city **cities;
WINDOW *game_win;
WINDOW *prompt_win;

//============================= prototypes ====================================================
void printTrains(void);
void makeNewTrack(void);
void makeTrainTrack(void);
void placeTrack(void);
int checkIfCity(const int x_cor, const int y_cor);
void gameLoop(void);
void initScreen(void);
void initGame(void);
void makeCities(void);
void printCities(void);
void setPrev(void);
void moveCurs(void);
void getKeyboardInput(void);
void checkTracksSize(void);
void checkTrackLength(void); 
int checkMoney(void);
void checkPayOut(void);
void getMoney(const int i);
void printPrompt(void);
void makeGameBorder(void);
void makeGameWindow(void);
void makePromptBorder(void);
void makePromptWindow(void);
void makeColors(void);
void cleanUp(void);
int notSameSpot(void);
void moveX(const int x_dir);
void moveY(const int y_dir);
int checkIfUnfinishedTrack(void);
void checkCityDistance(void);
int cityDistance(const int i, const int j);
int randint(int n);
void printTrainPiece(const int i, const int j,const int k);
void printPreviousPiece(const int i, const int j);
void printTrainAtEnd(const int i,const int j);
const char* getTrainPiece(const int i, const int j,const int k);
const char* getEnginePieceA(const int i, const int j);
const char* getCaboosePieceA(const int i, const int j);
const char* getEnginePieceB(const int i, const int j);
const char* getCaboosePieceB(const int i, const int j);
void printTrainCarsA(int i,const int j, int k);
void printTrainCarsB(int i,const int j, int k);
void resetK(void);
void switchTrackFuncPtr(void);
void refreshTrack(void);
void makeInstructionBorder(void);
void makeInstructionWindow(void);

//============================= main program ==================================================


int main (void) {
	initScreen();
	initGame();
	gameLoop();
 	exit (EXIT_SUCCESS);
}

void gameLoop(void) {
	while(play_game) {	
		printPrompt();
		getKeyboardInput();	
		moveCurs();
		if(place_tracks && notSameSpot()) {
			placeTrack();
		}
	}
}

void getKeyboardInput(void) {
	switch(getch()){
		case KEY_UP: 
					setPrev();
					moveY(-1);
			break;
		case KEY_DOWN:
					setPrev();
					moveY(1);
			break;
		case KEY_LEFT:
					setPrev();
					moveX(-1);
			break;
		case KEY_RIGHT:
					setPrev();
					moveX(1);
			break;
		case 'y': makeNewTrack();
				break;
		case 'g' : printTrains();
				   resetK();
				   switchTrackFuncPtr();
				   checkPayOut();
			break;
		case 'q': play_game = 0;
				break;
		default : //do nothing
			break;
		}
}

void setPrev(void) {
	previous_x = x;
	previous_y = y;
}

void moveCurs(void) {
	wmove(game_win,y,x);
	wrefresh(game_win);
}

int notSameSpot(void) {
	if (x == previous_x && y == previous_y) {
		return 0;
	}
	return 1;
}

void moveX(const int x_dir) {
	if (x_dir  == -1 && x >= 1) {
		x += x_dir;
	}
	else if (x_dir == 1 && x <= WIDTH - 2) {
		x += x_dir;
	} 
}

void moveY(const int y_dir) {
	if (y_dir  == -1 && y >= 1) {
		y += y_dir;
	}
	else if (y_dir == 1 && y <= HEIGHT - 2) {
		y += y_dir;
	} 
}

void makeNewTrack(void) {
	int i;
	if(place_tracks == 0 && checkIfCity(x,y) == 1 ) {
		checkTracksSize();
		makeTrainTrack();
		place_tracks = 1;
		curs_set(0);
	}
	else if (place_tracks == 0 && (i = checkIfUnfinishedTrack()) != -1) {
		tracks[i]->length--;
		prev_track_i = track_i;
		track_i = i;
		place_tracks = 1;
	}
	else if (place_tracks == 1 && checkIfCity(x,y) == 1){
		place_tracks = 0;
		track_i = prev_track_i;
		curs_set(1);
		printCities();
	}
}

int checkIfUnfinishedTrack(void) {
	int i;
	for (i = 0; i <= track_i; i++) {
		if(tracks[i]->track_x[tracks[i]->length-1] == x && tracks[i]->track_y[tracks[i]->length-1] == y) {
			if(!checkIfCity(x,y)) {
				return i;
			}
		}
	}
	return -1;
}

int checkIfCity(const int x_cor, const int y_cor) {
	int i;
	for(i = 0; i < num_cities; i++) {
		if(x_cor == cities[i]->x && y_cor == cities[i]->y) {
			return 1;
		}
	}
	return 0;
}

void makeTrainTrack(void) {
	traintrack *newtrack = malloc(size_track);
	newtrack->length = 0;
	newtrack->max_length = 10;
	newtrack->track_x = malloc(sizeof(int) * 10);
	newtrack->track_y = malloc(sizeof(int) * 10);
	newtrack->k = 1;
	newtrack->at_station = 1;
	newtrack->moveTrain = printTrainCarsA;
	newtrack->getEnginePiece = getEnginePieceA;
	newtrack->getCaboosePiece = getCaboosePieceA;	
	tracks[track_i] = newtrack;
}


void checkTracksSize(void) {
	track_i++;
	prev_track_i = track_i;
	if(track_i >= tracks_length) {
		tracks_length += 5;
		traintrack **temp = realloc(tracks,(size_track * tracks_length));
		if(temp == NULL){
			temp = realloc(tracks,(size_track * tracks_length));
		}
		tracks = temp;
	}
}

void placeTrack(void) {
	if(checkMoney()) {
		tracks[track_i]->track_x[tracks[track_i]->length] = x;
		tracks[track_i]->track_y[tracks[track_i]->length] = y;
		tracks[track_i]->length++;
		checkTrackLength();
		if (tracks[track_i]->length > longest_track) {
			longest_track = tracks[track_i]->length;
		}
		mvwprintw(game_win,y,x,"#");
		player_money -= 10;
	}
}

int checkMoney(void) {
	if(player_money >= 10) {
		return 1;
	}
	else {
		curs_set(1);
		place_tracks = 0;
		return 0;
	}
}

void checkTrackLength(void) {
	if(tracks[track_i]->length >= tracks[track_i]->max_length) {
		tracks[track_i]->max_length += 10;
		int *temp = realloc(tracks[track_i]->track_x,sizeof(int) * tracks[track_i]->max_length);
		if(temp == NULL) {
			temp = realloc(tracks[track_i]->track_x,sizeof(int) * tracks[track_i]->max_length);
		}
		tracks[track_i]->track_x = temp;
		temp = realloc(tracks[track_i]->track_y,sizeof(int) * tracks[track_i]->max_length);
		if(temp == NULL) {
			temp = realloc(tracks[track_i]->track_y,sizeof(int) * tracks[track_i]->max_length);
		}
		tracks[track_i]->track_y = temp;
	}
}

void checkPayOut(void) {
	int i;
	for (i = 0; i <= track_i; i++) {
		if(checkIfCity(STARTTRACK) && checkIfCity(ENDTRACK)){
			getMoney(i);
			wrefresh(game_win);
		}
	}
}

void getMoney(const int i) {
	const int len = tracks[i]->length;
	const int len_x = abs(tracks[i]->track_x[0] - tracks[i]->track_x[len-1]);
	const int len_y = abs(tracks[i]->track_y[0] - tracks[i]->track_y[len-1]);
	const double ratio = (double)(len_x + len_y) / len;
	player_money +=(int) ((len * 12) * ratio);
}


//iterates through tracks array to print each train
void printTrains(void) {
	int i,j,limit;
	curs_set(0);
	track_i = prev_track_i;
	limit = longest_track + 4;
	for(i = 1; i <= limit; i++ ) {
		for(j = 0; j <= track_i; j++) {   //if track doesnt end on city, skip it
			if(!checkIfCity(tracks[j]->track_x[tracks[j]->length-1],tracks[j]->track_y[tracks[j]->length-1])) {
					tracks[j]->at_station = 1;
			}
			else if(i < tracks[j]->length) {
				tracks[j]->moveTrain(i,j,0);
				tracks[j]->at_station = 0;
			}
			else if (i < tracks[j]->length + 4) {
				tracks[j]->moveTrain(tracks[j]->length-1,j,tracks[j]->k++);
				tracks[j]->at_station = 0;
			}
			else {
				tracks[j]->at_station = 1;
			}
		}
		refreshTrack();
	}
	curs_set(1);
}

void refreshTrack(void) {
	int i,sum;
	sum = 0;
	for(i = 0; i <=track_i; i++) {
		sum += tracks[i]->at_station;
	}
	if(sum <= track_i) {  //if all trains are at station, skip sleep and refresh
		wrefresh(game_win);
		usleep(SLEEP_TIME);
	}
}

//recursive. prints all the train cars going A to B, then prints train track behind train
void printTrainCarsA(int i,const int j, int k) {
	if(k < 4 && i > 0) {
		printTrainPiece(i,j,k);
		i--;
		k++;
		printTrainCarsA(i,j,k);
	}
	else {
		printPreviousPiece(i,j);
	}
}

//recursive. prints all the train cars going B to A, then prints train track behind train
void printTrainCarsB(int i,const int j, int k) {
	if(k < 4 && tracks[j]->length - i < tracks[j]->length - 1) {
		printTrainPiece(tracks[j]->length - i,j,k);
		i--;
		k++;
		printTrainCarsB(i,j,k);
	}
	else {
		printPreviousPiece(tracks[j]->length - i,j);
	}
}

//prints correct train piece for that location, or if it is a city, prints city icon
void printTrainPiece(const int i, const int j,const int k) {
	if(checkIfCity(tracks[j]->track_x[i],tracks[j]->track_y[i]) == 1) {
		mvwprintw(game_win,tracks[j]->track_y[i],tracks[j]->track_x[i],"@");
	}
	else {
		wattron(game_win,COLOR_PAIR(k+1));
		mvwprintw(game_win,tracks[j]->track_y[i],tracks[j]->track_x[i],getTrainPiece(i,j,k));
		wattroff(game_win,COLOR_PAIR(k+1));
	}
}

//prints piece immediatly behind train. 
void printPreviousPiece(const int i, const int j) {
	if(checkIfCity(tracks[j]->track_x[i],tracks[j]->track_y[i]) == 1) {
		mvwprintw(game_win,tracks[j]->track_y[i],tracks[j]->track_x[i],"@");
	}
	else {
		mvwprintw(game_win,tracks[j]->track_y[i],tracks[j]->track_x[i],"#");
	}
}

//returns correct train piece based on value of k.
const char* getTrainPiece(const int i, const int j,const int k) {
	switch (k) {
		case 0: return tracks[j]->getEnginePiece(i,j);
			break;
		case 1:	//fall through
		case 2: return "+";
			break;
		case 3: return tracks[j]->getCaboosePiece(i,j);
			break;
		default:
			break;		
	}
}

//returns correct orientation of engine piece when moving A to B
const char* getEnginePieceA(const int i, const int j) {
	if(tracks[j]->track_y[i] < tracks[j]->track_y[i+1]) {
		return "v";
	}
	else if (tracks[j]->track_y[i] > tracks[j]->track_y[i+1]) {
		return "^";
	}
	else if (tracks[j]->track_x[i] > tracks[j]->track_x[i+1]){
		return "<";
	}
	else {
		return ">";
	}
}

//return correct orientation of caboose piece when moving A to B
const char* getCaboosePieceA(const int i, const int j) {
	if(tracks[j]->track_y[i] < tracks[j]->track_y[i+1]) {
		return "^";
	}
	else if (tracks[j]->track_y[i] > tracks[j]->track_y[i+1]) {
		return "v";
	}
	else if (tracks[j]->track_x[i] > tracks[j]->track_x[i+1]){
		return ">";
	}
	else {
		return "<";
	}
}

//returns correct orientation of engine piece when moving B to A
const char* getEnginePieceB(const int i, const int j) {
	if(tracks[j]->track_y[i] < tracks[j]->track_y[i-1]) {
		return "v";
	}
	else if (tracks[j]->track_y[i] > tracks[j]->track_y[i-1]) {
		return "^";
	}
	else if (tracks[j]->track_x[i] > tracks[j]->track_x[i-1]){
		return "<";
	}
	else {
		return ">";
	}
}

//return correct orientation of caboose piece when moving B to A
const char* getCaboosePieceB(const int i, const int j) {
	if(tracks[j]->track_y[i] < tracks[j]->track_y[i-1]) {
		return "^";
	}
	else if (tracks[j]->track_y[i] > tracks[j]->track_y[i-1]) {
		return "v";
	}
	else if (tracks[j]->track_x[i] > tracks[j]->track_x[i-1]){
		return ">";
	}
	else {
		return "<";
	}
}

void switchTrackFuncPtr(void) {
	int i;
	for(i = 0; i <= track_i; i++) {
		if(tracks[i]->moveTrain == &printTrainCarsA) {
			tracks[i]->moveTrain = printTrainCarsB;
			tracks[i]->getEnginePiece = getEnginePieceB;
			tracks[i]->getCaboosePiece = getCaboosePieceB;
		}
		else {
			tracks[i]->moveTrain = printTrainCarsA;
			tracks[i]->getEnginePiece = getEnginePieceA;
			tracks[i]->getCaboosePiece = getCaboosePieceA;
		}
	}
}

void resetK(void) {
	int i;
	for(i = 0; i <= track_i; i++) {
		tracks[i]->k =1;
	}
}

void printCities(void){
	int i;
	for(i = 0; i < num_cities; i++) {
		mvwprintw(game_win,cities[i]->y, cities[i]->x,"@");
	}
	wmove(game_win,0,0);
	wrefresh(game_win);
}

void makeCities(void) {
	int i;
	for(i = 0; i < num_cities; i++){
		cities[i] = malloc(size_city);
		cities[i]->x = rand() / (RAND_MAX / WIDTH + 1);
		cities[i]->y = rand() / (RAND_MAX / HEIGHT + 1);
	}
}

void checkCityDistance(void) {
	int i,j;
	for(i = 0; i < num_cities;i++) {
		for(j = 0; j < num_cities; j++) {
			if( j != i && cityDistance(i,j) == 1) {
				cities[i]->x = randint(WIDTH);
				cities[i]->y = randint(HEIGHT);
				j = -1;
			}
		}
	}
}

/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 */
//found in an answer by Laurence Gonsalves on stack overflow
int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {

    // Supporting larger values for n would requires an even more
    // elaborate implementation that combines multiple calls to rand()
    assert (n <= RAND_MAX);

    // Chop off all of the values that would cause skew...
    int end = RAND_MAX / n; // truncate skew
    assert (end > 0);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}

int cityDistance(const int i, const int j) {
	if((abs(cities[i]->x - cities[j]->x) < 7 )&& (abs(cities[i]->y - cities[j]->y) < 7)) {
		return 1;
	}
	else {
		return 0;
	}
}

void initScreen(void){
	initscr();
	noecho();	      //dont display key strokes
	cbreak();	     //disable line buffering
	keypad(stdscr, TRUE);	  //enable keypad
	refresh();
	makeGameWindow();
	makeGameBorder();
	makePromptWindow();
	makePromptBorder();
	makeInstructionBorder();
	makeInstructionWindow();
}

void initGame(void){
	time_t t;
	srand((unsigned) time(&t));
	num_cities= randint(4) + 7;
	tracks_length = num_cities;
	tracks = malloc(size_track * tracks_length);
	cities = malloc(size_city * num_cities);   
	previous_y = previous_x = x = y = 0;
	atexit(cleanUp);
	makeCities();
	checkCityDistance();
	printCities();
	makeColors();
}

void makeColors(void) {
	start_color();  
	init_pair(1,COLOR_YELLOW,COLOR_BLACK);
	init_pair(2,COLOR_GREEN,COLOR_BLACK);
	init_pair(3,COLOR_GREEN,COLOR_BLACK);	
	init_pair(4,COLOR_RED,COLOR_BLACK);

}

void makeGameBorder(void) {
	WINDOW *game_border = newwin(HEIGHT+2,WIDTH+2,0,0);
	wborder(game_border,'|','|','-', '-', '+', '+', '+', '+');
	wrefresh(game_border);
}

void makeGameWindow(void) {
	game_win = newwin(HEIGHT,WIDTH,1,1);
	wrefresh(game_win);
}

void makePromptBorder(void) {
	WINDOW *prompt_border = newwin(3,26,0,WIDTH + 4);
	wborder(prompt_border,'|','|','-', '-', '+', '+', '+', '+');
	wrefresh(prompt_border);
}

void makePromptWindow(void) {
	prompt_win = newwin(1,25,1,WIDTH + 5);
	wrefresh(prompt_win);
}
void makeInstructionBorder(void) {
	WINDOW *instruction_border = newwin(6,32,5,WIDTH + 4);
	wborder(instruction_border,'|','|','-', '-', '+', '+', '+', '+');
	wrefresh(instruction_border);
}

void makeInstructionWindow(void) {
	WINDOW *instruction_win = newwin(4,30,6,WIDTH + 5);
	mvwprintw(instruction_win,0,0,"use arrow keys to move cursor");
	mvwprintw(instruction_win,1,0,"use y key to place track");
	mvwprintw(instruction_win,2,0,"use g key to start trains");
	mvwprintw(instruction_win,3,0,"use q key to quit");
	wrefresh(instruction_win);
	moveCurs();
}

void printPrompt(void) {
	mvwprintw(prompt_win,0,0,"player money is %d  ", player_money);
	wrefresh(prompt_win);
	moveCurs();
}

void cleanUp(void) {
	free(tracks);
	free(cities);
	endwin(); 
}