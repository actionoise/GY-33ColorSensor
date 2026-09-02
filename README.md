MQTT Color Command 

This project is an experiment in communication between a smartphone and an ESP8266 without a direct data connection between the two devices.

MQTT commands are received by a web page running on the smartphone. Each command is converted into a 
different color and displayed as a large square on the screen. A GY-33 color sensor placed over the display reads 
the color, allowing the ESP8266 to identify the corresponding command.

In the current prototype, the colors are mapped to movement and cleaning commands 
for an iRobot Roomba. After recognizing a color, the ESP8266 can send the
corresponding Roomba Open Interface command through its serial connection. 
The system is not limited to Roomba robots: the detected commands can also
control motor drivers, relays, actuators, or other microcontroller-based devices.

The experiment reuses an old smartphone whose USB data connection no longer
works reliably, although the port can still be used to charge the battery.
This makes the phone suitable as a permanently powered visual MQTT gateway instead of discarding it.

How it works

The smartphone opens the web page.

MQTT settings are entered manually or loaded by scanning a Walkeremote QR code.

The web page connects to the MQTT broker and subscribes to the selected topic.

When an MQTT command arrives, the page displays its assigned color.

The GY-33 reads the color from the smartphone screen through I2C.

The ESP8266 compares the reading with previously calibrated values and identifies the command.

In the Roomba configuration, the ESP8266 sends the corresponding command to the robot through its serial interface.

The smartphone display therefore acts as the data output, while its USB port is used only to keep the phone charged.

Roomba command colors

Command

Display color

Right

Red

Left

Green

Backward

Blue

Forward

Cyan

Stop

Magenta

Dock

Yellow

Cleaning motors

Orange

Clean

White

Spot

Purple

Stop cleaning

Light blue

The color mapping can be changed in the web page and in the ESP8266 sketch. 
These commands currently reproduce the controls used for the Roomba experiment,
but each detected color can instead be assigned to a motor-driver action such as forward,
reverse, left, right, speed control, or stop.

Hardware

ESP8266 development board

GY-33 color sensor

Smartphone with a web browser

USB charger for keeping the smartphone powered

Four wires for I2C and power

MQTT broker and Internet connection for the smartphone

GY-33 wiring

The GY-33 is used through its integrated controller in I2C mode.

GY-33

ESP8266

VCC

3.3 V

GND

GND

DR / SDA

GPIO4

CT / SCL

GPIO5

S0

GND

Leave S1, direct SDA, direct SCL, INT, and NC disconnected. The default I2C address is 0x5A.

Calibration

Smartphone displays and color sensors do not produce ideal RGB values. Screen brightness,
viewing distance, ambient light, display technology, and the position of the sensor can change the readings.

The calibration sketch records ten readings for every command color and calculates
average normalized RGB values. These measured values can then be copied into the final command-recognition sketch.

For repeatable results:

Keep the smartphone brightness fixed.

Disable automatic brightness and night mode.

Place the sensor in the same position for calibration and operation.

Shield the sensor from external light.

Keep each color visible while its ten readings are recorded.

Purpose

The experiment explores an unconventional optical communication channel for reusing old smartphones as
MQTT-enabled gateways. The phone handles network communication, while the ESP8266 receives commands by 
observing colors on the display instead of requiring USB, Bluetooth, or a direct Wi-Fi connection to the phone.

The present application controls a Roomba by forwarding the recognized commands from the ESP8266 to 
the robot over serial. However, the optical MQTT bridge is hardware-independent: the same output 
commands can be connected to an H-bridge, a motor driver, relays, servos, or another serial-controlled machine.

Why use the display instead of USB UART?

On many smartphones, especially older Android devices, it can be difficult or 
impossible to use USB charging and an OTG/UART data connection at the same time.
Some devices require special powered adapters, do not support simultaneous
charging and USB host mode, or have a damaged USB data connection while charging still works.

This project avoids that limitation completely:

The smartphone receives commands over MQTT through the web page.

The screen transfers each command optically to the GY-33 sensor.

The ESP8266 or any other microcontroller, such as an ATmega, ATtiny, and so on
interprets the color and controls the connected hardware.

The smartphone USB port of mobile or other devices whant to use
remains dedicated exclusively to charging.

This arrangement allows the phone to remain powered continuously without depending 
on USB serial communication. It is particularly useful for upcycling a device whose display, 
browser, Wi-Fi, and charging functions still work but whose USB data interface is unusable
because broken or driver problem.

Project status

This is an experimental prototype. Reliable operation requires calibration for the specific
smartphone screen and sensor position used in the setup.



License

Add the license that best fits your project before publishing or distributing the source code.
