#include "iGraphics.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

int highscores[3] = {0};

int callCountEnemyPosition = 0;

const int pixelbymeter = 10; // this is only for you, the coder to translate it into something more familiar

struct cars
{
    double x, y, vel, acc;
};

// Variables and constants related to the moving objects:
struct cars enemy[3] = {{175 - 30, 50}, {350 - 30, 50}, {700 - 30, 50}}, myCar = {525 - 30, 50};
const struct cars enemyCopy[3] = {{175 - 30, 50}, {350 - 30, 50}, {700 - 30, 50}}, myCarCopy = {525 - 30, 50};

double myGameY = 0, enemyY[3] = {0}; // this is my car's and enemy's "actual" positions in the game
int myAccelerationIndicator = 0;     // -1, 0 or 1

float roadRail0 = 0;

// In functions making the game and related global variables:
void countdown(void); // under independentTimeTrackers
int countdownCount = 0, countdownIndicator = 0 /*0 or 1*/, ongoingRaceIndicator = 0 /*0 or 1*/;
void resetTimers(void);
void resetGlobalVariables(void);
void enemyAndRoadMovement(void);
void myMovement(void);
void objectPositions(void);         // cover for object movements and race related "detectors"
void independentTimeTrackers(void); // cover for second counters

// myCar acceleration controls:
int accelerationCounter = 0;
void myAccelerationChecker(void); // stops acceleration when keys are not pressed
int previousAccelerationCounter = 0, accelerationCheckerCall = 0;

// end of race and related things:
const int raceLength = 30000;
void endOfRaceDetectorAndScore(void); // checks and handles end of race, under objectPositions cover
int endOfRaceIndicator = 0;
void endMessageCountdown(void);
int endMessageCount = 0;
int showScoreIndicator = 0;
void showScore(void); // a display functio

// scoring and stuff
int currentScore = 0;
void timeTracking(void);
int currentFinishTime = 0; // in unit of 16 millisecond or so

// screens and screenIndex:
int playcount = -1;
void playtime0(void);
void playscreen(void); // introscreen
void screen0(void);    // home
void screen1(void);    // game screen
void screen2(void);    // result / score screen
void screen3(void);    // highscores screen
void gameoverscreen10(void);
void displayMyVariables(void);           // for debug
void displayEnemyAndRoadVariables(void); // also for debug
void displayMySpeed(void);               // for showing speed
int screenIndex = -1;                    // 0, 1, 2, 3

void iDraw()
{
    iClear();
    switch (screenIndex)
    {
    case -1:
        playscreen();
        break;
    case 0:
        screen0();
        break;
    case 1:
        screen1();
        break;
    case 2:
        break;
    case 3:
        screen3();
        break;
    case 10:
        gameoverscreen10();
        break;
    }
}

void iMouse(int button, int state, int mx, int my)
{
    switch (screenIndex)
    {
    case 0:
        if (0 > (mx - 352) * (mx - 352 - 170) && 0 > (my - 309) * (my - 309 - 40))
            screenIndex = 3;
        else if (0 > (mx - 337) * (mx - 337 - 200) && 0 > (my - 225) * (my - 225 - 50))
            screenIndex = 1;
        else if (0 > (mx - 352) * (mx - 352 - 170) && 0 > (my - 151) * (my - 151 - 40))
            exit(0);
        break;
    case 1:
        if (ongoingRaceIndicator)
        {
            if (GLUT_LEFT_BUTTON == button && GLUT_DOWN == state)
                myCar.x -= 10;
            else if (GLUT_RIGHT_BUTTON == button && GLUT_DOWN == state)
                myCar.x += 10;
        }
        break;
    }

    // pixelsaver!!
    if (1 == screenIndex && GLUT_DOWN == state)
        printf("(%d, %d)\n", mx, my);
}

void iMouseMove(int mx, int my)
{
}

