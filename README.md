# Buddy-Bot

A simple robot pet

This project use the follow modules:

- UWB positioning module
- Lidar module
- Motor module
- Voice control module

The sensor of UWB is _ULM3PDOA_ and _ULM3_, the preview used as anchor and the last used as tag, anchor could give the distance and degree between anchor and tag.

Lidar is _RPLIDAR C1_, it can scan around 10 meters and get the location of all obstacles. Based on this we can implement obstacle avoidance and display environment information in real time.

The motor module use _MG310 DC geared motor_ to implement car move with given distance and angle.

And _USB to audio device_ is used to perform voice control module, it will record voice in real time, and then using Convolutional Neural Networks technology to convert spead to text.

## UWB Module

The other functions or classes could include the head file "follow/ulm3_samples.h" and use .getData() or .controlCar() function to get corresponding location data from ULM3PDOA or controal car's direction and distance traveled sensor don't need to worry about the low-level implementation details.

### ULM3PDOAComm

This is the driver class of ULM3 PDOA sensor.

### ULM3AcquisitionCallback

This function is response to process the char array transmitted from ULM3PDOAComm.

These char array looks like this:
**MPxxxx,tag_id,x_cm,y_cm,distance_cm,RangeNumber,pdoa_deg,aoa_deg,distance_offset_cm,pdoa_offset_deg\r\n**

All we needed is _distance_cm, aoa_deg_, the distance and degree between anchor and tag.

The input is char[], thus we can add a '\0' to the end and use _sscanf_ to get data from input array.

### SyncQueue

This is a circular synchronized queue work for multi-threaded interactions.

This queue use mutex and conditional variable to implement mutual exclusion and wake up blocked thread.

## Voice Controlled module

### Functionality

This module enables the control of the buddybot via voice commands.

#### Specific Components:

- **Audio Input:** Utilizes PulseAudio lib for audio stream input.

- **Speech Recognition:** Employs the [`stt` library](https://github.com/coqui-ai/STT) from COQUI AI to interface with a custom-trained model for keyword recognition.

- **Control Mechanism:** Based on recognized keywords, the buddybot performs specific movements.

### Considerations

The project uses the COQUI AI `stt` library, which is a locally running speech-to-text model. It can use either the general model provided by COQUI or a custom-trained model tailored for specific needs.

Due to performance and real-time considerations, this project is designed to recognize only a few specific keywords and execute corresponding motion patterns.

### Highlights

- **Speed:** The general model processes very slowly, so a smaller model trained with a limited amount of developer data is used to achieve faster performance.
- **Accuracy:** While the recognition speed is high, the accuracy for keyword detection is moderately reliable.

### Challenges

- The main disadvantages are the recognition limited to specific voices and severe susceptibility to noise interference.

## RPLIDAR C1 Control Class

![alt text](/doc/picture/426791713181087_.pic_hd.jpg)

### Connect to radar and initialize

Instantiate the radar object lidar, and instantiate the serial port channel class sl::IChannel, and establish a connection with the radar through the function lidar->connect(channel). Since RPLidar C1 needs to start the motor in advance, use the function setMotorSpeed() to complete the corresponding operation.

### Read data and process

Create the array sl_lidar_response_measurement_node_hq_t nodes[8192] to store the position information of obstacles read by the radar. The function grabScanDataHq(nodes, count, 0) usually reads the data in a circle around the radar (about 510) and saves it to the nodes array. , since the points read by the grabScanDataHq function are out of order rather than increasing sequentially according to the angle, the function ascendScanData(nodes, count) is needed to sort the points so that they keep the angle increasing sequentially. The member variables of the structure sl_lidar_response_measurement_node_hq_t are sequentially They are angle_z_q14, dist_mm_q2, and quality respectively represent the angle, distance and quality of the current point. Since what we get are coordinates in the polar coordinate system, we also need to convert the current polar coordinates into the corresponding Cartesian coordinates in subsequent use.

### Stop radar

Use the function lidar->stop() to stop the radar and stop scanning obstacle points. Since RPLidar C1 needs to manually turn on and off the drive motor, call the function lidar->setMotorSpeed(0) to stop the radar motor.

### A\* algorithm

The A\* (A-Star) algorithm is a commonly used pathfinding algorithm, used to find the shortest path from the starting point to the target point in a graphical environment.
The cost function 𝑓(𝑛) consists of two parts: the cost 𝑔(𝑛) from the starting point to the current node along the generated path and the estimated cost ℎ(𝑛) from the current node to the end point. Formula expression: 𝑓(𝑛) = 𝑔(𝑛) + ℎ(𝑛)
h = abs(end_node(1) - openlist(i,1))+abs(end_node(2) - openlist(i,2));
open list: a grid that records all the cells considered to find the shortest path
closed list: a grid whose records will no longer be considered

### Generate dynamic paths

![alt text](/doc/picture/426191713175831_.pic.jpg)
We set the initial point to (50, 70), the stop point to the Cartesian coordinates after processing the data obtained by the UWB module, and set the processed data obtained from the radar to the obstacle point of A*, start the A* algorithm, and obtain the generated path A list of coordinate points and drawn in real time through QT (A\* coordinates are scaled according to a 1:10 scale).

## Motor module

1. **Motor Driver**
   ![alt text](/doc/image.png)
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

**2. Encoder-Based Speed Loop Control**

- Using `pigpio.h`, capture the A and B directional encoder signals from both left and right motors. Employ rising edge counting and the accumulated count values are periodically captured by timer0. The captured `vLcurrent` and `vRcurrent` are adjusted based on the target speed using feedback from the difference values, employing a PID algorithm for speed correction, adjusting PWM output to complete the speed loop.

**3. Precise Angle and Displacement Control**

- Inside timer0, based on the target change in angle or displacement, accumulate the total count value `target_count`. The relationship between count values and actual angles and displacements is derived from formulas based on practical measurement data.
