#include "samurai.hpp"
#include <thread>
#include <signal.h>

Role::Role(int id, CommentedIStream& cs): id(id) {
  int reachSize;
  cs >> reachSize;
  for (int k = 0; k != reachSize; k++) {
    int x, y;
    cs >> x >> y;
    reach.emplace_back(Coordinates(x, y));
  }
  cs >> vision >> activity >> homeX >> homeY;
}

Setting::Setting(CommentedIStream &cs) {
  string samuraiDBname;
  string scoreDBname;
  cs >> samuraiDBname >> scoreDBname;
  samuraiDB = SamuraiDB(samuraiDBname, scoreDBname);
  cs >> gameName >> turns >> timeAllowed
     >> width >> height >> cureTurns;
  int armySizes[2];
  cs >> armySizes[0] >> armySizes[1];
  for (int a = 0; a != 2; a++) {
    for (int s = 0; s != armySizes[a]; s++) {
      roles[a].emplace_back(3*a+s,cs);
    }
  }
}

void Setting::sendSettingInfo(ostream& os, const int weapon) {
  os << "# <number of turns>\n" << turns << endl;
  os << "# <samurai ID>\n" << weapon << endl;
  os << "# <field width> <field height>\n" << width << ' ' << height << endl;
  os << "# Home positions of samrai\n";
  for (int a = 0; a != 2; a++) {
    for (Role& role: roles[a]) {
      os << role.homeX << ' ' << role.homeY << endl;
    }
  }
}

void SamuraiState::init
(Setting& setting, BattleField& field, int id, int sd, int wp) {
  samurai = &setting.samuraiDB.samuraiList[id];
  side = sd;
  weapon = wp;
  role = &setting.roles[side][weapon];
  position = field.section(role->homeX, role->homeY);
  position->arrive(this);
  curePeriod = 0;
  hidden = false;
  char cdirname[] = "/tmp/SamurAIXXXXXX";
  if (mkdtemp(cdirname) == 0)
    throw ErrorReport("Filed to make a temporary directory: "+errno);
  dirname = cdirname;
  toAIpath = dirname + "/toAI";
  mkfifo(toAIpath.c_str(), 0600);
  fromAIpath = dirname + "/fromAI";
  mkfifo(fromAIpath.c_str(), 0600);
  processId = fork();
  if (processId == -1) {
    throw ErrorReport("Failed to fork an AI process: "+errno);
  } else if (processId == 0) {
    // Child process
    string command(setting.samuraiDB.programDir);
    if (command.back() != '/') command += '/';
    command += samurai->invocation;
    for (size_t pos = command.find("$1");
	pos != string::npos;
	pos = command.find("$1")) {
      command.replace(pos, 2, samurai->progname);
    }
    system((command + " <" + toAIpath + " >" + fromAIpath).c_str());
    exit(0);
  } else {
    toAI = new ofstream(toAIpath);
    ifstream* fromAIstream = new ifstream(fromAIpath);
    fromAI = CommentedIStream(*fromAIstream);
  }
}

bool SamuraiState::move(GameState& state, int direction, ostream& comments) {
  Section* p = position->neighbors[direction];
  if (p == 0) {
    comments << "Invalid move direction: " << direction
	     << " from " << position->coords.toString();
    return false;
  }
  for (int a = 0; a != 2; a++) {
    for (int w = 0; w != 3; w++) {
      SamuraiState& ss = state.samuraiStates[a][w];
      if (&ss != this &&
	  ss.role->homeX == p->coords.x &&
	  ss.role->homeY == p->coords.y) {
	comments << "Moving to home of another samurai: "
		 << position->coords.toString();
	return false;
      }
    }
  }
  if (hidden)
    if (p->state < 0 || p->state/3 != role->id/3) {
      comments << "Hidden move to non-territory from: "
	       << position->coords.toString()
	       << " to " << p->coords.toString();
      return false;
    }
  if (dump) {
    *ds << "Samurai " << side << "." << weapon
	<< " moves from " << position->coords.toString()
	<< " to " << p->coords.toString() << endl;
  }
  position->leave(this);
  position = p;
  position->arrive(this);
  return true;
}
bool SamuraiState::occupy(GameState& state, int direction, ostream& comments) {
  if (hidden) {
    comments << "Hidden samurai tried occupation";
    return false;
  }
  state.battleField.occupy(state, *role, direction, *position);
  return true;
}
bool SamuraiState::hide(ostream& comments) {
  if (hidden) {
    comments << "Hidden samurai tries to hide itself further";
    return false;
  }
  assert(position->apparent == this);
  if (position->state < 0 || position->state/3 != role->id/3) {
    comments <<"Trying to hide itself at non-territory: ";
    return false;
  }
  hidden = true;
  position->apparent = 0;
  return true;
}
bool SamuraiState::appear(ostream& comments) {
  if (!hidden) {
    comments << "Non-hidden samurai tries to appear";
    return false;
  }
  if (position->apparent != 0) return false;
  hidden = false;
  position->apparent = this;
  return true;
}
void SamuraiState::cure() {
  if (curePeriod > 0) curePeriod -= 1;
}
void SamuraiState::house(BattleField& field) {
  Section* home = field.section(role->homeX, role->homeY);
  position->leave(this);
  hidden = false;
  home->arrive(this);
  position = home;
}
void SamuraiState::die(BattleField& field) {
  if (dump) {
    *ds << "Samurai " << side << "." << weapon << " disqualified" << endl;
  }
  house(field);
  alive = false;
}
void SamuraiState::injure(BattleField& field, Setting& setting) {
  if (dump) {
    *ds << "Samurai " << side << "." << weapon << " injured" << endl;
  }
  house(field);
  curePeriod = setting.cureTurns;
}


