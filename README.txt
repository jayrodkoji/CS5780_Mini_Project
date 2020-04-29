Ball Target Game

Overview:
    A gyroscope based tilting game that allows the player to interact with a ball with the goal of rolling to various
    targets. There are two levels the user will experience. First, the red level. This level is indicated by a red ball.
    Upon completion of the red level the player will enter the green level indicated by a green ball. To aid the player,
    a red and green led have been added. As the player hits targets the red led will flash faster and faster until it
    locks on solid. This indicated completion of the red level. The green level follows the same pattern. Be careful not
    to let the blinking light cause added anxiety. Upon completing the green level the play will be welcomed with a sceen
    declaring them a winner.



Milestones:

    Milestone 1: Get board needed for project. Initialize gyroscope to get x and y values.

        To accomplish this, we first researched and found a board that would allow us to implement the above project. We
        settled upon the stm32f4-discovery board.

        Next we used the SPI protocol to initialize, read, and write from the l3gd20 gyroscope. The helper functions we
        we used for this can be found in the spi.c file in Src/Helper. We created a function that explicitly
        retrieved the x and y gyroscope data. This can be found in the l3gd20.c file Src/Helper.

    Milestone 2: Create images (backgrounds with targets and ball) needed to display. Display layer 1
                (background with targets) and layer 2 (ball) images.

        The most difficult aspect of the project was figuring out how to not only initialize the LCD display but also
        to display images. This could have easily been a Milestone in and of itself. Once we finally figured out
        the (what seemed like infinite) parameters, we were able to easily create the required images for the target
        and the balls. To aid in displaying the images we used a tool called lcd-image-converter, which helped us create
        the header files for the images. You will find these in the Inc folder.

        To display the targets in random places we used RNG to generate the random numbers which we normalize to the display
        dimensions. This can be found in the rng.c file in Src/Helper.

        Displaying on the individual layers as well as the other functions for the lcd in the lcd.c file in Src/Helper.

    Milestone 3: Update ball position from gyroscope x and y data.

        This also was a tricky bit. We were easily able to get the x and y data; however, the l3gd20 has a ton of noise. We made
        some adjustments to the gyroscope high pass filter and set our own thresholds in main.c to try and minimize this.
        From the new position we increment the ball x and y locations based on the speed which can be a positive or negative
        value up to a max speed of 20.

        Most of this code is found in main.c

    Milestone 4: Trigger LED when target is met. Display winning screen.

        For this milestone we created a structure in main.c that has progressive states the game can be in. This is triggered
        by a distance calculation. If the distance between the ball x and y values is close enough to the target x and y values
        the target is determined hit. As the number of hit targets increases the various states change. In all states excluding the
        last, the LED frequency frequency is increased. We used timers to accomplish this. The LED helper functions can
        be found in led.c in Src/Helper.



Protocols/Labs Used:

    SPI:
        Communication/control of l3gd20 gyroscope.
        Communication/control of LCD.

    GPIO:
        LED.

    Timers:
        LED flash frequency.

    Interrupts:
        LED functionality.

    RNG:
        Random number generation.

    UART:
        Mainly in testing x and y data.



Hardware:

    stm32f4-discovery board:
        the main microcontroller used as the platform.

    l3gd20 digital gyroscope:
        Used to get players tilt for movement of ball.

     ILI9341 2.4" QVGA TFT LCD display:
        Used to display the various intro, game play, and winner screen.

     Two LEDs:
        To display player progress and cause anxiety.
