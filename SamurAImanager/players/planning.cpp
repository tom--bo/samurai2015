#include "players.hpp"
#include <list>
#include <utility>
#include <fstream>
#include<sstream>
#include <iostream>
#include<iomanip>
#include <unistd.h>
extern double enemyTerritoryMerits;
extern double blankTerritoryMerits;
extern double friendTerritoryMerits;
extern double hurtingMerits;
extern double hidingMerits;
extern double avoidingMerits;
extern double movingMerits;
extern double centerMerits;
extern double doubleMerits;

extern void setMerits(int weaponid);

list<int> bestPlay;
list<int> currentPlay;
double bestMerits;
int enemyMemory[100] = {0};
int myfield[2] = {0};
int myTern = 0;

int oldMap[225];
int oldEnemyPostionX[3]={};
int oldEnemyPostionY[3]={};
#define enemyTerritoryMAX 5.0
#define blankTerritoryMAX 5.0
#define friendTerritoryMAX 5.0
#define injuryMAX 1.0
#define hidingMAX 1.0
#define avoidingMAX 1.0
#define movingMAX 1.0
#define doubleActionMAX 1.0
#define centerMAX 10.0

struct PlanningPlayer: Player {
    PlanningPlayer():Player(){
        for(int i=0;i<225;i++){
            oldMap[i]=8;
        }   
    
    }
    void printMap2(int pid,int turn,const char* title,int map[225]){
        ostringstream oss;
        oss<<"mylog/log"<<pid<<"-turn"<<turn;
        std::ofstream ofs(oss.str(), std::ios::app );
        ofs<<"#"<<title<<std::endl;
        for(int y=0;y<15;y++){
        for(int x=0;x<15;x++){
            int tmp=15*y+x;
            ofs<<map[tmp]<<" ";
        }
        ofs<<endl;
        }
    }
    void printMap(int pid,int turn,const char* title,int map[15][15]){
        ostringstream oss;
        oss<<"mylog/log"<<pid<<"-turn"<<turn;
        std::ofstream ofs(oss.str(), std::ios::app );
        ofs<<"#"<<title<<std::endl;
        for(int x=0;x<15;x++){
        for(int y=0;y<15;y++){
            ofs<<map[x][y]<<" ";
        }
        ofs<<endl;
        }
    }
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
                int enemyTerritory, blankTerritory, friendTerritory, injury, hiding, avoiding, moving, doubleAction, center;
                info.tryAction(action, undo, enemyTerritory, blankTerritory, friendTerritory, injury, hiding, avoiding, moving, center,myTern, enemyMemory, myfield, doubleAction);
                double gain = enemyTerritoryMerits*enemyTerritory/enemyTerritoryMAX
                    + blankTerritoryMerits*blankTerritory/blankTerritoryMAX
                    + friendTerritoryMerits*friendTerritory/friendTerritoryMAX
                    + hurtingMerits*injury/injuryMAX
                    + hidingMerits*hiding/hidingMAX
                    + avoidingMerits*avoiding/avoidingMAX
                    + movingMerits*moving/movingMAX
                    + doubleMerits*doubleAction/doubleActionMAX
                    + centerMerits*center/centerMAX;
                plan(info, me, power-required[action], merits+gain, myTern, enemyMemory, myfield);
                undo.apply();
                currentPlay.pop_back();
            }
        }
    }
    void play(GameInfo& info) {
        currentPlay.clear();
        updateMyField(info);
        setMerits(info.weapon);
        guessEnemyPostion(info);
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
    void guessEnemyPostion(GameInfo& info){       
        int turnOrder[6][2]={{0,7},{3,8},{4,11},{1,6},{2,9},{5,10}};
        //int TureTurnNum=turnOrder[info.weapon+3*info.side][myTern%2]+(myTern/2)*12;
        int TureTurnNum=info.turn;
        int attackAreaNum[3]={16,12,8};
        int attackAreaX[3][16]={
            {0,0,0,0,0,0,0,0,-1,-2,-3,-4,1,2,3,4},
            {1,1,1,2,-1,-1,-1,-2,0,0,0,0},
            {1,1,1,-1,-1,-1,0,0}
        };
        int attackAreaY[3][16]={
            {1,2,3,4,-1,-2,-3,-4,0,0,0,0,0,0,0,0},
            {1,0,-1,0,1,0,-1,0,1,2,-1,-2},
            {1,0,-1,1,0,-1,1,-1}
        };
        int diffField[15][15] = {};
        for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            diffField[j][i]=7;
        }
        }
        //std::cerr<<"aaaaaa "<<info.turn<<" bbbbb"<<TureTurnNum<<std::endl;
        printMap2(info.weapon+info.side*3,TureTurnNum,"real",info.field);
        //detect difference
        for(int i=0;i<225;i++){
            int x=i%15;
            int y=i/15;
            if(oldMap[i]!=info.field[i]&&oldMap[i]!=9){
                diffField[y][x]=info.field[i];
            }
        }

        printMap(info.weapon+info.side*3,TureTurnNum,"diff",diffField);
        //count enemy possible pos
        int possibleMap[3][15][15]={};
        int countPossiblePos[3]={};
        for(int j=0;j<225;j++){
            int x=j%15;
            int y=j/15;
            if(diffField[y][x]>=3&&diffField[y][x]<=5){
                int enemyId=diffField[y][x];
                countPossiblePos[enemyId-3]++;
                //fill which xy in enemy attack area
                for(int k=0;k<attackAreaNum[enemyId-3];k++){
                    int tmpx=x+attackAreaX[enemyId-3][k];
                    int tmpy=y+attackAreaY[enemyId-3][k];
                    if(tmpx<0||tmpx>=15||tmpy<0||tmpy>=15)continue;  
                    possibleMap[enemyId-3][tmpy][tmpx]++;
                }
            }   
        }
        //remain only postions which have times equals to count
        for(int enemyId=3;enemyId<6;enemyId++){
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleMap[enemyId-3][y][x]!=countPossiblePos[enemyId-3]){
                    possibleMap[enemyId-3][y][x]=0;
                }
            }   
        }
        printMap(info.weapon+info.side*3,TureTurnNum,"enemy3possible",possibleMap[0]);
        printMap(info.weapon+info.side*3,TureTurnNum,"enemy4possible",possibleMap[1]);
        printMap(info.weapon+info.side*3,TureTurnNum,"enemy5possible",possibleMap[2]);
        
        //eliminate possbles
        
        //eliminate by empty and not your team's but outOfSight
        for(int enemyId=3;enemyId<6;enemyId++){
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleMap[enemyId-3][y][x]>0&&(info.field[j]<3||info.field[j]==8)){
                    possibleMap[enemyId-3][y][x]=0;
                }
            }   
        }
        printMap(info.weapon+info.side*3,TureTurnNum,"enemy3possible",possibleMap[0]);
        printMap(info.weapon+info.side*3,TureTurnNum,"enemy4possible",possibleMap[1]);
        printMap(info.weapon+info.side*3,TureTurnNum,"enemy5possible",possibleMap[2]);
        
        //eliminate by oldEnemyPostion
        for(int enemyId=3;enemyId<6;enemyId++){
            if(oldEnemyPostionX[enemyId-3]==-1&&oldEnemyPostionY[enemyId-3]==-1)continue;
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleMap[enemyId-3][y][x]>0){
                    if(abs(x-oldEnemyPostionX[enemyId-3])+abs(y-oldEnemyPostionY[enemyId-3])<=1){continue;}
                    possibleMap[enemyId-3][y][x]=0;
                }
            }   
        }
        printMap(info.weapon+info.side*3,TureTurnNum,"enemy3possible",possibleMap[0]);
        printMap(info.weapon+info.side*3,TureTurnNum,"enemy4possible",possibleMap[1]);
        printMap(info.weapon+info.side*3,TureTurnNum,"enemy5possible",possibleMap[2]);
        


        //confirmEnemyPostion
        //TODO:insert to GameInfo.emeySamurai

        //save old field for next
        for(int i=0;i<225;i++){
            oldMap[i] = info.field[i];
        }
        for(int id=3;id<6;id++){
            oldEnemyPostionX[id-3]=info.samuraiInfo[id].curX;
            oldEnemyPostionY[id-3]=info.samuraiInfo[id].curY;
        }
    
    }
    void updateMyField(GameInfo& info){
        int tmpField[15][15] = {};
        int tmp[3][3] = {};
        for(int i=0; i<225; i++){
            int y = i/15;
            int x = i%15;
            if(info.field[i] >= 0 && info.field[i] < 3) {
                tmpField[x][y] = 1;
            }else if(info.field[i] < 6) {
                tmpField[x][y] = -1;
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
};

Player* player = new PlanningPlayer();

