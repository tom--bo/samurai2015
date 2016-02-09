double enemyTerritoryMerits;
double blankTerritoryMerits;
double friendTerritoryMerits;
double hurtingMerits;
double hidingMerits;
double avoidingMerits;
double movingMerits;
double centerMerits;
double doubleMerits;
double dangerMerits;
double assassinMerits;
double respawnMerits;

void setMerits(int weaponid){
    switch(weaponid%3) {
        case 0:
            enemyTerritoryMerits  =   60;
            blankTerritoryMerits  =   40;
            friendTerritoryMerits =   20;
            hurtingMerits         =10000;
            hidingMerits          =   30;
            avoidingMerits        =  600;
            movingMerits          =   10;
            centerMerits          =   50;
            doubleMerits          =  400;
            dangerMerits          =   40;
            assassinMerits        =  100;
            respawnMerits         =   60;
            break;
        case 1:
            enemyTerritoryMerits  =   60;
            blankTerritoryMerits  =   50;
            friendTerritoryMerits =   20;
            hurtingMerits         =10000;
            hidingMerits          =   30;
            avoidingMerits        =  600;
            movingMerits          =    5;
            centerMerits          =    5;
            doubleMerits          =  400;
            dangerMerits          =   40;
            assassinMerits        =  100;
            respawnMerits         =   60;
            break;
        case 2:
            enemyTerritoryMerits  =   60;
            blankTerritoryMerits  =   50;
            friendTerritoryMerits =   20;
            hurtingMerits         =10000;
            hidingMerits          =   30;
            avoidingMerits        =  600;
            movingMerits          =    5;
            centerMerits          =    5;
            doubleMerits          =  400;
            dangerMerits          =   40;
            assassinMerits        =  100;
            respawnMerits         =   60;
            break;
    }
}



