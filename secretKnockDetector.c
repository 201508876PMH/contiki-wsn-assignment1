#include <stdio.h>
#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "lib/memb.h"
#include "sys/log.h"
#include "dtw.h"

/**
 * @ This secretKnockDetector.c file is ment to be used in conjuction
 * @ with the seperate logic file called dtw.h
 * @ THIS PROGRAM HAS ONLY BEEN TESTED IN A SIMULATED ENVIROMENT (COOJA)
 * 
 * @ Author: Peter Marcus Hoveling
 * @ Date: 31/03/2021
*/

/* Macro definition */
#define LOG_MODULE "Knock detector"
#define LOG_LEVEL LOG_LEVEL_DBG

/* GLOBAL */
static process_event_t bttn_pressed_event;
static process_event_t recorded_data_ready_event;
static process_event_t termination_event;
static int userPasscodeArray[10];
static int userTrialArray[10];

/*-----------------------------------------------------------*/
PROCESS(secretKnockDetector, "Secret Knock Detector process");
PROCESS(bttnProcess, "Fuction for pressing the button");
PROCESS(timerProcess, "Function for timer");
PROCESS(secretCodeLogger, "Function for recording secret knocks");
PROCESS(secretCodeValidator, "Function for validating secret knocks");
PROCESS(acceptState, "Accept state for when password is correct");
AUTOSTART_PROCESSES(&secretKnockDetector);
/*-----------------------------------------------------------*/

/**
 * Initial process, calling the bttnProcess and timerProcess
 * ---
 * Arguments: secretKnockDetector, ev, data
 * Retruns: None
*/
PROCESS_THREAD(secretKnockDetector, ev, data)
{
    PROCESS_BEGIN();
    printf("\n");
    LOG_INFO("* * * PROGRAM HAS BEEN STARTED, PRESS BUTTON FOR INSTRUCTIONS * * * \n");
    printf("\n");
    process_start(&bttnProcess, NULL);
    process_start(&timerProcess, NULL);
    PROCESS_END();
}

/**
 * Button process which waits for button event to call the timerProcess.
 * When calling the timer process, we send a custom bttn_pressed_event
 * ---
 * Arguments: bttnProcess, ev, data
 * Returns: None
*/
PROCESS_THREAD(bttnProcess, ev, data)
{
    PROCESS_BEGIN();
    bttn_pressed_event = process_alloc_event();

    SENSORS_ACTIVATE(button_sensor);
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    process_post(&timerProcess, bttn_pressed_event, NULL);
    PROCESS_END();
}

/**
 * Timer process, for when the user starts the recording of his/her passcode.
 * The timer is set to 5 seconds, where while the timer is active we send the
 * logged timestamp for the bttn presses to the loggerProcess, and send  a 
 * custom termination event for the logger process when the time has expired.
 * ---
 * Arguments: timerProcess, ev, data
 * Returns: None
*/
PROCESS_THREAD(timerProcess, ev, data)
{
    static struct timer timer; // Define timer vatiable
    struct timer *timerPtr;
    timerPtr = &timer;
    static clock_time_t recordedClockTime;
    static bool isRecording = false;

    PROCESS_BEGIN();
    recorded_data_ready_event = process_alloc_event();
    termination_event = process_alloc_event();

    PROCESS_WAIT_EVENT_UNTIL(ev == bttn_pressed_event);
    leds_on(0x40);
    LOG_INFO("Bttn pressed!, you now have 5 seconds to record your secret knocks \n");

    timer_set(timerPtr, CLOCK_SECOND * 5); // Initiate timer to trigger event
    process_start(&secretCodeLogger, timerPtr);
    isRecording = true;
    while (isRecording == true)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

        if (timer_expired(timerPtr))
        {
            isRecording = false;
            process_post(&secretCodeLogger, termination_event, NULL);
        }
        else
        {
            recordedClockTime = timer_remaining(timerPtr);
            process_post(&secretCodeLogger, recorded_data_ready_event, &recordedClockTime);
        }
    }
    PROCESS_WAIT_EVENT_UNTIL(timer_expired(&timer)); // Wait until timer is expired
    timer_reset(&timer);                             // Reset timer
    PROCESS_END();
}

