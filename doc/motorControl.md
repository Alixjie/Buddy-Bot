

1. **Motors**
   - Use two 12V high-power DC reduction motors with built-in Hall encoders to control the direction of the cart.
![alt text](image-1.png)

2. **Motor Driver**
![alt text](image.png)
   - The motors are driven by a TB6612 driver board, which includes a voltage regulator circuit. It can supply 12V to the motors while providing 5V to the Raspberry Pi. It accepts PWM wave inputs and direction signals to control the motors and transmits encoder signals to the Raspberry Pi.
   - Connection details of control pins:
     - Raspberry Pi pin | Raspberry Pi GPIO | TB6612/(MOTOR)
     - 2 | 5V
     - 6 | Ground
     - 13 | GPIO27 | PWMB (Motor1 speed control)
     - 15 | GPIO22 | PWMA (Motor2 speed control)
     - 29 | GPIO5 | AIN1 (Motor1 rotate direction control)
     - 31 | GPIO6 | AIN2
     - 16 | GPIO23 | BIN1 (Motor2 rotate direction control)
     - 18 | GPIO24 | BIN2
     - 32 | GPIO12 | EA2 (Motor2 A phase)
     - 36 | GPIO16 | EB2 (Motor2 B phase)
     - 38 | GPIO20 | EA1 (Motor1 A phase)
     - 40 | GPIO21 | EB1 (Motor1 B phase)

**3. PWM Generation**
   - Utilize the `pigpio.h` library for PWM output control on GPIO22 (left wheel) and GPIO27 (left wheel), with a frequency of 10,000 Hz.

**4. Encoder-Based Speed Loop Control**
   - Using `pigpio.h`, capture the A and B directional encoder signals from both left and right motors. Employ rising edge counting with callback functions `callbackL` and `callbackR` to accumulate counts. The accumulated count values are periodically captured by timer0, stored into `vLcurrent` and `vRcurrent`, and then reset for recounting. The captured `vLcurrent` and `vRcurrent` are adjusted based on the target speed using feedback from the difference values, employing a PID algorithm for speed correction, adjusting PWM output to complete the speed loop.
   - Utilize two instances of the professor’s encoder class to collect encoder signals from the two motors.
   - Numerical Explanation:
     - `gpioSetTimerFunc(0, 54, timercallback);`
     - Based on actual measurements, when the timer is set to 54ms, the differential in encoder count values matches the differential in control output PWM duty cycle. By adding a bias of +/-30, direct feedback correction to the target values can be made:
       ```
       if (countR > 0)
           vR_current = countR + 30;
       if (countR < 0)
           vR_current = countR - 30;
       if (countL > 0)
           vL_current = countL + 30;
       if (countL < 0)
           vL_current = countL - 30;
       ```
     - **PID Speed Correction**:
       - using `VelocityL(int Target_LV, int Current_LV)` and `VelocityR(int Target_RV, int Current_RV)` to correct the PWM output.
       - The PWM adjustment is calculated using the incremental PID formula:
       - ```math
           Pwm(t) = Kp \cdot [e(t) - e(t-1)] + Ki \cdot \sum e(t) + Kd \cdot [e(t) - 2e(t-1) + e(t-2)]
          ```
**5. Precise Angle and Displacement Control**
   - Inside timer0, based on the target change in angle or displacement, accumulate the total count value `target_count`. The relationship between count values and actual angles and displacements is derived from formulas based on practical measurement data:
     - `Target_count = distance * 18`
     - `Target_count = angle * 10/3 + 0.004 * angle * angle`
   - Upon reaching the total count value, control the PWM to stop the cart and use a condition function to notify the data sending function that the command has been completed.
