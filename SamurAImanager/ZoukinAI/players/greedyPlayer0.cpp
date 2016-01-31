double territoryMerits;
double selfTerritoryMerits;
double hurtingMerits;
double hidingMerits;
double avoidingMerits;
double movingMerits;
double doubleMerits;
void setMerits(int weaponid){
    switch(weaponid){
        case 0:
            territoryMerits = 2;
            selfTerritoryMerits = 0.1;
            hurtingMerits = 100;
            hidingMerits = 0.5;
            avoidingMerits = -12;
            movingMerits = 0.2;               
            doubleMerits = 50;
            break;
        case 1:
            territoryMerits = 2;
            selfTerritoryMerits = 0.1;
            hurtingMerits = 100;
            hidingMerits = 2;
            avoidingMerits = -12;
            movingMerits = 0.5;               
            doubleMerits = 50;
            break;
        case 2:
            territoryMerits = 2;
            selfTerritoryMerits = 0.1;
            hurtingMerits = 100;
            hidingMerits = 1;
            avoidingMerits = -12;
            movingMerits = 0.5;               
            doubleMerits = 50;
            break;
    }
}


