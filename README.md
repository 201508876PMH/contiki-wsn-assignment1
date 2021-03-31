# Dynamic Time Warping (DTW) on sensor node
Contiki-WSN-assignment1

‘Secret knocks’ are used to identify people by sharing a knowledge of a secret by knocking on a door. The one person behind the door listens to the knocks and checks if it matches the rhythm, number of knocks and so on of the known knock.

In this project, we rebuild this schema – the sensor mote holds / knows the secrete sequence and we have to input the sequence using the button. As we learned during the lecture, we have to account for potential shifts in time when comparing the signals. Upon input, the sensor should compare the signals using dynamic time warping - if the result is correct, the green LED should light up, otherwise the red LED should light up.

The following steps should be completed using the sensor node:
1. Write a program that allows you to enter and store a ‘secret’ sequence into
   the mote using the button
   - You have to decide about how you want to store the information.
   - The sensor node will need to ‘know’ when the sequence is over and can
      be stored – come up with an approach to finish the sequence.
2. Write another programme to check whether a new sequence matches the stored
   sequence.
   - Wait for input signal and record input (see 1b for completing a sequence)
   - Use Dynamic Time Warping (DTW) to compare the signals
   - Figure out the best threshold to accept input signals
   - Change the LED to indicate whether the signal was accepted or not

## Compiling the program
To compile the program for the intendid architecture, run the following:
```
make TARGET=sky 
```
To compile the program and upload:
```
make TARGET=sky PORT=/dev/ttyUSB0 <name of file>.upload
```
To login and see your program:
```
make TARGET=sky PORT=/dev/ttyUSB0 login
```
## Program output
```
* * * PROGRAM HAS BEEN STARTED, PRESS BUTTON FOR INSTRUCTIONS * * *
```
User presses the button:
```
Bttn pressed!, you now have 5 seconds to record your secret knocks
```
For every button press user makes:
```
Button was pressed at: <time of button press ex: 428>
```
After 5 seconds:
```
---------- YOUR SECRET CODE: ----------
420 0 0 0 0 0 0 0 0 0
---------------------------------------
```

