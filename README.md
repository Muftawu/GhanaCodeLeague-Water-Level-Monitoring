# GhanaCodeLeague-Water-Level-Monitoring
An IoT project to measure the water level in a tank and alerts personnel in charge through text message when filled to desired level


HOW WATER LEVEL DETECTION WORKS 

Basically the idea of this project is to know the water level in a tank at different time intervals.
With this, the Ultrasonic Sensor (SR04) device was used. This device when powered and programmmed can tell the 
distance between itself and any obstacle in its pathway or range of view.

So by employing this principle, the sensor is mounted on a vertical plane and made to face down directly into the 
tank. This way, after every 3 seconds it returns a reading for the level of the water. 

The readings are then used as conditions to control the behaviour of the system based on whether it above or below our desired value.
When these conditions are met, a buzzer goes off and a RED led flashes as indication. There is also an LCD to display the water level at all times.

HOW THE TEXT MESSAGING WORKS 
There are two microcontrollers used here. An Arduino MEGA and an ESP-8266 popularly known as the NodeMCU.
The MEGA controls the connected hardware in the system such as the Ultrasonic Sensor, the buzzer, LCD and the LEDs. It does not possess WIFI 
capabilities.

The NodeMCU on the other hand has WIFI-capabilites and hence used for text-messaging by pinging a request to a hosting server
which then sends the inputted text message to the desired number.

