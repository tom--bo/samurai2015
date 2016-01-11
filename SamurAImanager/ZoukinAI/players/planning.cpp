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
int myTern = 0;

struct PlanningPlayer: Player {
    void plan(GameInfo& info, SamuraiInfo& me, int power, double merits, int myTern, int enemyMemory[100]) {
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
                info.tryAction(action, undo, territory, selfTerritory, injury, hiding, avoiding, moving, myTern, enemyMemory);
                double gain = territoryMerits*territory
                    + selfTerritoryMerits*selfTerritory
                    + hurtingMerits*injury
                    + hidingMerits*hiding
                    + avoidingMerits*avoiding
                    + movingMerits*moving;
                plan(info, me, power-required[action], merits+gain, myTern, enemyMemory);
                undo.apply();
                currentPlay.pop_back();
            }
        }
    }
    void play(GameInfo& info) {
        currentPlay.clear();
        bestMerits = -1;
        myTern += 1;
        SamuraiInfo& me = info.samuraiInfo[info.weapon];
        for (int s = 3; s != 6; s++) {
            SamuraiInfo& si = info.samuraiInfo[s];
            if((si.curX != -1 && si.curY != -1 && si.curX != si.homeX && si.curY != si.homeY) && abs(me.curX-si.curX)+abs(me.curY-si.curY)<=5){
                enemyMemory[myTern] += 1;
            }
        }
        plan(info, info.samuraiInfo[info.weapon], 7, 0, myTern, enemyMemory);
        for (int action: bestPlay) {
            cout << action << ' ';
        }
    }
};

Player* player = new PlanningPlayer();