void readResponse(SamuraiState& ss) {
  ss.fromAI >> ss.response;
  ss.done = true;
}

GameState::GameState(Setting& stng, int players[]):
  setting(stng),
  battleField(BattleField(setting.width, setting.height)) {
  for (int side = 0; side != 2; side++) {
    vector<Role>& roles = setting.roles[side];
    unsigned int armySize = roles.size();
    samuraiStates[side] = new SamuraiState[armySize];
    for (unsigned int s = 0; s != armySize; s++) {
      SamuraiState& ss = samuraiStates[side][s];
      ss.init(setting, battleField, players[side*armySize+s], side, s);
    }
  }
  for (int side = 0; side != 2; side++) {
    vector<Role>& roles = setting.roles[side];
    unsigned int armySize = roles.size();
    for (unsigned int s = 0; s != armySize; s++) {
      SamuraiState& ss = samuraiStates[side][s];
      sendGameInfo(*ss.toAI, side, s);
      if (logif) {
	sendGameInfo(ifs[3*side+s], side, s);
      }
      ss.response = -1;
      ss.done = false;
      thread* readerThread = new thread(readResponse, ref(ss));
      chrono::system_clock::time_point till =
	chrono::system_clock::now() +
	chrono::milliseconds(setting.timeAllowed);
      while (!ss.done) {
	this_thread::sleep_for(chrono::milliseconds(1));
	chrono::system_clock::time_point now = 
	  chrono::system_clock::now();
	if (now >= till) break;
      }
      ss.alive = ss.done;
      if (!ss.alive) {
	cerr << "Time out at initiation" << endl;
	readerThread->detach();
	kill(ss.processId, SIGKILL);
      } else {
	readerThread->join();
	delete readerThread;
	if (ss.response != 0) {
	  cerr << "Wrong response at initiation" << endl;
	  ss.alive = false;
	}
      }
    }
  }
  turn = 0;
}

void GameState::sendGameInfo(ostream& os, const int side, const int weapon) {
  os << "# <# turns> <side> <weapon> <width> <height> <cure period>\n"
     << setting.turns << ' ' << side << ' ' << weapon << ' '
     << battleField.width << ' ' << battleField.height << ' '
     << setting.cureTurns << endl;
  os << "# Home positions\n";
  for (int a = 0; a != 2; a++) {
    for (int w = 0; w != 3; w++) {
      os << setting.roles[(a+side)%2][w].homeX << ' '
	 << setting.roles[(a+side)%2][w].homeY << endl;
    }
  }
  os << "# Ranks and scores of samurai\n";
  for (int a = 0; a != 2; a++) {
    for (int w = 0; w != 3; w++) {
      SamuraiState& ss = samuraiStates[(a+side)%2][w];
      os << ss.samurai->rank << ' ' << ss.samurai->score << endl;
    }
  }
  os.flush();
}

void GameState::sendTurnInfo(ostream& os, const int side, const int weapon) {
  SamuraiState ss = samuraiStates[side][weapon];
  if (ss.alive) {
    os << "# Turn information for samurai of side " << side
       << " with weapon " << weapon << endl;
    os << "# <turn>\n" << turn << endl;
    os << "# <cure period>\n" << ss.curePeriod << endl;
    const int width = battleField.width;
    const int height = battleField.height;
    bool visible[width*height] = {};
    for (int weapon = 0; weapon != 3; weapon++) {
      const int v = setting.roles[side][weapon].vision;
      const Section& s = *samuraiStates[side][weapon].position;
      int x0 = s.coords.x;
      int y0 = s.coords.y;
      for (int dy = -v; dy <= v; dy++) {
	int y = y0+dy;
	if (0 <= y && y < height) {
	  int v1 = v-abs(dy);
	  for (int dx = -v1; dx <= v1; dx++) {
	    int x = x0+dx;
	    if (0 <= x && x < width) {
	      visible[y*width+x] = true;
	    }
	  }
	}
      }
    }
    os << "# Samurai states: <position x> <position y> <hiding>\n";
    for (int a = 0; a != 2; a++) {
      for (int w = 0; w != 3; w++) {
	SamuraiState& ss = samuraiStates[(a+side)%2][w];
	int x = ss.position->coords.x;
	int y = ss.position->coords.y;
	if (a == 0 || (visible[y*width+x] && !ss.hidden)) {
	  os << x << ' ' << y << ' ' << (ss.alive ?  (ss.hidden ? 1 : 0)  : -1) << endl;
	} else {
	  os << -1 << ' ' << -1 << ' ' <<  (ss.alive ? 1 : -1) << endl;
	}
      }
    }
    os << "# Battle field states\n";
    for (int y = 0; y != height; y++) {
      for (int x = 0; x != width; x++) {
	if (visible[y*width+x]) {
	  Section* sect = battleField.map.at(Coordinates(x, y));
	  int sectState =
	    (sect->state == -1 ? 8 :
	     (side == 0 ? sect->state :
	      (sect->state >= 3 ? sect->state -3 :
	       sect->state + 3)));
	  os << ' ' << sectState;
	} else {
	  os << ' ' << 9;
	}
      }
      os << '\n';
    }
    os.flush();
  }
}

