#include "players.hpp"
#include <unistd.h>
#include <cstdlib>

bool logging = false;

CommentedIStream::CommentedIStream(istream &is):
is(&is) {
}

int CommentedIStream::get() {
    if (is->eof()) exit(0);
    return is->get();
}

void CommentedIStream::skipComments() {
    char c = get();
    while (isspace(c) || c == commentChar) {
        if (c == commentChar) {
            do {
                c = get();
            } while (c != '\n');
        } else {
            c = get();
        }
    }
    is->unget();
}

CommentedIStream& operator>>(CommentedIStream &cs, int &i) {
    cs.skipComments();
    *cs.is >> i;
    return cs;
}

void SamuraiInfo::homePosition(CommentedIStream& is) {
    is >> homeX >> homeY;
}

void SamuraiInfo::readScoreInfo(CommentedIStream& is) {
    is >> rank >> score;
}

void SamuraiInfo::readTurnInfo(CommentedIStream& is) {
    int state;
    is >> curX >> curY >> state;
    alive = (state >= 0);
    hidden = (alive ? state : 0);
}

GameInfo::GameInfo(CommentedIStream& is) {
    is >> turns >> side >> weapon
        >> width >> height >> cureTurns;
    for (SamuraiInfo& s: samuraiInfo) {
        s.homePosition(is);
    }
    for (SamuraiInfo& s: samuraiInfo) {
        s.readScoreInfo(is);
    }
    cout << 0 << endl;
}

void GameInfo::readTurnInfo(CommentedIStream& is) {
    is >> turn;
    if (turn < 0) exit(0);
    is >> curePeriod;
    if (logging) clog << "Samurai positions before turn " << turn;
    for (SamuraiInfo& s: samuraiInfo) {
        s.readTurnInfo(is);
        if (logging) clog << " (" << s.curX << "," << s.curY << ")"
            << (s.hidden!=0 ? "*" : "");
    }
    if (logging) clog << endl;
    field = new int[width*height];
    for (int y = 0; y != height; y++) {
        for (int x = 0; x != width; x++) {
            is >> field[y*width+x];
        }
    }
}

bool GameInfo::isValid(const int action) const {
    const SamuraiInfo& myself = samuraiInfo[weapon];
    return isValidAt(action, myself.curX, myself.curY, myself.hidden);
}

bool GameInfo::isValidAt
(const int action, const int curX, const int curY, const int hidden) const {
    if (logging) {
        clog << "Checking " << action << ", samurai@:";
        for (int s = 0; s != 6; s++) {
            clog << " (" << samuraiInfo[s].curX << "," 
                << samuraiInfo[s].curY << ")"
                << (samuraiInfo[s].hidden!=0 ? "*" : "");
        }
        clog << endl;
    }
    switch (action) {
        case 1: case 2: case 3: case 4:
            if (hidden == 0 && curePeriod == 0) {
                if (logging) {
                    clog << "Action of " << (hidden ? "hidden " : "")
                        << "samurai " << weapon << ": "
                        << action << endl;
                }
                return true;
            } else {
                return false;
            }
        case 5: case 6: case 7: case 8: {
            int x = curX;
            int y = curY;
            switch (action) {
                case 5: y += 1; break;
                case 6: x += 1; break;
                case 7: y -= 1; break;
                case 8: x -= 1; break;
            }
            if (x < 0 || width <= x || y < 0 || height <= y) return false;
            if (hidden != 0 && field[y*width+x] >= 3) return false;
            for (int s = 0; s != 6; s++) {
                const SamuraiInfo& si = samuraiInfo[s];
                if (hidden == 0 && si.hidden == 0 &&
                        x == si.curX && y == si.curY)
                    return false;
                if (s != weapon && x == si.homeX && y == si.homeY)
                    return false;
            }
            if (logging) {
                clog << "Action of " << (hidden!=0 ? "hidden " : "")
                    << "samurai " << weapon << ": "
                    << action << " from (" << curX << "," << curY << ") to ("
                    << x << "," << y << ")" << endl;
            }
            return true;
        }
        case 9:
            if (hidden!=0) return false;
            if (field[curY*width+curX] >= 3) return false;
            if (logging) {
                clog << "Action of " << (hidden!=0 ? "hidden " : "")
                    << "samurai " << weapon << ": "
                    << action << endl;
            }
            return true;
        case 10:
            if (hidden==0) return false;
            for (int s = 0; s != 6; s++) {
                if (s != weapon &&
                        samuraiInfo[s].curX == curX && samuraiInfo[s].curY == curY) 
                    return false;
            }
            if (logging) {
                clog << "Action of " << (hidden!=0 ? "hidden " : "")
                    << "samurai " << weapon << ": "
                    << action << endl;
            }
            return true;
        default:
            cerr << "Invalid action " << action << " tried" << endl;
            exit(1);
    }
}

