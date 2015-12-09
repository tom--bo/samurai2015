#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <cassert>
#include <random>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

struct CommentedIStream {
  istream* is;
  char commentChar;
  CommentedIStream() {};
  CommentedIStream(istream &is);
  bool good();
  int get();
  void skipComments();
};

CommentedIStream& operator>>(CommentedIStream& cs, int& i);

CommentedIStream& operator>>(CommentedIStream& cs, string& str);

struct ErrorReport {
  string msg;
  ErrorReport(string s);
};

struct Coordinates {
  const int x;
  const int y;
  Coordinates(int x, int y);
  Coordinates north() const;
  Coordinates south() const;
  Coordinates east() const;
  Coordinates west() const;
  Coordinates operator+(const Coordinates c) const;
  Coordinates rotate(const int direction) const;
  string toString() const;
  bool operator==(const Coordinates another) const;
};

CommentedIStream& operator>>(CommentedIStream& cs, Coordinates& coords);

struct CoordHash {
  size_t operator()(const Coordinates& c) const;
};

struct Section;

struct FieldMap: unordered_map <Coordinates, Section*, CoordHash> {
  Section* locate(Coordinates p);
};

struct SamuraiState;
struct GameState;
struct Role;

struct Section {
  const Coordinates coords;
  int state; // -1: free; s>=0: occupied by samurai s
  int population;
  SamuraiState* apparent;
  Section *neighbors[4];
  Section(Coordinates coords);
  void setNeighbors(FieldMap& map);
  void leave(SamuraiState* s);
  void arrive(SamuraiState* s);
  void occupy(Role& role);
};

struct BattleField {
  const int width;
  const int height;
  FieldMap map;
  BattleField(int w, int h);
  Section* section(int x, int y);
  void occupy(GameState& state, Role& role, int direction, Section& pos);
};

struct Samurai {
  // Permanent info
  string name;
  string progname;
  string invocation;
  // Tournament state info
  int score;
  int rank;
  Samurai() {};
  Samurai(CommentedIStream& cs);
  void readScore(CommentedIStream& ss);
};

struct SamuraiDB {
  int numSamurai;
  string programDir;
  Samurai *samuraiList;
  SamuraiDB();
  SamuraiDB(string samuraiDBname, string scoreDBname);
};

struct Role {
  const int id;
  vector <Coordinates> reach;
  int vision;
  int activity;
  int homeX;
  int homeY;
  Role(int id, CommentedIStream& cs);
};

struct Setting {
  string gameName;
  int turns;
  int timeAllowed;
  int width;
  int height;
  int cureTurns;
  SamuraiDB samuraiDB;
  vector<Role> roles[2];
  Setting(CommentedIStream &cs);
  void sendSettingInfo(ostream& os, const int weapon);
};

struct SamuraiState {
  Samurai* samurai;
  Role* role;
  int side;
  int weapon;
  Section* position;
  int curePeriod;
  bool hidden;
  pid_t processId;
  string dirname;
  string toAIpath;
  string fromAIpath;
  ostream* toAI;
  CommentedIStream fromAI;
  bool alive;
  int response;
  bool done;
  queue<int> actions;
  void init(Setting& setting, BattleField& field, int id, int side, int weapon);
  bool move(GameState& state, int direction, ostream& comments);
  bool occupy(GameState& state, int direction, ostream& comments);
  bool hide(ostream& comments);
  bool appear(ostream& comments);
  void cure();
  void house(BattleField& field);
  void die(BattleField& field);
  void injure(BattleField& field, Setting& setting);
};
  
struct GameState {
  Setting setting;
  BattleField battleField;
  int turn;
  SamuraiState* samuraiStates[2];
  GameState(Setting& stng, int players[]);
  void sendGameInfo(ostream& os, const int side, const int weapon);
  void sendTurnInfo(ostream& os, const int side, const int weapon);
  void receiveActionCommands
  (CommentedIStream& is, const int side, const int weapon, ostream& log);
};

ostream& operator<<(ostream& os, const Coordinates& coord);
ostream& operator<<(ostream& os, const Role& role);
ostream& operator<<(ostream& os, const Setting& setting);
ostream& operator<<(ostream& os, const SamuraiDB& db);

void receiveFromAI(CommentedIStream& is, const Setting& setting,
		   GameState& gameState, const int side, const int weapon,
		   ostream& log, int turn);

extern bool dump;
extern ostream* ds;
extern bool logif;
extern ofstream ifs[];