void readActions(SamuraiState& ss) {
  while (!ss.actions.empty()) {
    ss.actions.pop();
  }
  while (true) {
    int action;
    ss.fromAI >> action;
    if (!ss.fromAI.good()) {
      ss.alive = false;
      break;
    }
    if (action == 0) break;
    ss.actions.push(action);
    if (dump) *ds << ' ' << action;
  }
  if (dump) { *ds << endl; ds->flush(); }
  ss.done = true;
}

void GameState::receiveActionCommands
(CommentedIStream& is, const int side, const int weapon, ostream& log) {
  log << "# Turn " << turn << '/' << setting.turns << endl;
  log << "# <samurai id>\n" << side*3+weapon << endl;
  log << "# <think time> <used time>\n"
      << setting.timeAllowed << ' ' << 0 << endl;
  ostringstream turnComments;
  SamuraiState& ss = samuraiStates[side][weapon];
  if (ss.alive) {
    ss.done = false;
    thread* readerThread = new thread(readActions, ref(ss));
    chrono::system_clock::time_point till =
      chrono::system_clock::now() +
      chrono::milliseconds(setting.timeAllowed);
    while (!ss.done) {
      this_thread::sleep_for(chrono::milliseconds(1));
      chrono::system_clock::time_point now = 
	chrono::system_clock::now();
      if (now >= till) break;
    }
    if (!ss.done) {
      cerr << "Timed out @ turn " << turn << endl;
      turnComments << "Timed out";
      readerThread->detach();
      kill(ss.processId, SIGKILL);
      goto DEAD;
    }
    readerThread->join();
    delete readerThread;
    log << "# Actions\n";
    {
      int power = ss.role->activity;
      if (!ss.actions.empty() && ss.curePeriod != 0) {
	turnComments << "Trying to act under recovery";
	goto FINISH;
      }
      while (!ss.actions.empty()) {
	int action = ss.actions.front();
	if (action < 0 || action > 10) {
	  turnComments << "Invalid action specified: " << action;
	  goto FINISH;
	} else {
	  static int required[] = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1}; 
	  power -= required[action];
	  if (power < 0) {
	    turnComments << "Action limit exceeded";
	    goto FINISH;
	  }
	  if (dump) {
	    *ds << "Samurai " << ss.side << "." << ss.weapon
		<< " performs action: " << action << endl;
	  }
	  ss.actions.pop();
	  if (action <= 4) {
	    if (!ss.occupy(*this, action-1, turnComments)) goto FINISH;
	  } else if (action <= 8) {
	    if (!ss.move(*this, action-5, turnComments)) goto FINISH;
	  } else if (action == 9) {
	    if (!ss.hide(turnComments)) goto FINISH;
	  } else if (action == 10) {
	    if (!ss.appear(turnComments)) goto FINISH;
	  }
	  log << action << ' ';
	}
      }
    }
  FINISH:
    log << '0' << endl
	<< "# Comments" << endl
	<< "\"" << turnComments.str() << "\"" << endl;
    return;
  } else {
    turnComments << "Disqualified";
  }
 DEAD:
  ss.die(battleField);
  log << "-1" << endl
      << "# Comments" << endl
      << "\"" << turnComments.str() << "\"" << endl;
  return;
}

ostream& operator<<(ostream& os, const Setting& setting) {
  cout << "Game Name: " << setting.gameName << endl
       << "Turns: " << setting.turns << endl
       << "Time Allowed: " << setting.timeAllowed << endl
       << "Field Size: " << setting.width << " x " << setting.height << endl
       << "Samurai Set:" << endl
       << setting.samuraiDB;
  for (int a = 0; a != 2; a++) {
    cout << "Army " << a << endl;
    for (auto&& role: setting.roles[a]) {
      cout << role;
    }
  }
  return os;
}

ostream& operator<<(ostream& os, const Role& role) {
  os << "Reach:";
  for (auto&& c: role.reach) {
    os << " " << c.toString();
  }
  os << endl;
  os << "Vision: " << role.vision << "; "
     << "Activity: " << role.activity << "; "
     << "Home: " << Coordinates(role.homeX, role.homeY).toString() << ";"
     << endl;
  return os;
}
