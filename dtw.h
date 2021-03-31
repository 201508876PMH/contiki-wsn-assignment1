#include <stdio.h>
#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "lib/memb.h"
#include "sys/log.h"
#include <math.h>

/**
 * @ This *.h file is ment for seperate logic to the original
 * @ secretKnockDetector.c file.
 * 
 * @ Author: Peter Marcus Hoveling
 * @ Date: 31/03/2021
*/

/* GLOBAL */
static double distanceMatrix[10][10];
static double costMatrix[10][10];

/* Prototyping */
void calcDistanceMatrix(int userPasscodeArray[], int userTrialArray[]);
void computeAccumulatedCostMatrix(int userPasscodeArray[], int userTrialArray[]);
void displayCostMatrix();
void displayDistanceMatrix();
void displayArray();
double findSmallest(double val1, double val2, double val3);
bool isPasswordAccepted(int userPasscodeArray[], int userTrialArray[]);

/**
 * Function for calculating the distance matrix
 * ---
 * Arguments: Two int arrays, the first being the original passcode
 * and the second being the attempted passcode
 * 
 * Returns: None (void)
*/
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
                distanceMatrix[i][j] = (userPasscodeArray[j] - userTrialArray[i]) * (-1);
            }
            else
            {
                distanceMatrix[i][j] = userPasscodeArray[j] - userTrialArray[i];
            }
        }
    }
    // displayDistanceMatrix();
}

/**
 * Function to calculate the costmatrix and warping function
 * Inspired from: https://towardsdatascience.com/an-illustrative-introduction-to-dynamic-time-warping-36aa98513b98
 * ---
 * Arguments: Two int arrays, the first being the original passcode
 * and the second being the attempted passcode
 * 
 * Returns: None (void)
*/
void computeAccumulatedCostMatrix(int userPasscodeArray[], int userTrialArray[])
{
    calcDistanceMatrix(userPasscodeArray, userTrialArray);
    uint8_t i;
    uint8_t j;

    /* Initialization */
    costMatrix[0][0] = distanceMatrix[0][0];

    for (i = 0; i < 10; i++)
    {
        costMatrix[i][0] = distanceMatrix[i][0] + costMatrix[i - 1][0];
    }
    for (j = 0; j < 10; j++)
    {
        costMatrix[0][j] = distanceMatrix[0][j] + costMatrix[0][j - 1];
    }

    /* Accumulated */
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            costMatrix[i][j] = findSmallest(
                             costMatrix[i - 1][j],
                             costMatrix[i][j - 1],
                             costMatrix[i - 1][j - 1]) +
                         distanceMatrix[i][j];
        }
    }
    // displayCostMatrix();
}

/**
 * Function for validating if attempted passcode vs. the original.
 * Makes use of the dynamic time warping algorithm, were every
 * value from the final cost array must be under 100, to be accepted
 * as a valid code.
 * ---
 * Arguments: Two int arrays, the first being the original passcode
 * and the second being the attempted passcode
 * 
 * Returns: A simple bool, either false or true
 */
bool isPasswordAccepted(int userPasscodeArray[], int userTrialArray[])
{
    computeAccumulatedCostMatrix(userPasscodeArray, userTrialArray);

    double currentElement = costMatrix[0][0];
    int shortestPath[30];

    uint8_t i = 0;
    uint8_t a;

    uint8_t row = 0;
    uint8_t column = 0;

    while (row != 10 && column != 10)
    {
        double right = costMatrix[row][column + 1];
        double crooked = costMatrix[row + 1][column + 1];
        double down = costMatrix[row + 1][column];

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
        currentElement = smallest;
        i++;
    }
    // displayArray(shortestPath);
    uint8_t tjekMe;
    for (tjekMe = 0; tjekMe < 10; tjekMe++)
    {
        if (shortestPath[tjekMe] > 100)
        { //If any element is below threshold of 100
            return false;
        }
    }

    return true;
}

/**
 * Function for finding the smallest value for a 3 given input
 * ---
 * Arguments: Three type double
 * Returns: 1 double type, being the smallest of the given values
 */
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

/**
 * Function for printing cost matrix
 * ---
 * Arguments: None
 * Returns: None (void)
*/
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
            printf("%d \t ", (int)costMatrix[row][columns]);
            columns++;
        }
        printf("\n");
        row++;
    }
}

/**
 * Function for printing the distance matrix
 * ---
 * Arguments: None
 * Returns: None (void)
*/
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
            printf("%d \t ", (int)distanceMatrix[row][columns]);
            columns++;
        }
        printf("\n");
        row++;
    }
}

/**
 * Function for displaying any given array of size 10
 * ---
 * Arguments: int array
 * Returns: none (void)
*/
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