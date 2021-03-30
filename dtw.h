#include <stdio.h>
#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "lib/memb.h"
#include "sys/log.h"
#include <math.h>

/* GLOBAL */
static double matrix[10][10];
static double cost[10][10];

/* Prototyping */
void calcDistanceMatrix(int userPasscodeArray[], int userTrialArray[]);
void computeAccumulatedCostMatrix(int userPasscodeArray[], int userTrialArray[]);
void displayCostMatrix();
void displayDistanceMatrix();
void displayArray();
double findSmallest(double val1, double val2, double val3);
bool isPasswordAccepted(int userPasscodeArray[], int userTrialArray[]);

void calcDistanceMatrix(int userPasscodeArray[], int userTrialArray[])
{
    uint8_t i;
    uint8_t j;
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (userPasscodeArray[j] - userTrialArray[i] < 0)
            {
                matrix[i][j] = (userPasscodeArray[j] - userTrialArray[i]) * (-1);
            }
            else
            {
                matrix[i][j] = userPasscodeArray[j] - userTrialArray[i];
            }
        }
    }
    // displayDistanceMatrix();
}

void computeAccumulatedCostMatrix(int userPasscodeArray[], int userTrialArray[])
{
    calcDistanceMatrix(userPasscodeArray, userTrialArray);
    uint8_t i;
    uint8_t j;

    /* Initialization */
    cost[0][0] = matrix[0][0];

    for (i = 0; i < 10; i++)
    {
        cost[i][0] = matrix[i][0] + cost[i - 1][0];
    }
    for (j = 0; j < 10; j++)
    {
        cost[0][j] = matrix[0][j] + cost[0][j - 1];
    }

    /* Accumulated */
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            cost[i][j] = findSmallest(
                             cost[i - 1][j],
                             cost[i][j - 1],
                             cost[i - 1][j - 1]) +
                         matrix[i][j];
        }
    }
    // displayCostMatrix();
}

bool isPasswordAccepted(int userPasscodeArray[], int userTrialArray[])
{
    computeAccumulatedCostMatrix(userPasscodeArray, userTrialArray);

    double currentElement = cost[0][0];
    int shortestPath[30];

    uint8_t i = 0;
    uint8_t a;

    uint8_t row = 0;
    uint8_t column = 0;

    while (row != 10 && column != 10)
    {
        double right = cost[row][column + 1];
        double crooked = cost[row + 1][column + 1];
        double down = cost[row + 1][column];

        double matchArray[] = {right, crooked, down};
        double smallest = matchArray[0];

        shortestPath[i] = (int)currentElement;
        uint8_t smallestIndex = 0;
        for (a = 0; a < 3; a++)
        {
            if (matchArray[a] < smallest)
            {
                smallest = matchArray[a];
                smallestIndex = a;
            }
        }
        if (smallestIndex == 0)
        { // If Right was the smallest
            column++;
        }
        else if (smallestIndex == 1)
        { // If crooked was the smallest
            row++;
            column++;
        }
        else
        { // If down was the smallest
            row++;
        }
        // printf("Curent Element: %d \n", (int)currentElement);
        currentElement = smallest;
        i++;
    }
    // displayArray(shortestPath);
    uint8_t tjekMe;
    for (tjekMe = 0; tjekMe < 10; tjekMe++)
    {
        if (shortestPath[tjekMe] > 105)
        { //If any element is below threshold of 85
            return false;
        }
    }

    return true;
}

double findSmallest(double val1, double val2, double val3)
{
    double array[] = {val1, val2, val3};
    uint8_t i;
    double smallest = array[0];

    for (i = 0; i < 3; i++)
    {
        if (array[i] < smallest)
        {
            smallest = array[i];
        }
    }
    return smallest;
}

void displayCostMatrix()
{
    int row = 0;

    printf("\n");
    printf("Printing COST matrix:\n");
    while (row < 10)
    {
        int columns = 0;
        while (columns < 10)
        {
            printf("%d \t ", (int)cost[row][columns]);
            columns++;
        }
        printf("\n");
        row++;
    }
}

void displayDistanceMatrix()
{
    int row = 0;

    printf("\n");
    printf("Printing DISTANCE matrix:\n");
    while (row < 10)
    {
        int columns = 0;
        while (columns < 10)
        {
            printf("%d \t ", (int)matrix[row][columns]);
            columns++;
        }
        printf("\n");
        row++;
    }
}

void displayArray(int array[])
{
    static int printCounter = 0;
    static int i = 0;
    printf("\n");
    printf("---------- DISPLAY ARRAY: ---------- \n");
    while (i < 10)
    {
        printf("%d \t", array[i]);
        printCounter++;
        i++;
    }
    i = 0;
    printf("\n");
    printf("------------------------------------- \n");
    printf("\n");
}