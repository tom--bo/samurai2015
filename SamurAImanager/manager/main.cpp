#include "samurai.hpp"
#include <cctype>

void initLog(ostream& os, Setting& setting, int players[]) {
  os << "### SamurAI 3x3 Game Log ###\n";
  os << "# <game name> <#turns> <width> <height>\n"
     << '"' << setting.gameName << "\" "
     << setting.turns << ' '
     << setting.width << ' ' << setting.height << ' '
     << setting.cureTurns << endl;
  os << "# Army sizes\n"
     << "3 3\n";
  os << "# Samurai Info\n";
  for (int a = 0; a != 2; a++) {
    for (int w = 0; w != 3; w++) {
      os << "# Army " << a << " Samurai " << w << endl;
      Samurai& s = setting.samuraiDB.samuraiList[players[3*a+w]];
      Role& r = setting.roles[a][w];
      os << '"' << s.name << "\" "
	 << s.rank << ' ' << s.score << endl;
      os << r.reach.size();
      for (Coordinates c: r.reach) {
	os << ' ' << c.x << ' ' << c.y;
      }
      os << endl;
      os << r.vision << ' ' << r.activity << ' '
	 << r.homeX << ' ' << r.homeY << endl;
    }
  }
}

bool dump = false;
ofstream dumpStream;
ostream* ds;

bool logif = false;
ofstream ifs[6];

[[noreturn]]
static void invalidOption(char* argv[], int k,
			  string msg = "Invalid option") {
  cerr << "Usage: " << argv[0]
       << " [-h] [-r] [-d] [-D<path>] [-a<path>]" << endl
       << "Specify -h for further help on options" << endl;
  throw ErrorReport(msg + ": " + argv[k]);
}

[[noreturn]]
static void helpMessage(char* argv[]) {
  cerr << "Usage: " << argv[0]
       << " [-h] [-r] [-d] [-D<path>] [-a<path>]" << endl
       << "  -h: obtain this help message" << endl
       << "  -r: game result report output" << endl
       << "  -d: dump game process" << endl
       << "  -D<path>: dump game process to the specifiedl path" << endl
       << "  -a<path>: dump AI program interface output" << endl
       << "    Output path for player N is obtained by concatenating <path> and N" << endl;
  exit(0);
}

static void processOptions(int argc, char* argv[]) {
  for (int k = 1; k < argc; k++) {
    if (*argv[k] != '-') invalidOption(argv, k);
    char* option = argv[k] + 1;
    switch (option[0]) {
    case 'd':
      if (option[1] != 0) invalidOption(argv, k);
      dump = true;
      ds = &cerr;
      break;
    case 'D':
      dump = true;
      if (option[1] == 0) 
	invalidOption(argv, k, "Dump output file not specified");
      dumpStream.open(option+1);
      if (!dumpStream.good()) 
	throw ErrorReport(string("Failed to open file ") + (option+1));
      ds = &dumpStream;
      break;
    case 'a': {
      if (option[1] == 0) 
	invalidOption(argv, k, "Interface dump file name not specified");
      for (int s = 0; s != 6; s++) {
	string filename(option+1);
	filename += ('0'+s);
	ifs[s].open(filename);
	if (!ifs[s].good()) 
	  throw ErrorReport(string("Failed to open file ") + filename);
      }
      logif = true;
      break;
    }
    case 'h':
      helpMessage(argv);
    default:
      invalidOption(argv, k);
    }
  }
}


int main(int argc, char* argv[]) {
  int turn;
  try {
    turn = -1;
    processOptions(argc, argv);
    CommentedIStream cs(cin);
    Setting setting(cs);
    int players[6];
    for (int s = 0; s != 6; s++) {
      players[s] = s;
    }
    initLog(cout, setting, players);
    GameState gameState(setting, players);
    int order[] = { 0, 3, 4, 1, 2, 5, 3, 0, 1, 4, 5, 2 };
    for (turn = 0; turn != setting.turns; turn++) {
      gameState.turn = turn;
      for (int a = 0; a != 2; a++) {
	for (int w = 0; w != 3; w++) {
	  gameState.samuraiStates[a][w].cure();
	}
      }
      int samuraiUp = order[turn%12];
      int activeSide = samuraiUp/3;
      int activeSamurai = samuraiUp%3;
      if (dump) {
	*ds << "### Turn: " << turn << "; Side: " << activeSide
	    << "; Samurai: " << activeSamurai << "; Actions:";
      }
      ostream* toAI =
	gameState.samuraiStates[activeSide][activeSamurai].toAI;
      gameState.sendTurnInfo(*toAI, activeSide, activeSamurai);
      if (logif) {
	int s = 3*activeSide+activeSamurai;
	gameState.sendTurnInfo(ifs[s], activeSide, activeSamurai);
	ifs[s].flush();
      }
      CommentedIStream& fromAI =
	gameState.samuraiStates[activeSide][activeSamurai].fromAI;
      gameState.receiveActionCommands(fromAI, activeSide, activeSamurai, cout);
      if (dump) {
	*ds << endl;
	int width = gameState.battleField.width;
	int height = gameState.battleField.height;
	FieldMap map = gameState.battleField.map;
	for (int y = 0; y != height; y++) {
	  for (int x = 0; x != width; x++) {
	    Section* sect = map.at(Coordinates(x, y));
	    *ds << ' ';
	    if (sect->state < 0) {
	      *ds << '.';
	    } else {
	      *ds << sect->state;
	    }
	    if (sect->population != 0) {
	      *ds << '*';
	    } else {
	      *ds << ' ';
	    }
	  }
	  *ds << '\n';
	}
	for (int a = 0; a != 2; a++) {
	  for (int w = 0; w != 3; w++) {
	    SamuraiState& ss = gameState.samuraiStates[a][w];
	    *ds << "Samurai " << ss.side << "." << ss.weapon << "@"
		<< ss.position->coords.toString();
	    if (ss.hidden) *ds << "; hiding";
	    if (ss.curePeriod != 0) *ds << "; resting: " << ss.curePeriod;
	    *ds << endl;
	  }
	}
	ds->flush();
      }
    }
    for (int a = 0; a != 2; a++) {
      for (int w = 0; w != 3; w++) {
	SamuraiState& ss = gameState.samuraiStates[a][w];
	*ss.toAI << -1 << endl;
	remove(ss.toAIpath.c_str());
	remove(ss.fromAIpath.c_str());
	remove(ss.dirname.c_str());
      }
    }
    return 0;
  } catch (ErrorReport err) {
    cerr << err.msg << endl;
    if (turn < 0) {
      cerr << "Before game starts" << endl;
    } else {
      cerr << "At turn " << turn << endl;
    }
    return -1;
  }
}
