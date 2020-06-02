# MICRO-MANIA

This repository contains files relating to the final project in the course Microprocessor Applications 2. Each team had 3 members and 2 weeks to create a game built on top of the Real-Time Operating System we developed during the first 2 months of the course. Our project consisted of 4 mini-games played on the TI MSP432 launchpad (powered by ARM Cortex-M4) . Of our 4 mini-games, one was single player and three were multiplayer (using TI's CC3100 network processor).

## 0. Note

README under construction while making additional software to take in-game screenshots and record a video of the games running. 

## 1. Hardware

Each of the game console systems contained the hardware summarized below.

**Primary Development Board**
- The primary dev board used for this project is Texas Instrument's [MSP432P401R launchpad](https://www.digikey.com/product-detail/en/texas-instruments/MSP-EXP432P401R/296-39653-ND/5170609?utm_adgroup=Programmers%20Dev&utm_source=google&utm_medium=cpc&utm_campaign=Dynamic%20Search&utm_term=&utm_content=Programmers%20Dev&gclid=CjwKCAjw8df2BRA3EiwAvfZWaOgytQmXXzaZwAcTeqqKzGvp99005kN3yQMMQ2ZUGjG0ndnfPLmGoxoCn70QAvD_BwE).
- The target processor for the RTOS and game software was the launchpad's ARM Cortex-M4.

**Additional Hardware**
- To enable board-to-board communication via UDP, each launchpad interfaced a TI [CC3100 SimpleLink](https://www.digikey.com/product-detail/en/texas-instruments/CC3100BOOST/296-37769-ND/4862810?utm_adgroup=RF%2FIF%20and%20RFID&utm_source=google&utm_medium=cpc&utm_campaign=Shopping_Texas%20Instruments_0296_Co-op&utm_term=&utm_content=RF%2FIF%20and%20RFID&gclid=CjwKCAjw8df2BRA3EiwAvfZWaJutkkvgnPuYSjb0I8DprFYE4M8eggaCiC-Tff1q4PmoknfxObZhlxoCDgAQAvD_BwE) networking backpack and utilized a TI library that implemented SendPacket and ReceivePacket functions.
-  Each console's launchpad connected to Professor Jin's custom EEL4930 PCB that contained a touchscreen LCD controllable via SPI, RGB LEDs controllable via PWM, 4 buttons and an analog stick that is readable via interrupt or polling lines.

| ![Hardware screenshot goes here](image link here) | 
|:--:| 
| ***Figure 1.1:** Photo of the hardware this repository's software ran on or interfaced with.* |


## 2. RTOS

During the first 5 assignments of the course, each student developed their own RTOS and applications to demo new working features of the operating system. The most important aspects of the RTOS are listed below.

- Scheduler supported background threads, periodic threads, and aperiodic threads.
- Scheduler supported preemptive scheduling of all threads based on user-defined priorities.
- Implemented mutex and semaphore for thread synchronization.
- Implemented interprocess Communication (IPC) between threads using pipes.

Related Files:
[RTOS Folder](https://github.com/digitaldanny/micro-mania/tree/master/rtos)

## 3. Games

### 3.1 Slither (Designed and Developed by Daniel Hamilton)

This game is a limited recreation of the browser game _Slither.IO_, which is reminiscent of the classic arcade game _Snake_. The goal of the game is to kill the other player's snake by tricking him into running into your side while eating randomly spawned food on the map to grow longer. 

| ![Slither screenshot goes here](image link here) | 
|:--:| 
| ***Figure 3.1:** Two snapshots of the same game moment showing how each player sees a different portion of the game depending on their location in the map.* |

Although the game was a simple concept, development on an embedded system was challenging because the snake's head was always in the middle of the screen. This meant that most of the LCD screen pixels had to be redrawn constantly, which is a time consuming task for a 48 MHz processor. To alleviate the LCD refreshing problem, we rewired the LCD so that it would work in parallel mode instead of SPI mode. This allowed us to transfer LCD pixel data much more quickly.

Video Link:
[Slither Gameplay](insert link here)

Related Files:
[Core Game](https://github.com/digitaldanny/micro-mania/blob/master/src/game3.c)
[Snake Functions](https://github.com/digitaldanny/micro-mania/blob/master/src/game3_snake_functions.c)

### 3.2 Flappy Bird (Designed and Developed by Jacob Crain)

This game is a recreation of the hit mobile game _Flappy Bird_ where the player uses a button to control the bird and guide it through randomly spawning pipe obstacles. The goal of the game is to beat your highest score, which is the number of pipes you have cleared successfully. Upon colliding with a pipe, the player dies and restarts the game.

| ![Flappy bird screenshot goes here](image link here) | 
|:--:| 
| ***Figure 3.2:** Flappy bird gameplay showing the bird as the player tries to guide it through the pipe obstacles.* |

Although this game is single player, our implementation allows two players to take turns playing the game while the other player watches.

Video Link:
[Flappy Bird Gameplay](insert link here)

Related Files:
[Core Game](https://github.com/digitaldanny/micro-mania/blob/master/src/game1.c)

### 3.3 Other Games (Designed and Developed by Jacob Crain and Matthew Dolan)

We also implemented two other games called _Tiles_ and _Goomball_. _Tiles_ is a single player memorization-based game where a 4x4 table of tiles with 8 colors are shown to the player at the start of the game.  Each tile has a matching tile with the same color. After 3 seconds, all tiles are flipped over and the player has 3 attempts to guess where all the pairs are.

_Goomball_ is a multiplayer Mario-themed game where both players try to avoid getting hit by the goombas. The last person alive wins the match. The main goal of this game was to experiment with using sprites from images on our LCD.

## 4. Contributors

Daniel Hamilton [**(@sweatpantsdanny)**](https://github.com/sweatpantsdanny)

Jacob Crain [**(@Jake16000)**](https://github.com/Jake16000)

Matthew Dolan [**(@mattbuster)**](https://github.com/mattbuster)