/**
 * Process for continuously logging the sent data from the timerProcess.
 * This process is initated from the timerProcess and only stops logging
 * bttn press input, when a custom termination event is sent. When this is done
 * the user code is printed.
 * ---
 * Arguments: secretCodeLogger, ev, data
 * Returns: None
*/
PROCESS_THREAD(secretCodeLogger, ev, data)
{
    static int arrayCounter = 0;
    static int i = 0;
    static int printCounter = 0;
    PROCESS_BEGIN();

    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == recorded_data_ready_event || ev == termination_event);
        if (ev == recorded_data_ready_event)
        {
            userPasscodeArray[arrayCounter] = (int)*((clock_time_t *)data);
            arrayCounter++;
            LOG_DBG("Button was pressed at: %d \n", (int)*((clock_time_t *)data));
        }
        else
        {
            break;
        }
    }
    leds_off(0x40);
    printf("\n");
    printf("---------- YOUR SECRET CODE: ---------- \n");
    int arraySize = sizeof(userPasscodeArray) / sizeof(userPasscodeArray[0]);
    while (i < arraySize)
    {
        printf("%d \t", userPasscodeArray[i]);
        printCounter++;
        i++;
    }
    printf("\n");
    printf("--------------------------------------- \n");
    printf("\n");
    printf("Succes! Your secret 'knock' code was recorded. \n");
    process_start(&secretCodeValidator, NULL);
    PROCESS_END();
}

/**
 * Process for validating the attempted code matches the original.
 * The process continuously validates user input from a 5 seconds
 * counter. Every 5 seconds, the logic from the dtw.h file is called
 * and a match is attempted. If the 'isPasswordAccepted' function returns
 * true, the passcodes match and the user is granted access.
 * ---
 * Arguments: secretCodeValidator, ev, data
 * Returns: None
*/
PROCESS_THREAD(secretCodeValidator, ev, data)
{
    static struct timer timer; // Define timer vatiable
    struct timer *timerPtr;
    timerPtr = &timer;
    static clock_time_t recordedClockTime;
    static bool isRecording = false;

    static int arrayCounter = 0;
    static int i = 0;
    static int printCounter = 0;
    static int codeIsNotCorrect = true;

    PROCESS_BEGIN();
    LOG_INFO("Device is now locked until correct 'knock'-series is entered! \n");

    while (codeIsNotCorrect == true)
    {
        // printf("Press bttn to attemp to log in \n");
        PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
        timer_set(timerPtr, CLOCK_SECOND * 5);
        leds_on(0x40);
        printf("Enter your code... \n");
        isRecording = true;

        while (isRecording == true)
        {

            PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
            LOG_DBG("Button was pressed at: %d \n", (int)timer_remaining(timerPtr));

            if (timer_expired(timerPtr))
            {
                leds_off(0x40);
                printf("\n");
                printf("---------- YOUR ATTEMPTED PASSCODE: ---------- \n");
                int arraySize = sizeof(userTrialArray) / sizeof(userTrialArray[0]);
                while (i < arraySize)
                {
                    printf("%d \t", userTrialArray[i]);
                    printCounter++;
                    i++;
                }
                i = 0;
                arrayCounter = 0;

                timer_reset(&timer); // Reset timer
                printf("\n");
                printf("----------------------------------------------- \n");

                if (isPasswordAccepted(userPasscodeArray, userTrialArray) == true)
                {
                    leds_off(LEDS_RED);
                    process_start(&acceptState, NULL);
                    isRecording = false;
                    codeIsNotCorrect = false;
                }
                else
                {
                    leds_on(LEDS_RED);
                    LOG_INFO("Your time for entering code expired or the passcode was incorrect!...try again. \n");
                    isRecording = false;
                }
                int a = 0;
                while (a < arraySize)
                {
                    userTrialArray[a] = 0;
                    a++;
                }

                a = 0;
            }
            else
            {
                recordedClockTime = timer_remaining(timerPtr);
                userTrialArray[arrayCounter] = recordedClockTime;
                arrayCounter++;
            }
        }
    }
    PROCESS_END();
}