void iKeyboard(unsigned char key)
{
    if (27 == key)
    {
        screenIndex = 0;
        resetTimers();
        resetGlobalVariables();
    }

    switch (screenIndex)
    {
    case 1:
        if (0 == countdownIndicator && 0 == ongoingRaceIndicator && 0 == endOfRaceIndicator)
        {
            if ('\n' + 3 == key) // actually iKeyboard registers enter as 13 instead of 10 == '\n'
                countdownIndicator = 1;
            iResumeTimer(1);
        }

        switch (key)
        {
        case 'w':
            if (-1 == myAccelerationIndicator)
            {
                accelerationCounter = 0;
                accelerationCheckerCall = 0;
                previousAccelerationCounter = accelerationCounter = 0;
            }
            myAccelerationIndicator = 1;
            accelerationCounter++;
            break;
        case 's':
            if (1 == myAccelerationIndicator)
            {
                accelerationCounter = 0;
                accelerationCheckerCall = 0;
                previousAccelerationCounter = accelerationCounter = 0;
            }
            myAccelerationIndicator = -1;
            accelerationCounter++;
            break;
        case 'p': // testing key
            myGameY += 70000;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void iSpecialKeyboard(unsigned char key)
{
}

int main()
{
    iSetTimer(15, objectPositions);
    iPauseTimer(0);
    iSetTimer(1000, independentTimeTrackers);
    iPauseTimer(1);
    iSetTimer(500, playtime0);
    iInitialize(874, 500, "NEED FOR SPEED!");
    srand(time(NULL));
    return 0;
}

// screens
void screen0(void)
{
    iSetColor(200, 200, 200);
    iFilledRectangle(0, 0, 874, 500);
    iSetColor(0, 0, 0);
    iFilledRectangle(352, 309, 170, 40);                        // 309, 284, 309
    iSetColor(255, 255, 255);                                   // 437, 250
    iText(375, 320, "High Scores", GLUT_BITMAP_TIMES_ROMAN_24); // 309 - 284 + 295 ==
    iSetColor(0, 0, 0);
    iFilledRectangle(337, 225, 200, 50);
    iSetColor(255, 255, 255);
    iText(414, 243, "Play", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(0, 0, 0);
    iFilledRectangle(352, 151, 170, 40); // 191, 171, 191 - 40 = 151
    iSetColor(255, 255, 255);
    iText(415, 164, "Exit", GLUT_BITMAP_TIMES_ROMAN_24); // 184 - (171 - 151) = 164
    iSetColor(0, 0, 0);
    iText(307, 10, "Press Esc to return HERE", GLUT_BITMAP_TIMES_ROMAN_24);
}

void screen1(void)
{
    /*
    object arrangement (bottom to top, same as writing serial):
        1. Screen base (the black rectangle filling whole screen serving as background)
        2. Road rails (red road marks)
        3. End of road
        4. Enemy Cars
        5. My Car
        6. Countdown
        7. Debug variables
    */

    // 1. Screen base
    iSetColor(30, 30, 30);
    iFilledRectangle(0, 0, 874, 500);

    // 2. roadrails
    iSetColor(255, 0, 0);

    if (-100 >= roadRail0)
        roadRail0 += 100;

    double x[] = {437, 237, 237, 437, 637, 637};
    double y[] = {roadRail0, roadRail0 - 80, roadRail0 - 130, roadRail0 - 50, roadRail0 - 130, roadRail0 - 80};

    float offset = 0;
    if (0 <= myCar.vel)
    {
        offset = sqrt(myCar.vel);
        for (int index = 0; 6 > index; index++)
            y[index] -= offset;
    }

    for (int iteration = 0; 12 > iteration; iteration++) // road
    {
        iFilledPolygon(x, y, 6);
        for (int index = 0; 6 > index; index++)
            y[index] += 100;
    }

    // 3. End of road
    iSetColor(210, 210, 210);
    iFilledRectangle(0, raceLength - myGameY + offset + 50, 874, 2500);

    // 4. enemy cars
    int enemyColors[] = {255, 0, 0, 255, 0};
    for (int index = 0; 3 > index; index++) // enemies
    {
        iSetColor(enemyColors[index], enemyColors[index + 2], enemyColors[index + 1]);
        //iFilledRectangle(enemy[index].x, enemy[index].y, 60, 100);
        iShowBMP2(enemy[index].x, enemy[index].y, "bmpImages\\enemycars.bmp", 0);
    }

    // 5. my car
    iSetColor(255, 255, 255);
    //iFilledRectangle(myCar.x, myCar.y, 60, 100);
    iShowBMP2(myCar.x, myCar.y, "bmpImages\\nome (1).bmp", 0);

    // 6. countdown
    if (0 == countdownIndicator && 0 == ongoingRaceIndicator && 0 == endOfRaceIndicator && 0 == showScoreIndicator)
    {
        iSetColor(200, 200, 200);
        iFilledRectangle(247, 235, 380, 30);
        iSetColor(255, 255, 255);
        iText(277, 241, "Press Enter To Start Countdown", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    else if (1 == countdownIndicator && (0 == ongoingRaceIndicator || (3 == countdownCount || 4 == countdownCount)))
    {
        switch (countdownCount)
        {
        case 1:
            iSetColor(0, 0, 0);
            iFilledRectangle(387, 230, 100, 40);
            iSetColor(255, 0, 0);
            iText(397, 240, "READY", GLUT_BITMAP_TIMES_ROMAN_24);
            break;
        case 2:
            iSetColor(0, 0, 0);
            iFilledRectangle(387, 230, 100, 40);
            iSetColor(255, 255, 0);
            iText(390, 240, "STEADY", GLUT_BITMAP_TIMES_ROMAN_24);
            break;
        case 3:
            iSetColor(0, 0, 0);
            iFilledRectangle(402, 230, 70, 40);
            iSetColor(0, 255, 0);
            iText(416, 240, "GO!", GLUT_BITMAP_TIMES_ROMAN_24);
            ongoingRaceIndicator = 1;
            iResumeTimer(0);
            break;
        case 4:
            countdownIndicator = 0;
            iPauseTimer(1);
            countdownCount = 0;
            break;
        }
    }

    // Winner message:
    if (0 == ongoingRaceIndicator && 1 == endOfRaceIndicator)
    {
        iSetColor(30, 30, 30);
        iFilledRectangle(362, 230, 150, 40);
        iSetColor(255, 215, 0);
        iText(390, 242, "You Win!", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    if (1 == showScoreIndicator)
    {
        showScore();
    }

    // 7. Debug variables
    // displayMyVariables();
    // displayEnemyAndRoadVariables();

    // 8. Show Speed:
    displayMySpeed();
}

void screen3(void) // Need to do work on files!
{
    iSetColor(150, 150, 150);
    iFilledRectangle(0, 0, 874, 500);
    char *Serials = "Position:";
    char *Scores = "Score:";
    iSetColor(0, 0, 0);
    iText(490, 409, Serials, GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(0, 0, 0);
    iText(284, 409, Scores, GLUT_BITMAP_TIMES_ROMAN_24);

    for (int i = 0; 3 > i; i++)
    {
        iSetColor(0, 0, 0);
        char parser[30];
        sprintf(parser, "%d", highscores[i]);
        char parser2[2];
        sprintf(parser2, "%d", i + 1);
        iText(490, 409 - (i + 1) * 100, parser2, GLUT_BITMAP_TIMES_ROMAN_24);
        iText(284, 409 - (i + 1) * 100, parser, GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

void resetTimers(void)
{
    iPauseTimer(0);
    iPauseTimer(1);
}

void resetGlobalVariables(void)
{
    roadRail0 = 0;
    for (int index = 0; 3 > index; index++)
    {
        enemy[index].x = enemyCopy[index].x;
        enemy[index].y = enemyCopy[index].y;
        enemy[index].vel = enemyCopy[index].vel;
        enemy[index].acc = enemyCopy[index].acc;
        enemyY[index] = 0;
    }

    myCar.x = myCarCopy.x;
    myCar.y = myCarCopy.y;
    myCar.vel = myCarCopy.vel;
    myCar.acc = myCarCopy.acc;

    myGameY = 0;
    myAccelerationIndicator = 0;
    countdownIndicator = 0;
    ongoingRaceIndicator = 0;
    countdownCount = 0;
    accelerationCounter = 0;
    previousAccelerationCounter = 0;
    accelerationCheckerCall = 0;
    endOfRaceIndicator = 0;
    endMessageCount = 0;
    showScoreIndicator = 0;
    currentScore = 0;
    currentFinishTime = 0;
    callCountEnemyPosition = 0;
}

void countdown(void)
{
    countdownCount++;
}

void independentTimeTrackers(void)
{
    if (1 == countdownIndicator)
        countdown();
    if (1 == endOfRaceIndicator)
        endMessageCountdown();
}

void endMessageCountdown(void)
{
    endMessageCount++;
}

void objectPositions(void)
{
    myMovement();
    enemyAndRoadMovement();
    if (0 != myAccelerationIndicator)
        myAccelerationChecker();
    endOfRaceDetectorAndScore();
    if (1 == ongoingRaceIndicator)
        timeTracking();
}

void enemyAndRoadMovement(void)
{
    callCountEnemyPosition++;
    int enemySpeed[] = {500, 300, 700};
    double offset = 0;
    if (0 <= myCar.vel)
        offset = sqrt(myCar.vel);
    for (int index = 0; 3 > index; index++)
    {
        enemyY[index] += enemySpeed[index] * 16 / 1000;
        enemy[index].y = enemyY[index] + offset + 50 - myGameY;
    }

    roadRail0 -= 20 * offset * 16 / 1000; // technically it should be roadvelocity * 16 / 1000, but since offset and roadvelocity are equal here coincidentally, I wrote it this way
    if (-100 >= roadRail0)
        roadRail0 += 100;

    if (0 == callCountEnemyPosition % 30)
        for (int index = 0; 3 > index; index++)
        {
            int movement = rand() % 3 - 1;
            enemy[index].x += movement * 10;
            if (enemy[index].x > 800)
            {
                enemy[index].x -= 10;
            }
            else if (enemy[index].x < 74)
            {
                enemy[index].x += 10;
            }
        }
}

void myMovement(void)
{
    switch (myAccelerationIndicator)
    {
    case 0:
        if (0 < myCar.vel)
            myCar.acc = -100 - myCar.vel * myCar.vel / 5000;
        else if (0 > myCar.vel)
        {
            myCar.acc = 0;
            myCar.vel = 0;
        }
        break;
    case 1:
        myCar.acc = 200 - myCar.vel * myCar.vel / 5000;
        break;
    case -1:
        if (0 == myCar.vel)
            myCar.acc = 0;
        else if (0 < myCar.vel)
            myCar.acc = -350 - myCar.vel * myCar.vel / 5000;
        else
        {
            myCar.acc = 0;
            myCar.vel = 0;
        }
        break;
    }

    myCar.vel += myCar.acc * 16 / 1000;
    myGameY += myCar.vel * 16 / 1000;

    double offset = 0;
    if (0 <= myCar.vel)
        offset = sqrt(myCar.vel);
    myCar.y = offset + 50;

    for (int index = 0; 3 > index; index++)
    {
        if ((myCar.x - enemy[index].x < 60 && myCar.x - enemy[index].x > -60) && (myGameY - enemyY[index] < 100 && myGameY - enemyY[index] > -100))
        {
            resetGlobalVariables();
            screenIndex=10;
        }
    }
    if (enemyY[2] >= raceLength)
    {
        resetGlobalVariables();
        screenIndex = 10;
    }
    if (myCar.x > 800 || myCar.x < 74)
    {
        resetGlobalVariables();
        screenIndex = 10;
    }
}

void myAccelerationChecker(void) // it only turns the acceleration state (myAccelerationIndicator) to 0
{
    accelerationCheckerCall++;
    if (0 != previousAccelerationCounter)
    {
        if (0 == accelerationCheckerCall % 4 && previousAccelerationCounter >= accelerationCounter)
        {
            myAccelerationIndicator = 0;
            accelerationCheckerCall = 0;
            previousAccelerationCounter = accelerationCounter = 0;
        }
        else if (0 == accelerationCheckerCall % 4)
            previousAccelerationCounter = accelerationCounter;
    }
    else
    {
        if (35 == accelerationCheckerCall)
        {
            previousAccelerationCounter = accelerationCounter;
            accelerationCheckerCall = 0;
        }
    }
}

void endOfRaceDetectorAndScore(void)
{
    if (myGameY > raceLength && (1 == ongoingRaceIndicator && 0 == endOfRaceIndicator)) // this extra condition is for ensuring it is called only once per race
    {
        ongoingRaceIndicator = 0;
        endOfRaceIndicator = 1;
        iResumeTimer(1);
    }

    if (3 <= endMessageCount)
    {
        endMessageCount = 0;
        endOfRaceIndicator = 0;
        iPauseTimer(1);
        currentScore = (int)(64000000 / currentFinishTime);
        showScoreIndicator = 1;
        for (int i = 0; 3 > i; i++)
        {
            if (currentScore > highscores[i])
            {
                highscores[i] = currentScore;
                break;
            }
        }
    }
}

void showScore(void)
{
    iSetColor(0, 0, 0);
    char parser[300];
    sprintf(parser, "Score: %d", currentScore);
    iText(400, 230, parser);
}

void timeTracking(void)
{
    currentFinishTime++;
}

void displayMyVariables(void)
{
    char parser[300];
    iSetColor(255, 255, 255);
    sprintf(parser, "Velocity = %g", myCar.vel);
    iText(700, 300, parser, GLUT_BITMAP_TIMES_ROMAN_10);
    sprintf(parser, "Accelrat = %g", myCar.acc);
    iText(700, 280, parser, GLUT_BITMAP_TIMES_ROMAN_10);
    sprintf(parser, "Real Y = %g", myGameY);
    iText(700, 260, parser, GLUT_BITMAP_TIMES_ROMAN_10);
}

void displayEnemyAndRoadVariables(void)
{
    char parser[300];
    iSetColor(255, 255, 255);
    sprintf(parser, "enemy[3] position = %g", enemyY[2]);
    iText(700, 350, parser, GLUT_BITMAP_TIMES_ROMAN_10);
    sprintf(parser, "road[0] position = %g", roadRail0);
    iText(700, 230, parser, GLUT_BITMAP_TIMES_ROMAN_10);
}

void displayMySpeed(void)
{
    char parser[40];
    iSetColor(255, 255, 255);
    sprintf(parser, "%dMPH", (int)myCar.vel / 5);
    iText(30, 230, parser, GLUT_BITMAP_TIMES_ROMAN_24);
}

void playtime0(void)
{
    playcount++;
    if (3 <= playcount)
    {
        iPauseTimer(2);
        screenIndex = 0;
        playcount = 0;
    }
}

void playscreen(void)
{
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, 874, 500);
    iShowBMP(0, 0, "bmpImages\\NFS Intro (Retro Version).bmp");
}

void gameoverscreen10(void)
{
    iSetColor(155, 0, 0);
    iFilledRectangle(0, 0, 874, 500);
    iSetColor(0, 0, 0);
    iText(230, 380, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
}