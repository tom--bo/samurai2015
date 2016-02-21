#include "players.hpp"
#include <unistd.h>
#include <cstdlib>
#define DANGER_THRESHOLD 0.33

bool logging = false;
int reborn[3] = {0, 0, 0};

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

void GameInfo::occupy(int action) {
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
        SamuraiInfo& me = samuraiInfo[weapon];
        rotate(action-1, ox[weapon][k], oy[weapon][k], x, y);
        x += me.curX;
        y += me.curY;
        if (0 <= x && x < width && 0 <= y && y < height) {
            field[y*width+x] = weapon;
            for (int s = 3; s != 6; s++) {
                SamuraiInfo& si = samuraiInfo[s];
                if (si.curX == x && si.curY == y) {
                    si.curX = si.homeX;
                    si.curY = si.homeY;
                    si.hidden = 0;
                    reborn[s-3] = turn+cureTurns;
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

int GameInfo::getPointAroundHome(int x, int y) {
    SamuraiInfo& me = samuraiInfo[weapon];
    int distanceFromHome = abs(me.homeX-x) + abs(me.homeY-y);
    int pointMap0[4][4] = {
        {0,2,2,1},
        {2,1,0,0},
        {2,1,0,0},
        {2,1,0,0}
    };
    int pointMap1[5][5] = {
        {0,2,2,2,2},
        {2,1,1,1,0},
        {2,1,1,0,0},
        {2,1,0,0,0},
        {2,0,0,0,0}
    };
    int pointMap2[4][4] = {
        {0,2,2,2},
        {2,2,2,2},
        {2,2,2,0},
        {2,2,2,0}
    };

    if(weapon%3 == 0) {
        if(abs(me.homeX-x) <= 3 && abs(me.homeY-y) <= 3) {
            return pointMap0[abs(me.homeY-y)][abs(me.homeX-x)];
        }
    }else if(weapon%3 == 1) {
        if(abs(me.homeX-x) <= 4 && abs(me.homeY-y) <= 4) {
            return pointMap1[abs(me.homeY-y)][abs(me.homeX-x)];
        }
    }else if(weapon%3 == 2) {
        if(abs(me.homeX-x) <= 3 && abs(me.homeY-y) <= 3) {
            int dir = (me.homeX-samuraiInfo[1].homeX)/abs(me.homeX-samuraiInfo[1].homeX);
            if((x-me.homeX)*dir>0) {
                return pointMap2[abs(me.homeY-y)][abs(me.homeX-x)]*3/2;
            }
            return pointMap2[abs(me.homeY-y)][abs(me.homeX-x)];
        }
    }
    return 0;
}

void GameInfo::tryAction(int action, Undo& undo,  int& enemyTerritory, int& blankTerritory, int& friendTerritory, int& injury, int& hiding, int& avoiding, int& moving, int& center, int turn, int enemyMemory[100], int myfield[2], int& doubleAction, int dangerMap[15][15], int& danger, int& assassin, int& respawn, int& venture, int& isolate) {
    SamuraiInfo& me = samuraiInfo[weapon];
    enemyTerritory = blankTerritory = friendTerritory = injury = hiding = avoiding = moving = assassin = center = doubleAction = danger = respawn= venture = isolate = 0;
    switch (action) {
        case 1: case 2: case 3: case 4: { // occupation
            static const int aroundHomePoint = 3;
            static const int size[3] = {4, 5, 7};
            static const int ox[3][7] = {
                {0, 0, 0, 0, 0, 0, 0},
                {0, 0, 1, 1, 2, 0, 0},
                {-1,-1,-1,0,1,1,1}};
            static const int oy[3][7] = {
                {1, 2, 3, 4},
                {1, 2, 0, 1, 0},
                {-1,0,1,1,-1,0,1}};
            if(me.curX==me.homeX&&me.curY==me.homeY){
                respawn=1;
            }
            for (int k = 0; k != size[weapon]; k++) {
                int x, y, distanceFromHome = 0;
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
                            if (current >= 8) { // unoccupied
                                blankTerritory++;
                                blankTerritory += getPointAroundHome(x, y);
                            } else if (current < 3) { // friends' territory
                                friendTerritory++;
                                friendTerritory += getPointAroundHome(x, y);
                            } else if (current >= 3) { // opponents' territory
                                enemyTerritory++;
                                enemyTerritory += getPointAroundHome(x, y);
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

            //isolate
            for(int s=0;s<3;s++){
                if(s==weapon)continue;
                int sum=0;
                for(int k=0; k<4; k++) {
                    sum += beforeFriendAction[s][k];
                }
                SamuraiInfo& si = samuraiInfo[s];
                beforeDistance = abs(si.curX-oldX) + abs(si.curY-oldY); 
                afterDistance = abs(si.curX-me.curX) + abs(si.curY-me.curY); 
                if(afterDistance<=5 && sum>=8){
                    distance = afterDistance - beforeDistance;
                    isolate+=distance;
                }
            } 


            bool preInEnemyTerritory = false, nowInEnemyTerritory = false;

            int samuraiID = weapon+side*3;
            if(weapon%3 == 1 && ((turn%12 == 3) || (turn%12 == 9))) { // sword
                SamuraiInfo& si = samuraiInfo[4];
                if(si.curX!=-1 && si.curY!=-1 && si.curX != si.homeX && si.curY != si.homeY){
                    preInEnemyTerritory = isEnemyTerritory(oldX, oldY, 4, samuraiInfo[4].curX, samuraiInfo[4].curY);
                    nowInEnemyTerritory = isEnemyTerritory(me.curX, me.curY, 4, samuraiInfo[4].curX, samuraiInfo[4].curY);
                }
            } else if(weapon%3 == 2 && ((turn%12 == 5) || (turn%12 == 11))) { // ax 
                SamuraiInfo& si = samuraiInfo[5];
                if(si.curX!=-1 && si.curY!=-1 && si.curX != si.homeX && si.curY != si.homeY){
                    preInEnemyTerritory = isEnemyTerritory(oldX, oldY, 5, samuraiInfo[5].curX, samuraiInfo[5].curY);
                    nowInEnemyTerritory = isEnemyTerritory(me.curX, me.curY, 5, samuraiInfo[5].curX, samuraiInfo[5].curY);
                }
            } else if(weapon%3 == 0 && ((turn%12 == 1) || (turn%12 == 7))) { // spir
                SamuraiInfo& si = samuraiInfo[3];
                if(si.curX!=-1 && si.curY!=-1 && si.curX != si.homeX && si.curY != si.homeY){
                    preInEnemyTerritory = isEnemyTerritory(oldX, oldY, 3, samuraiInfo[3].curX, samuraiInfo[3].curY);
                    nowInEnemyTerritory = isEnemyTerritory(me.curX, me.curY, 3, samuraiInfo[3].curX, samuraiInfo[3].curY);
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
            if(enemyMemory[turn/6] >= 1) {
                hiding += 2;
            }
            if(turn/6 > 0 && enemyMemory[turn/6-1] >= 1) {
                hiding += 1;
            }
            hiding += 1;
            break;
        case 10: // appear
            undo.recSamurai(&me);
            me.hidden = 0;
            if(enemyMemory[turn/6] >= 1) {
                hiding -= 2;
            }
            if(turn/6 > 0 && enemyMemory[turn/6-1] >= 1) {
                hiding -= 1;
            }
            hiding -= 1;
            break;
    }
    
    //check enemy attack area for avoiding
    //bool 
    for (int s = 3; s != 6; s++) {
        SamuraiInfo& si = samuraiInfo[s];
        if(si.curX==-1&&si.curY==-1)continue;
        if( (reborn[s-3]-turn)>6) {
            continue;
        }
        int diffx=abs(me.curX-si.curX);
        int diffy=abs(me.curY-si.curY);
        //axe
        if(s==5 && (diffx<=2 && diffy<=2) && diffx+diffy!=4 ){
            avoiding--;
        }
        //sword
        if(s==4 && diffx+diffy<=3){
            avoiding--;
        }
        //spear
        if(s==3 && diffx+diffy<=5 ){
            if(diffx==2&&diffy==2)continue;
            if(diffx==2&&diffy==3)continue;
            if(diffx==3&&diffy==2)continue;
            avoiding--;
        }
    }

    //dangerMap
    int max=0;
    for(int j=0;j<225;j++){
        int x=j%15;
        int y=j/15;
        if(dangerMap[y][x]>max){
            max=dangerMap[y][x];
        }
    }
    if(dangerMap[me.curY][me.curX]>0){
        if(dangerMap[me.curY][me.curX]>max*DANGER_THRESHOLD){
            danger--;
        }else if(me.hidden==0){
            danger--;
        }
    }

    //assasin
    for(int s=3; s<=5; s++) {
        SamuraiInfo& si = samuraiInfo[s];
        if(si.curX != -1 && si.curY != -1) {
            int assassinPossibility=0;
            int list[4][2]={{1,0},{0,1},{-1,0},{0,-1}};
            for(auto diff:list){
                int dx=si.curX+diff[0];
                int dy=si.curY+diff[1];
                if(dx<0||dy<0||dx>14||dy>14)continue;
                if(isEnemyTerritory(me.curX,me.curY,weapon+3,dx,dy)){
                    assassinPossibility+=1;
                }
            }
            if(assassinPossibility>2){
                assassinPossibility=2;
            }
            if(si.curX == si.homeX && si.curY == si.homeY) {
                if((reborn[s-3]-turn) >= 0 && (reborn[s-3]-turn)<=6 ){
                    if(assassinPossibility>0 && me.hidden == 1) {
                        assassin+=assassinPossibility;
                    }
                }
            }else{
                if( assassinPossibility>0 && me.hidden == 1) {
                    assassin+=assassinPossibility;
                }
            }
        } 
    }

    //venture
    int massCount=0;
    int fillArea=0;
    for(int i=0;i<225;i++){
        int x=i%15;
        int y=i/15;
        if(x==me.curX&&y==me.curY)continue;
        if(abs(x-me.curX)<=1&&abs(y-me.curY)<=1){
            massCount+=1;
            if(field[i]>2){
                fillArea++;
            }
        }
    }
    if(fillArea>=4){
        venture=1;
    }

    // add by position (closeness from center)
    int diffx=abs(me.curX - 7);
    int diffy=abs(me.curY - 7);
    int diffCenter = diffy;
    if(diffx>diffy){
        diffCenter=diffx;
    }
    switch(diffCenter) {
        case 0:  center+=10; break;
        case 1:  center+=10; break;
        case 2:  center+=10; break;
        case 3:  center+=10; break;
        default:
                 center+=0; break;
    }
}
bool GameInfo::isEnemyTerritory(int meX, int meY, int enemyID, int enemyX, int enemyY) {
    int diffx=abs(meX-enemyX);
    int diffy=abs(meY-enemyY);
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
    int dummy0, dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8, dummy11, dummy13, dummy14, dummy15,dummy16, dummy17;
    int dummy9[100] = {};
    int dummy10[2] = {};
    int dummy12[15][15]={};
    tryAction(action, dummy, dummy0, dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8, dummy9, dummy10, dummy11, dummy12, dummy13, dummy14, dummy15, dummy16,dummy17);
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
            player->play(info, reborn);
            cout << "0\n";
        }
        if (logging) clog.flush();
        cout.flush();
    }
}