/**
 * The 'acceptstate' process, for when the user enteres the correct code
 * This process serves no other purpose than to show some simple ascii art
 * and visually show the code was accepted.
 * ---
 * Arguments: accepState, ev, dat
 * Returns: None
*/
PROCESS_THREAD(acceptState, ev, data)
{
    PROCESS_BEGIN();
    leds_on(LEDS_GREEN);
    printf("You entered the CORRECT passcode! \n");
    printf("The program ends here, reboot for program restart. \n");
    printf("\n");
    printf("8888888888888888888 YOU MAY ENTER THE SECRET ROOM 888888888888888888888 \n");
    printf("88.._|      | `-.  | `.  -_-_ _-_  _-  _- -_ -  .'|   |.'|     |  _..88 \n");
    printf("88   `-.._  |    |`!  |`.  -_ -__ -_ _- _-_-  .'  |.;'   |   _.!-'|  88 \n");
    printf("88      | `-!._  |  `;!  ;. _______________ ,'| .-' |   _!.i'     |  88 \n");
    printf("88..__  |     |`-!._ | `.| |_______________||.\"'|  _!.;'   |    _|.. 88 \n");
    printf("88   |``\"..__ |  |`\";.| i  |_|MMMMMMMMMMM|_|'| _!-|   |   _|..-|'    88 \n");
    printf("88   |      |``--..|_ | `;!|l|MMoMMMMoMMM|1|.'j   |_..!-'|     |     88 \n");
    printf("88   |      |    |   |`-,!_|_|MMMMP'YMMMM|_||.!-;'  |    |     |     88 \n");
    printf("88___|______|____!.,.!,.!,!|d|MMMo * loMM|p|,!,.!.,.!..__|_____|_____88 \n");
    printf("88      |     |    |  |  | |_|MMMMb,dMMMM|_|| |   |   |    |      |  88 \n");
    printf("88      |     |    |..!-;'i|r|MPYMoMMMMoM|r| |`-..|   |    |      |  88 \n");
    printf("88     |    _!.-j' | _!,\"  |_|M<>MMMMoMMM|_||!._|  `i-!.._ |      |  88 \n");
    printf("88     _!.-'|   | _.\"|  !; |1|MbdMMoMMMMM|l|`.| `-._|    |``-.._  |  88 \n");
    printf("88..-i'     |  _.''|  !-| !|_|MMMoMMMMoMM|_|.|`-. | ``._ |    |``\".. 88 \n");
    printf("88   |      |.|    |.|  !| |u|MoMMMMoMMMM|n||`. |`!   | `\".   |      88 \n");
    printf("88   |  _.-'  |  .'  |.' |/|_|MMMMoMMMMoM|_|! |`!  `,.|    |-._|     88 \n");
    printf("88  _!\"'|    !.'|  .'| .' |[@]MMMMMMMMMMM[@] |  `. | `._  |   `-._   88 \n");
    printf("88-'    |   .'   |.|  |/| /                 \\|`.  |`!    |.|     |`- 88 \n");
    printf("88      |_.'|   .' | .' |/                   \\ |  `.  | `._|         88 \n");
    printf("88     .'   | .'   |/|  /                     \\`!   |`.|    `.  |    88 \n");
    printf("88  _.'     !'|   .' | /                       \\`  |  `.    |`.|     88 \n");
    printf("88888888888888888888888888888888888888888888888888888888888888888888888 \n");
    printf("\n");
    PROCESS_END();
}