void rotate(int direction, int x0, int y0, int& x, int& y) {
    switch (direction) {
        case 0: x = x0; y = y0; break;
        case 1: x = y0; y = -x0; break;
        case 2: x = -x0; y = -y0; break;
        case 3: x = -y0; y = x0; break;
    }
}

void GameInfo::occupy(int direction) {
    static const int size[3] = {4, 5, 7};
    static const int ox[3][7] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 2, 0, 0},
        {-1,-1,-1,0, 1, 1, 1}};
    static const int oy[3][7] = {
        {1, 2, 3, 4},
        {1, 2, 0, 1, 0},
        {-1,-1,1, 1, 1,-1, 0}}; 
    SamuraiInfo& myself = samuraiInfo[weapon];
    for (int k = 0; k != size[weapon]; k++) {
        int x, y;
        rotate(direction, ox[weapon][k], oy[weapon][k], x, y);
        x += myself.curX;
        y += myself.curY;
        if (0 <= x && x < width && 0 <= y && y < height) {
            field[y*width+x] = weapon;
            for (int s = 3; s != 6; s++) {
                SamuraiInfo& si = samuraiInfo[s];
                if (si.curX == x && si.curY == y) {
                    si.curX = si.homeX;
                    si.curY = si.homeY;
                    si.hidden = 0;
                }
            }
        }
    }
}

FieldUndo::FieldUndo(int* sect): section(sect), state(*section) {}
void FieldUndo::apply() { *section = state; }

SamuraiUndo::SamuraiUndo(SamuraiInfo* si): si(si) {
    x = si->curX; y = si->curY; hidden = si->hidden;
}
void SamuraiUndo::apply() { si->curX = x; si->curY = y; si->hidden = hidden; }

void Undo::recField(int* sect) { fieldUndo.emplace_front(sect); }
void Undo::recSamurai(SamuraiInfo* si) { samuraiUndo.emplace_front(si); }
void Undo::apply() {
    for (FieldUndo& u: fieldUndo) u.apply();
    for (SamuraiUndo& u: samuraiUndo) u.apply();
}

