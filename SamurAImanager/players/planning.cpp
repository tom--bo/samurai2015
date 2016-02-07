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

int oldMap[225];
int oldEnemyPostionX[3]={};
int oldEnemyPostionY[3]={};
int oldTurnNum=-1;
#define enemyTerritoryMAX 5.0
#define blankTerritoryMAX 5.0
#define friendTerritoryMAX 5.0
#define injuryMAX 1.0
#define hidingMAX 1.0
#define avoidingMAX 1.0
#define movingMAX 1.0
#define doubleActionMAX 1.0
#define centerMAX 10.0


void printParam(int i){
return;
cerr<<"#player"<<i<<" "<<enemyTerritoryMerits<<" "<<blankTerritoryMerits<<" "<<friendTerritoryMerits<<" "<<hurtingMerits<<" "<<hidingMerits<<" "<<avoidingMerits<<" "<<movingMerits<<" "<<centerMerits<<" "<<doubleMerits<<endl;



}

struct PlanningPlayer: Player {
    PlanningPlayer():Player(){
        for(int i=0;i<225;i++){
            oldMap[i]=8;
        }   
    
    }
    void printMap2(int pid,int turn,const char* title,int map[225]){
        return;
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
        return;
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
    void printStr(int pid,int turn ,const char* str){
        return;
        ostringstream oss;
        oss<<"mylog/log"<<pid<<"-turn"<<turn;
        std::ofstream ofs(oss.str(), std::ios::app );
        ofs<<"#"<<str<<std::endl;
    }
    void plan(GameInfo& info, SamuraiInfo& me, int power, double merits, int enemyMemory[100], int myfleld[2]) {
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
                info.tryAction(action, undo, enemyTerritory, blankTerritory, friendTerritory, injury, hiding, avoiding, moving, center, info.turn, enemyMemory, myfield, doubleAction);
                double gain = enemyTerritoryMerits*enemyTerritory/enemyTerritoryMAX
                    + blankTerritoryMerits*blankTerritory/blankTerritoryMAX
                    + friendTerritoryMerits*friendTerritory/friendTerritoryMAX
                    + hurtingMerits*injury/injuryMAX
                    + hidingMerits*hiding/hidingMAX
                    + avoidingMerits*avoiding/avoidingMAX
                    + movingMerits*moving/movingMAX
                    + doubleMerits*doubleAction/doubleActionMAX
                    + centerMerits*center/centerMAX;
                plan(info, me, power-required[action], merits+gain, enemyMemory, myfield);
                undo.apply();
                currentPlay.pop_back();
            }
        }
    }
    void memoryTurnEnd(GameInfo& info){
        Undo undo;
        int enemyTerritory, blankTerritory, friendTerritory, injury, hiding, avoiding, moving, doubleAction, center; 
        for(int action: bestPlay){
            if(action>0&&action<5){info.occupy(action);}
            else info.tryAction(action, undo, enemyTerritory, blankTerritory, friendTerritory, injury, hiding, avoiding, moving, center, info.turn, enemyMemory, myfield, doubleAction);
        }
        //print default infomations
        for(int i=0;i<6;i++){
            ostringstream oss;
            oss<<"agent"<<i <<" "<<info.samuraiInfo[i].curX<<","<<info.samuraiInfo[i].curY;
            printStr(info.weapon+3*info.side,info.turn,oss.str().c_str());
        }
        printMap2(info.weapon+3*info.side,info.turn,"realAfter",info.field);
        //save old field for next
        for(int i=0;i<225;i++){
            oldMap[i] = info.field[i];
        }
        for(int id=3;id<6;id++){
            oldEnemyPostionX[id-3]=info.samuraiInfo[id].curX;
            oldEnemyPostionY[id-3]=info.samuraiInfo[id].curY;
        }
        oldTurnNum=info.turn;

    }
    void play(GameInfo& info) {
        currentPlay.clear();
        updateMyField(info);
        setMerits(info.weapon);
        guessEnemyPostion(info);
        bestMerits = -5000;
        SamuraiInfo& me = info.samuraiInfo[info.weapon];
        for (int s = 3; s != 6; s++) {
            SamuraiInfo& si = info.samuraiInfo[s];
            if((si.curX != -1 && si.curY != -1 ) && abs(me.curX-si.curX)+abs(me.curY-si.curY)<=5){
                enemyMemory[info.turn/6] += 1;
            }
        }
        plan(info, info.samuraiInfo[info.weapon], 7, 0, enemyMemory, myfield);
        for (int action: bestPlay) {
            cout << action << ' ';
        }
        memoryTurnEnd(info);
    }
    
    void guessEnemyPostion(GameInfo& info){       
        int turnOrder[6][2]={{0,7},{3,8},{4,11},{1,6},{2,9},{5,10}};
        int TureTurnNum=info.turn;
        int playerIndex=info.weapon+3*info.side;
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

        //print default infomations
        for(int i=0;i<6;i++){
            ostringstream oss;
            oss<<"agent"<<i <<" "<<info.samuraiInfo[i].curX<<","<<info.samuraiInfo[i].curY;
            printStr(playerIndex,TureTurnNum,oss.str().c_str());
        }
        printMap2(playerIndex,TureTurnNum,"real",info.field);

        //check for escape memory
        //if you dead, escape oldmap and oldEnemy
        if((TureTurnNum-oldTurnNum)/12>0){
            for(int i=0;i<6;i++){
                oldEnemyPostionX[i-3]=-1;
                oldEnemyPostionY[i-3]=-1;
            }
            for(int i=0;i<225;i++){
                oldMap[i]=9;
            }
        }

        for(int i=3;i<6;i++){
            if(i%3==info.weapon){
                int isEnemyDoubled=0;
                if((TureTurnNum%12)%2==1){isEnemyDoubled=1;}
                if(isEnemyDoubled){
                    oldEnemyPostionX[i-3]=-1;
                    oldEnemyPostionY[i-3]=-1;
                }
                else{
                    if(oldEnemyPostionX[i-3]==-1&&oldEnemyPostionY[i-3]==-1)continue;
                    if(info.samuraiInfo[i].hidden==0&&info.samuraiInfo[i].curX!=oldEnemyPostionX[i-3]&&info.samuraiInfo[i].curY!=oldEnemyPostionY[i-3])
                    {
                        SamuraiInfo& si=info.samuraiInfo[i];
                        if(si.curX==si.homeX&&si.curY==si.homeY)continue;//when enemy dead
                        continue;
                        cerr<<"At turn"<<TureTurnNum <<" double action fill by old is Error!!!!"<<endl;
                        cerr<<"old: "<<oldEnemyPostionX[i-3]<<","<<oldEnemyPostionY[i-3]<<endl; 
                        cerr<<"now: "<<info.samuraiInfo[i].curX<<","<<info.samuraiInfo[i].curY<<endl;
                        continue;
                    }
                    info.samuraiInfo[i].curX=oldEnemyPostionX[i-3];
                    info.samuraiInfo[i].curY=oldEnemyPostionY[i-3];
                }

            }
        }

        //detect difference
        for(int i=0;i<225;i++){
            int x=i%15;
            int y=i/15;
            if(oldMap[i]!=info.field[i]&&oldMap[i]!=9){
                diffField[y][x]=info.field[i];
            }
        }
        printMap(playerIndex,TureTurnNum,"diff",diffField);


        //fill enemy possible pos
        int possibleOccupyMap[3][15][15]={};
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
                    possibleOccupyMap[enemyId-3][tmpy][tmpx]++;
                }
            }   
        }
        //remain only postions which have times equals to count
        for(int enemyId=3;enemyId<6;enemyId++){
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleOccupyMap[enemyId-3][y][x]!=countPossiblePos[enemyId-3]){
                    possibleOccupyMap[enemyId-3][y][x]=0;
                }
            }   
        }
        printMap(playerIndex,TureTurnNum,"enemy3possible",possibleOccupyMap[0]);
        printMap(playerIndex,TureTurnNum,"enemy4possible",possibleOccupyMap[1]);
        printMap(playerIndex,TureTurnNum,"enemy5possible",possibleOccupyMap[2]);
       

        //
        //eliminate possbles to occupy
        //

        //eliminate by oldEnemyPostion
        for(int enemyId=3;enemyId<6;enemyId++){
            if(oldEnemyPostionX[enemyId-3]==-1&&oldEnemyPostionY[enemyId-3]==-1)continue;
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleOccupyMap[enemyId-3][y][x]>0){
                    if(abs(x-oldEnemyPostionX[enemyId-3])+abs(y-oldEnemyPostionY[enemyId-3])<=1){continue;}
                    possibleOccupyMap[enemyId-3][y][x]=0;
                }
            }   
        }
        printMap(playerIndex,TureTurnNum,"enemy3possible by oldEnemyPos",possibleOccupyMap[0]);
        printMap(playerIndex,TureTurnNum,"enemy4possible by oldEnemyPos",possibleOccupyMap[1]);
        printMap(playerIndex,TureTurnNum,"enemy5possible by oldEnemyPos",possibleOccupyMap[2]);
       
        //eliminate by oldFieldInfo
        for(int enemyId=3;enemyId<6;enemyId++){
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleOccupyMap[enemyId-3][y][x]>0){
                    bool isAlone=true;
                    int list[4][2]={{1,0},{0,1},{-1,0},{0,-1}};
                    for(auto tmp:list){
                        if(x+tmp[0]<0||x+tmp[0]>14||y+tmp[1]<0||y+tmp[1]>15)continue;
                        int color=oldMap[(y+tmp[1])*15+x+tmp[0]];
                        if(color==9||color==3||color==4||color==5){isAlone=false;break;}
                    }
                    if(isAlone){
                        possibleOccupyMap[enemyId-3][y][x]=0;
                    }
                }
            }   
        }
        printMap(playerIndex,TureTurnNum,"enemy3possible by oldField",possibleOccupyMap[0]);
        printMap(playerIndex,TureTurnNum,"enemy4possible by oldField",possibleOccupyMap[1]);
        printMap(playerIndex,TureTurnNum,"enemy5possible by oldField",possibleOccupyMap[2]);
       
        //eliminate by empty field around
        const int size[3] = {4, 5, 7};
        const int ox[3][7] = {
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 1, 2, 0, 0},
            {-1,-1,-1,0,1,1,1}};
        const int oy[3][7] = {
            {1, 2, 3, 4},
            {1, 2, 0, 1, 0},
            {-1,0,1,1,-1,0,1}};
        for(int enemyId=3;enemyId<6;enemyId++){
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleOccupyMap[enemyId-3][y][x]>0){
                    bool noRealty=false;
                    int weapon=enemyId%3;
                    int actions[4]={1,2,3,4};
                    for(int action: actions){
                        int remainOccupied=possibleOccupyMap[enemyId-3][y][x];
                        int countEnpty=0;
                        for (int k = 0; k != size[weapon]; k++) {
                            int diffx, diffy;
                            rotate(action-1, ox[weapon][k], oy[weapon][k], diffx, diffy);
                            diffx += x;
                            diffy += y;
                            if (0 <= diffx && diffx < 15 && 0 <= diffy && diffy < 15) {
                                if(info.field[diffx+diffy*15]==8||info.field[diffx+diffy*15]==info.weapon){
                                    countEnpty+=1;
                                }
                                if(diffField[diffx][diffy]==enemyId){
                                    remainOccupied-=1;    
                                }
                            }
                        }
                        if(remainOccupied==0&&countEnpty>0){
                            noRealty=true;
                            break;
                        }
                    }
                    if(noRealty){
                        //cerr<<"by empty!!!!!!!!!! at "<<TureTurnNum<<endl;
                        possibleOccupyMap[enemyId-3][y][x]=0;
                    }
                } 
            }
        } 
        printMap(playerIndex,TureTurnNum,"enemy3possible by emptyField",possibleOccupyMap[0]);
        printMap(playerIndex,TureTurnNum,"enemy4possible by emptyField",possibleOccupyMap[1]);
        printMap(playerIndex,TureTurnNum,"enemy5possible by emptyField",possibleOccupyMap[2]);
        
        
        //confirmEnemyPostion
        //confirm when you know where enemy was before turn and now the pospos is beside of before postion 
        for(int enemyId=3;enemyId<6;enemyId++){
            if(oldEnemyPostionX[enemyId-3]==-1&&oldEnemyPostionY[enemyId-3]==-1)continue;
            int count=0,confirmX=-1,confirmY=-1;
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleOccupyMap[enemyId-3][y][x]>0){
                    count++;
                    confirmX=x;
                    confirmY=y;
                }
            }
            if(count==1){
                int diffx=abs(confirmX-oldEnemyPostionX[enemyId-3]);
                int diffy=abs(confirmY-oldEnemyPostionY[enemyId-3]);
                if(diffx+diffy==1){
                    SamuraiInfo& si=info.samuraiInfo[enemyId];
                    if(si.hidden==0){
                        //if you can see enemy and not equaled to confirmXY, its error!!
                        if(si.curX!=confirmX||si.curY!=confirmY){
                            if(info.field[si.curX+si.curY*15]<3||info.field[si.curX+si.curY*15]==8)continue;
                            //cerr<<"postion conflict error!!!! when t="<<TureTurnNum<<" enemy="<<enemyId<<std::endl;
                            printParam(playerIndex); 
                        }
                        continue;
                    }
                    info.samuraiInfo[enemyId].curX=confirmX;
                    info.samuraiInfo[enemyId].curY=confirmY;
                    ostringstream oss;
                    oss<<"#define enemy"<<enemyId<<" to "<<confirmX<<","<<confirmY<<" by ocupyPos and oldEnemy";
                    printStr(playerIndex,TureTurnNum,oss.str().c_str());
                    //cerr<<playerIndex<<" "<<TureTurnNum<<" " << oss.str().c_str()<<std::endl; 
                }
            }
        }

        //estimate enemy pos
        int possibleEnemyMap[3][15][15]={};
        for(int enemyId=3;enemyId<6;enemyId++){
            SamuraiInfo& si=info.samuraiInfo[enemyId];
            if(si.curX!=-1||si.curY!=-1)continue;
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleOccupyMap[enemyId-3][y][x]>0){
                    possibleEnemyMap[enemyId-3][y][x]+=1;
                    if(y>0){possibleEnemyMap[enemyId-3][y-1][x]+=1;}
                    if(x>0){possibleEnemyMap[enemyId-3][y][x-1]+=1;}
                    if(y<14){possibleEnemyMap[enemyId-3][y+1][x]+=1;}
                    if(x<14){possibleEnemyMap[enemyId-3][y][x+1]+=1;}
                }
            }
            //eliminate by color
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(info.field[j]<3||info.field[j]==8){
                    possibleEnemyMap[enemyId-3][y][x]=0;
                }
            }
            //eliminate by oldEnemy
            if(oldEnemyPostionX[enemyId-3]!=-1&&oldEnemyPostionY[enemyId-3]!=-1){
                for(int j=0;j<225;j++){
                    int x=j%15;
                    int y=j/15;
                    int diffx=abs(x-oldEnemyPostionX[enemyId-3]);
                    int diffy=abs(y-oldEnemyPostionY[enemyId-3]);
                    if(diffx+diffy>1){  
                        possibleEnemyMap[enemyId-3][y][x]=0;
                    }
                }
            }
            //confirm by emey pos map
            int count=0,confirmX=0,confirmY=0;
            for(int j=0;j<225;j++){
                int x=j%15;
                int y=j/15;
                if(possibleEnemyMap[enemyId-3][y][x]>0){
                    count++;confirmX=x;confirmY=y;
                }
            }
            if(count==1){
                info.samuraiInfo[enemyId].curX=confirmX;
                info.samuraiInfo[enemyId].curY=confirmY;
                ostringstream oss;
                oss<<"#define enemy"<<enemyId<<" to "<<confirmX<<","<<confirmY<<" by enemyPostionEliminate";
                printStr(playerIndex,TureTurnNum,oss.str().c_str());
                //cerr<<playerIndex<<" "<<TureTurnNum<<" " << oss.str().c_str()<<std::endl; 
            }
        }
        printMap(playerIndex,TureTurnNum,"enemy3estimate",possibleEnemyMap[0]);
        printMap(playerIndex,TureTurnNum,"enemy4estimate",possibleEnemyMap[1]);
        printMap(playerIndex,TureTurnNum,"enemy5estimate",possibleEnemyMap[2]);
       

            
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

