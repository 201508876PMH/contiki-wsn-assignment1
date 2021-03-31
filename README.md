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

Succes! Your secret 'knock' code was recorded.
Device is now locked until correct 'knock'-series is entered!
```
Pressing the button:
```
Enter your code... 
```
Pressing the button/attempting to log in:
```
Button was pressed at: <time of button press ex: 428>
Button was pressed at: <time of button press ex: 356>
```
After 5 seconds:
```
---------- YOUR ATTEMPTED PASSCODE: ----------
420 356 0 0 0 0 0 0 0 0
-----------------------------------------------

Your time for entering code expired or the passcode was incorrect!...try again.
```
On successtate:
```
You entered the CORRECT passcode!
The program ends here, reboot for program restart.

8888888888888888888 YOU MAY ENTER THE SECRET ROOM 888888888888888888888
88.._|      | `-.  | `.  -_-_ _-_  _-  _- -_ -  .'|   |.'|     |  _..88
88   `-.._  |    |`!  |`.  -_ -__ -_ _- _-_-  .'  |.;'   |   _.!-'|  88
88      | `-!._  |  `;!  ;. _______________ ,'| .-' |   _!.i'     |  88
88..__  |     |`-!._ | `.| |_______________||."'|  _!.;'   |     _|..88
88   |``"..__ |    |`";.| i|_|MMMMMMMMMMM|_|'| _!-|   |   _|..-|'    88
88   |      |``--..|_ | `;!|l|MMoMMMMoMMM|1|.'j   |_..!-'|     |     88
88   |      |    |   |`-,!_|_|MMMMP'YMMMM|_||.!-;'  |    |     |     88
88___|______|____!.,.!,.!,!|d|MMMo * loMM|p|,!,.!.,.!..__|_____|_____88
88      |     |    |  |  | |_|MMMMb,dMMMM|_|| |   |   |    |      |  88
88      |     |    |..!-;'i|r|MPYMoMMMMoM|r| |`-..|   |    |      |  88
88      |    _!.-j'  | _!,"|_|M<>MMMMoMMM|_||!._|  `i-!.._ |      |  88
88     _!.-'|    | _."|  !;|1|MbdMMoMMMMM|l|`.| `-._|    |``-.._  |  88
88..-i'     |  _.''|  !-| !|_|MMMoMMMMoMM|_|.|`-. | ``._ |     |``"..88
88   |      |.|    |.|  !| |u|MoMMMMoMMMM|n||`. |`!   | `".    |     88
88   |  _.-'  |  .'  |.' |/|_|MMMMoMMMMoM|_|! |`!  `,.|    |-._|     88
88  _!"'|     !.'|  .'| .'|[@]MMMMMMMMMMM[@] \|  `. | `._  |   `-._  88
88-'    |   .'   |.|  |/| /                 \|`.  |`!    |.|      |`-88
88      |_.'|   .' | .' |/                   \  \ |  `.  | `._-Lee|  88
88     .'   | .'   |/|  /                     \ |`!   |`.|    `.  |  88
88  _.'     !'|   .' | /                       \|  `  |  `.    |`.|  88
88 vanishing point 888888888888888888888888888888888888888888888(FL)888
```
## How the passcode is validated
By using the dtw (dynamic time warping) algorithmn, we essentially calculate the distance matrix, the the warping function and the cost function. From this we end up having a matrix with the various cost values represented as such (example):
```
             [ 3     67    211   398   545   545   545   545   545   545  ] 
             [ 39    25    127   272   491   751   879   922   1002  1048 ]	 
             [ 175   100   30    78    200   363   600   880   1240  1408 ]	 
             [ 367   231   81    38    104   211   392   616   920   1270 ]	 
             [ 635   438   208   122   48    79    184   332   560   834  ]
costMatrix = [ 951   693   383   254   106   65    122   222   402   628  ]
             [ 1018  1022  632   460   238   156   82    108   214   366  ]
             [ 801   1175  926   711   415   292   144   101   162   269  ]
             [ 777   1224  1293  1035  665   501   279   193   113   147  ]
             [ 689   1170  1571  1393  949   744   448   319   159   113  ]
```
The validating function traverses through the matrix and chooses the lowest values for every neighbour (either right, crooked or down).
For this example matrix, we would end up with the following array:
```
double valueArray [] = {3, 25, 30, 38, 48, 65, 82, 101, 113, 113} 
```
The threshold has been set to 100. So for this example, the attempted passcode would **not** be accepted.
