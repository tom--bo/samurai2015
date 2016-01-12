double territoryMerits;
double selfTerritoryMerits;
double hurtingMerits;
double hidingMerits;
double avoidingMerits;
double movingMerits;
double doubleMerits;
 void setMerits(int weaponid){
    switch(weaponid){       case 0:
            territoryMerits = 3;
            selfTerritoryMerits = 0.1;
            hurtingMerits = 100;
            hidingMerits = 0.1;
            avoidingMerits = -10;
            movingMerits = 0.2;
            doubleMerits = 50;
            break;
        case 1:
            territoryMerits = 3;
            selfTerritoryMerits = 0.1;
            hurtingMerits = 100;
            hidingMerits = 0.1;
            avoidingMerits = -10;
            movingMerits = 0.2;
            doubleMerits = 50;
            break;
        case 2:
            territoryMerits = 3;
            selfTerritoryMerits = 0.1;
            hurtingMerits = 100;
            hidingMerits = 0.1;
            avoidingMerits = -10;
            movingMerits = 0.2;
            doubleMerits = 50;
            break;
    }
}
