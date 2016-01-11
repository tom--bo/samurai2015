#include "players.hpp"
#include <list>
#include <utility>

extern double territoryMerits;
extern double selfTerritoryMerits;
extern double hurtingMerits;
extern double hidingMerits;
extern double avoidingMerits;
extern double movingMerits;

list<int> bestPlay;
list<int> currentPlay;
double bestMerits;
int enemyMemory[100] = {};
int myfield[2] = {};
int myTern = 0;

struct PlanningPlayer: Player {
    void plan(GameInfo& info, SamuraiInfo& me, int power, double merits, int myTern, int enemyMemory[100], int myfleld[2]) {
        if (merits > bestMerits) {
            bestMerits = merits;
            bestPlay = currentPlay;
        } else if (merits == bestMerits) {
            if((int)(merits*3) % 2 == 0) {
                bestPlay = currentPlay;
            }
        }
        static const int required[] = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1}; 
        for (int action = 1; action != 11; action++) {
            if (required[action] <= power && info.isValidAt(action, me.curX, me.curY, me.hidden)) {
                currentPlay.push_back(action);
                Undo undo;
                int territory, selfTerritory, injury, hiding, avoiding, moving;
                info.tryAction(action, undo, territory, selfTerritory, injury, hiding, avoiding, moving, myTern, enemyMemory, myfield);
                double gain = territoryMerits*territory
                    + selfTerritoryMerits*selfTerritory
                    + hurtingMerits*injury
                    + hidingMerits*hiding
                    + avoidingMerits*avoiding
                    + movingMerits*moving;
                plan(info, me, power-required[action], merits+gain, myTern, enemyMemory, myfield);
                undo.apply();
                currentPlay.pop_back();
            }
        }
    }
    void play(GameInfo& info) {
        currentPlay.clear();
        updateMyField(info);
        setMerits(info);
        bestMerits = -1;
        myTern += 1;
        SamuraiInfo& me = info.samuraiInfo[info.weapon];
        for (int s = 3; s != 6; s++) {
            SamuraiInfo& si = info.samuraiInfo[s];
            if((si.curX != -1 && si.curY != -1 ) && abs(me.curX-si.curX)+abs(me.curY-si.curY)<=5){
                enemyMemory[myTern] += 1;
            }
        }
        plan(info, info.samuraiInfo[info.weapon], 7, 0, myTern, enemyMemory, myfield);
        for (int action: bestPlay) {
            cout << action << ' ';
        }
    }
    void updateMyField(GameInfo& info){
        int tmpField[15][15] = {};
        int tmp[3][3] = {};
        for(int i=0; i<225; i++){
            int y = i/15;
            int x = i%15;
            if(info.field[i] < 3) {
                tmpField[x][y] = 1;
            }else if(info.field[i] < 6) {
                tmpField[x][y] = 1;
            }
        }
        int minCost = 10;
        int minX, minY;
        for(int i=0; i<3; i++) {
            for(int j=0; j<3; j++) {
                for(int k=0; k<5; k++) {
                    for(int l=0; l<5; l++) {
                        tmp[i][j] += tmpField[i*5+k][j*5+l];
                    }
                }
                if(minCost > tmp[i][j]) {
                    minCost = tmp[i][j];
                    myfield[0] = i*3+2;
                    myfield[1] = j*3+2;
                }
            }
        }
    }
    void setMerits(GameInfo& info){
        switch(info.weapon){
            case 0:
                territoryMerits = 1;
                selfTerritoryMerits = 0.1;
                hurtingMerits = 100;
                hidingMerits = 0.2;
                avoidingMerits = -1;
                movingMerits = 1;               
                break;
            case 1:
                territoryMerits = 2;
                selfTerritoryMerits = 0.1;
                hurtingMerits = 100;
                hidingMerits = 2;
                avoidingMerits = -5;
                movingMerits = 0.5;               
                
                break;
            case 2:
                territoryMerits = 1;
                selfTerritoryMerits = 0.1;
                hurtingMerits = 100;
                hidingMerits = 1;
                avoidingMerits = -4;
                movingMerits = 0.1;               
                break;
        }
    }
};

Player* player = new PlanningPlayer();
