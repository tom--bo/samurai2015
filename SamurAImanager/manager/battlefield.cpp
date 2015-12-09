#include "samurai.hpp"

Coordinates::Coordinates(const int x, const int y): x(x), y(y) {}
Coordinates Coordinates::north() const { return Coordinates(x, y-1); }
Coordinates Coordinates::south() const { return Coordinates(x, y+1); }
Coordinates Coordinates::east() const { return Coordinates(x+1, y); }
Coordinates Coordinates::west() const { return Coordinates(x-1, y); }
Coordinates Coordinates::operator+(const Coordinates c) const {
  return Coordinates(x+c.x, y+c.y);
}
bool Coordinates::operator==(const Coordinates another) const {
  return x == another.x && y == another.y;
}

size_t CoordHash::operator()(const Coordinates& c) const {
  return 1000*c.x + c.y;
}

Coordinates Coordinates::rotate(const int direction) const {
  switch (direction) {
  case 0:
    return Coordinates(x, y);
  case 1:
    return Coordinates(y, -x);
  case 2:
    return Coordinates(-x, -y);
  case 3:
    return Coordinates(-y, x);
  }
  throw ErrorReport("Invalid direction to rotate: " + direction);
}

string Coordinates::toString() const {
  ostringstream os;
  os << "(" << x << "," << y << ")";
  return os.str();
}

Section* FieldMap::locate(Coordinates p) {
  FieldMap::const_iterator i = find(p);
  return (i == end() ? 0 : i->second);
}

Section::Section(Coordinates coords):
  coords(coords), state(-1), population(0), apparent(0) {
}
void Section::setNeighbors(FieldMap& map) {
  FieldMap::const_iterator south, east, north, west;
  neighbors[0] = map.locate(coords.south());
  neighbors[1] = map.locate(coords.east());
  neighbors[2] = map.locate(coords.north());
  neighbors[3] = map.locate(coords.west());
}
void Section::leave(SamuraiState* s) {
  population -= 1;
  if (!s->hidden) {
    assert(apparent == s);
    apparent = 0;
  }
}
void Section::arrive(SamuraiState* s) {
  population += 1;
  if (!s->hidden) {
    if (apparent != 0)
      throw ErrorReport("Moving to an already filled section " 
			+ coords.toString());
    apparent = s;
  }
}
void Section::occupy(Role& role) {
  state = role.id;
}

BattleField::BattleField(int w, int h): width(w), height(h) {
  for (int x = 0; x != width; x++) {
    for (int y = 0; y != height; y++) {
      Coordinates coords(x, y);
      map[coords] = new Section(coords);
    }
  }
  for (auto pair: map) {
    pair.second->setNeighbors(map);
  }
}

Section* BattleField::section(int x, int y) {
  return map.at(Coordinates(x, y));
}

void BattleField::occupy
(GameState& state, Role& role, int direction, Section& pos) {
  const Coordinates& p = pos.coords;
  for (Coordinates& c: role.reach) {
    Section* section = map.locate(p+c.rotate(direction));
    if (section != 0) {
      if (section->population != 0) {
	int a = 1-role.id/3;
	for (int w = 0; w != 3; w++) {
	  SamuraiState& ss = state.samuraiStates[a][w];
	  if (ss.position == section) {
	    if (dump)
	      *ds << "Samurai " << ss.side << "." << ss.weapon 
		  << " injured" << endl;
	    ss.injure(state.battleField, state.setting);
	  }
	}
      }
      section->occupy(role);
      if (dump) {
	*ds << "Section " << section->coords.toString() 
	    << " occupied by " << role.id << endl;
      }
    }
  }
}
