double enemyTerritoryMerits;
double blankTerritoryMerits;
double friendTerritoryMerits;
double hurtingMerits;
double hidingMerits;
double avoidingMerits;
double movingMerits;
double centerMerits;
double doubleMerits;

void setMerits(int weaponid){
    switch(weaponid){
        case 0:
            enemyTerritoryMerits = 2;
            blankTerritoryMerits = 1;
            friendTerritoryMerits = 0.1;
            hurtingMerits = 100;
            hidingMerits = 0.1;
            avoidingMerits = -10;
            movingMerits = 0.2;
            centerMerits = 0.2;
            doubleMerits = 50;
            break;
        case 1:
            enemyTerritoryMerits = 2;
            blankTerritoryMerits = 1;
            friendTerritoryMerits = 0.1;
            hurtingMerits = 100;
            hidingMerits = 0.1;
            avoidingMerits = -10;
            movingMerits = 0.2;
            centerMerits = 0.2;
            doubleMerits = 50;
            break;
        case 2:
            enemyTerritoryMerits = 2;
            blankTerritoryMerits = 1;
            friendTerritoryMerits = 0.1;
            hurtingMerits = 100;
            hidingMerits = 0.1;
            avoidingMerits = -10;
            movingMerits = 0.2;
            centerMerits = 0.2;
            doubleMerits = 50;
            break;
    }
}
