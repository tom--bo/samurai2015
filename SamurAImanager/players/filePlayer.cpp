#include <fstream>
#include <string>
#include <iostream>
double enemyTerritoryMerits;
double blankTerritoryMerits;
double friendTerritoryMerits;
double hurtingMerits;
double hidingMerits;
double avoidingMerits;
double movingMerits;
double centerMerits;
double doubleMerits;


double getIntFromFS(std::ifstream& ifs){
    std::string str;
    std::getline(ifs,str);
    return std::stod(str);
}

void setMerits(int weaponid){
    std::ifstream ifs("evolution/load.gen");
    std::string str;
    if (ifs.fail()){
        return;
    }
    enemyTerritoryMerits = getIntFromFS(ifs);
    blankTerritoryMerits = getIntFromFS(ifs);;
    friendTerritoryMerits = getIntFromFS(ifs);
    hurtingMerits = getIntFromFS(ifs);
    hidingMerits = getIntFromFS(ifs);
    avoidingMerits = getIntFromFS(ifs);
    movingMerits = getIntFromFS(ifs);
    centerMerits = getIntFromFS(ifs);
    doubleMerits = getIntFromFS(ifs);
    
}
