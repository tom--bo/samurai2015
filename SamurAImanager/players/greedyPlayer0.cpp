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

void setMerits(int weaponid){
    switch(weaponid%3) {
        case 0:
            enemyTerritoryMerits  = 60;
            blankTerritoryMerits  = 40;
            friendTerritoryMerits = 10;
            hurtingMerits         =500;
            hidingMerits          = 10;
            avoidingMerits        =100;
            movingMerits          =  5;
            centerMerits          = 30;
            doubleMerits          =200;
            dangerMerits          = 40;
            assassinMerits        =100;
            break;
        case 1:
            enemyTerritoryMerits  = 60;
            blankTerritoryMerits  = 50;
            friendTerritoryMerits = 20;
            hurtingMerits         =500;
            hidingMerits          = 10;
            avoidingMerits        =100;
            movingMerits          =  5;
            centerMerits          = 15;
            doubleMerits          =200;
            dangerMerits          = 40;
            assassinMerits        =100;
            break;
        case 2:
            enemyTerritoryMerits  = 60;
            blankTerritoryMerits  = 50;
            friendTerritoryMerits = 10;
            hurtingMerits         =500;
            hidingMerits          = 10;
            avoidingMerits        =100;
            movingMerits          =  5;
            centerMerits          =  5;
            doubleMerits          =200;
            dangerMerits          = 40;
            assassinMerits        =100;
            break;
    }
}



