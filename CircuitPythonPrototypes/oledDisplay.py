import busio
import displayio
import terminalio
import fourwire
import board
from adafruit_display_text import label
import busio

# Import the SSD1306 module.
import adafruit_ssd1306


SDA_pin = board.IO25
SCL_pin = board.IO26
# Create the I2C interface.
i2c = busio.I2C(SDA_pin, SCL_pin)

# Create the SSD1306 OLED class.
# The first two parameters are the pixel width and pixel height.  Change these
# to the right size for your display!
display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c)
# Alternatively you can change the I2C address of the device with an addr parameter:
#display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, addr=0x31)

# Clear the display.  Always call show after changing pixels to make the display
# update visible!
display.fill(0)

display.show()

display.fill(0)
display.text('Hello', 0, 0, 1)
display.text('World', 0, 10, 1)
display.show()


import board
import digitalio
import alarm
import time
import busio
from lcd.lcd import LCD
from lcd.i2c_pcf8574_interface import I2CPCF8574Interface
import digitalio

button_a = digitalio.DigitalInOut(board.IO27)
button_a.direction = digitalio.Direction.INPUT
button_a.pull = digitalio.Pull.UP

SDA_pin = board.IO25
SCL_pin = board.IO26

# List I2C devices
def scanI2C():
    i2c = busio.I2C(SDA_pin, SCL_pin)
    i2c.try_lock()
    print(i2c.scan())
    i2c.unlock()
    i2c.deinit()

#scanI2C()

display.fill(0)
display.text("Hello racers!", 0, 0, 1)
display.show()
time.sleep(2)

display.fill(0)
display.show()

start_time = time.monotonic()
last_pressed_time = start_time
last_lap_duration = last_pressed_time - start_time
previous_lap_duration = last_lap_duration
buttonPressed = False

# Configure a PinAlarm to trigger on LOW (object blocking sensor)
pin_alarm = alarm.pin.PinAlarm(board.IO32, value=True, pull=False)
future_time = time.monotonic() + 1
timer_alarm = alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 1)

print("Monitoring LM393 sensor with PinAlarm...")

def show_lap_time():
    display.fill(0)
    timeStr = f"Last Lap:{previous_lap_duration}"
    display.text(timeStr, 0, 0, 1)
    
    timeStr = f"Lap: {last_lap_duration}"
    display.text(timeStr, 0, 10, 1)
    display.show()

show_lap_time()

while True:
    button_click = False
    # Check if the pin alarm has been triggered
    timer_alarm = alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 0.2)
    alarm.light_sleep_until_alarms(pin_alarm, timer_alarm)

    if not button_a.value:  # Button A is pressed
        if not buttonPressed:
            buttonPressed = True
            button_click = True
    else:
        buttonPressed = False
    
    if alarm.wake_alarm == pin_alarm or button_click:
        print("Object detected! 🚀")  # Fast response when object blocks sensor
        buttonPressed = True
        previous_lap_duration = last_lap_duration
        last_lap_duration = time.monotonic() - last_pressed_time
        last_pressed_time = time.monotonic()
        show_lap_time()

    #if alarm.wake_alarm == timer_alarm:
    #    print("Wake from timer!")





    #if sensor.value:
    #    print("Value UP") 
    
    # Main program can continue running here (non-blocking)
    #print("Main loop running...")  
    #time.sleep(0.1)  # Small delay to prevent CPU overload