void GameInfo::tryAction(int action, Undo& undo,  int& territory, int& selfTerritory, int& injury, int& hiding, int& avoiding, int& moving, int myTern, int enemyMemory[100], int myfield[2], int& doubleAction) {
    SamuraiInfo& me = samuraiInfo[weapon];
    territory = selfTerritory = injury = hiding = avoiding = moving = doubleAction = 0;
    switch (action) {
        case 1: case 2: case 3: case 4: { // occupation
            static const int size[3] = {4, 5, 7};
            static const int ox[3][7] = {
                {0, 0, 0, 0, 0, 0, 0},
                {0, 0, 1, 1, 2, 0, 0},
                {-1,-1,-1,0,1,1,1}};
            static const int oy[3][7] = {
                {1, 2, 3, 4},
                {1, 2, 0, 1, 0},
                {-1,0,1,1,-1,0,1}};
            for (int k = 0; k != size[weapon]; k++) {
                int x, y;
                rotate(action-1, ox[weapon][k], oy[weapon][k], x, y);
                x += me.curX; y += me.curY;
                if (0 <= x && x < width && 0 <= y && y < height) {
                    bool isHome = false;
                    for (int s = 0; s != 6; s++) {
                        SamuraiInfo& si = samuraiInfo[s];
                        if (si.homeX == x && si.homeY == y) {
                            // Cannot occupy home positions
                            isHome = true;
                            break;
                        }
                    }
                    if (!isHome) {
                        int pos = y*width+x;
                        int current = field[pos];
                        if (current != weapon) {
                            selfTerritory += 1;
                            if (current >= 8) { // unoccupied
                                territory += 1;
                            } else if (current >= 3) { // opponents' territory
                                territory += 2;
                            }
                            undo.recField(&field[pos]);
                        }
                        for (int s = 3; s != 6; s++) {
                            SamuraiInfo& si = samuraiInfo[s];
                            if (si.curX == x && si.curY == y) {
                                undo.recSamurai(&si);
                                si.curX = si.homeX;
                                si.curY = si.homeY;
                                injury++;
                                si.hidden = 0;
                            }
                        }

                    }
                }
            }
            break;
        }
        case 5: case 6: case 7: case 8: {
            static const int dx[] = { 0, 1, 0, -1 };
            static const int dy[] = { 1, 0, -1, 0 };
            int distance=0, beforeDistance=0, afterDistance=0;
            undo.recSamurai(&me);
            int oldX = me.curX;
            int oldY = me.curY;
            me.curX += dx[action-5];
            me.curY += dy[action-5];

            beforeDistance = abs(oldX-myfield[0]) + abs(oldY-myfield[1]); 
            afterDistance = abs(me.curX-myfield[0]) + abs(me.curY-myfield[1]);
            distance = beforeDistance - afterDistance;
            moving += distance;

            bool preInEnemyTerritory = false, nowInEnemyTerritory = false;
            // doubleActionの時(剣と斧のみ）

            int samuraiID = weapon+side*3;
            if(samuraiID%2 == myTern%2) {
                if(weapon%3 == 1) { // sword
                    SamuraiInfo& si = samuraiInfo[4];
                    if(si.curX!=-1 && si.curY!=-1 && si.curX != si.homeX && si.curY != si.homeY){
                        preInEnemyTerritory = isEnemyTerritory(oldX, oldY, 4, samuraiInfo[4]);
                        nowInEnemyTerritory = isEnemyTerritory(me.curX, me.curY, 4, samuraiInfo[4]);
                    }
                } else if(weapon%3 == 2) { //ax
                    SamuraiInfo& si = samuraiInfo[5];
                    if(si.curX!=-1 && si.curY!=-1 && si.curX != si.homeX && si.curY != si.homeY){
                        preInEnemyTerritory = isEnemyTerritory(oldX, oldY, 5, samuraiInfo[5]);
                        nowInEnemyTerritory = isEnemyTerritory(me.curX, me.curY, 5, samuraiInfo[5]);
                    }
                } else if(weapon%3 == 0) { // spir
                    SamuraiInfo& si = samuraiInfo[3];
                    if(si.curX!=-1 && si.curY!=-1 && si.curX != si.homeX && si.curY != si.homeY){
                        preInEnemyTerritory = isEnemyTerritory(oldX, oldY, 3, samuraiInfo[3]);
                        nowInEnemyTerritory = isEnemyTerritory(me.curX, me.curY, 3, samuraiInfo[3]);
                    }
                }
            }
            if(preInEnemyTerritory == false && nowInEnemyTerritory == true) {
                doubleAction += 1;
            }else if(preInEnemyTerritory == true && nowInEnemyTerritory == false) {
                doubleAction -= 1;
            }
            break;
        }
        case 9: // hide
            undo.recSamurai(&me);
            me.hidden = 1;
            if(enemyMemory[myTern] >= 1) {
                hiding += 2;
            }
            if(enemyMemory[myTern-1] >= 1) {
                hiding += 1;
            }
            hiding += 1;
            break;
        case 10: // appear
            undo.recSamurai(&me);
            me.hidden = 0;
            if(enemyMemory[myTern] >= 1) {
                hiding -= 2;
            }
            if(enemyMemory[myTern-1] >= 1) {
                hiding -= 1;
            }
            hiding -= 1;
            break;
    }
    
    //check enemy attack area for avoiding
    //bool 
    bool isDanger=false;
    for (int s = 3; s != 6; s++) {
        SamuraiInfo& si = samuraiInfo[s];
        if(si.curX==-1&&si.curY==-1)continue;
        int diffx=abs(me.curX-si.curX);
        int diffy=abs(me.curY-si.curY);
        //axe
        if(s==5 && (diffx<=2 && diffy<=2) && diffx+diffy!=4 ){
            isDanger=true;
        }
        //sword
        if(s==4 && diffx+diffy<=3){
            isDanger=true;
        }
        //spear
        if(s==3 && diffx+diffy<=5 ){
            if(diffx==2&&diffy==2)continue;
            if(diffx==2&&diffy==3)continue;
            if(diffx==3&&diffy==2)continue;
            isDanger=true;
        }
    }
    if(isDanger){
        avoiding++;
    }


}
bool GameInfo::isEnemyTerritory(int meX, int meY, int enemyID, SamuraiInfo& si) {
    int diffx=abs(meX-si.curX);
    int diffy=abs(meY-si.curY);
    if(enemyID==4 && diffx+diffy<=3){
        return true;
    }
    if(enemyID==5 && (diffx<=2 && diffy<=2) && diffx+diffy!=4 ){
        return true;
    }
    if(enemyID==3 && diffx+diffy<=5 ){
        if(diffx==2&&diffy==2) return false;
        if(diffx==2&&diffy==3) return false;
        if(diffx==3&&diffy==2) return false;
        return true;
    }
    return false;
}

void GameInfo::doAction(int action) {
    Undo dummy;
    int dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy10;
    int dummy8[100] = {};
    int dummy9[2] = {};
    tryAction(action, dummy, dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8, dummy9, dummy10);
    cout << action << ' ';
}


int main(int argc, char* argv[]) {
    if (argc >= 2 && strcmp(argv[1],"-d")==0) {
        logging = true;
    }
    CommentedIStream in(cin);
    GameInfo info(in);
    while (true) {
        info.readTurnInfo(in);
        cout << "# Turn " << info.turn << endl;
        if (info.curePeriod != 0) {
            cout << "0" << endl;
        } else {
            player->play(info);
            cout << "0\n";
        }
        if (logging) clog.flush();
        cout.flush();
    }
}
