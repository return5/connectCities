/*
	pieces header for connectcities v. 2.0
	github.com/return5
	license: MIT
*/

enum   COLORS    {BLUE,GREEN,RED,MAGENTA,YELLOW,CYAN};
enum   TRAINCARS {ENGINE,CAR,CABOOSE};

typedef struct TRAINTRACK TRAINTRACK;
typedef struct TRACKPIECE TRACKPIECE;
typedef struct MOVETRAIN  MOVETRAIN;

struct TRACKPIECE {                //individual piece of track
	int            x,y;           //x and y coordinates of track piece
	TRACKPIECE     *next,*prev;  //next and prvious track piece on the traintrack
};

typedef struct TRAIN {			        //train cars
	enum   TRAINCARS    piece;	       //type of train car
	enum   COLORS       color;	      //color of the train car
	struct TRAIN        *next;       //next train car on this train
	TRACKPIECE          *location;  //holds track piece where train car is currently at
}TRAIN;

typedef struct CITY {		
	int           x, y;	   //x and y coordinates of city
	enum COLORS   color;  //color of city.
}CITY;

struct MOVETRAIN {					              //holds functions for moving train left or right along track.
	TRACKPIECE *(*getNextTrack)(const TRACKPIECE *const track);
	MOVETRAIN *next;					            //holds the next function. used to switch back and forth from left to right and back again
};

struct TRAINTRACK {		                  //struct to hold all information for entire traintrack
	int             len_track;	         //length of track
	double 			payout;             //payout percentage for track
	TRACKPIECE      *start_track;	    //start of the train track
	TRACKPIECE      *last_track;	   //last track piece on the line
	TRAIN           *engine,*caboose; //engine is start of train, caboose is end of train
	CITY            *head, *end;     //start and stop cities for the traintrack
	MOVETRAIN       *movetrain;	    //functions for moving train correct direction
	TRAINTRACK      *next;		   //linked list to hold all of the traintracks
};	
