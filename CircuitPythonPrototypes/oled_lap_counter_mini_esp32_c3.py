import busio
import displayio
import terminalio
import fourwire
import board
from adafruit_display_text import label
import digitalio
import time
import busio

# Import the SSD1306 module.
import adafruit_ssd1306

button_a = digitalio.DigitalInOut(board.IO7)
button_a.direction = digitalio.Direction.INPUT
button_a.pull = digitalio.Pull.UP

slot_sensor = digitalio.DigitalInOut(board.IO5)
slot_sensor.direction = digitalio.Direction.INPUT
slot_sensor.pull = digitalio.Pull.DOWN

def scanI2C():
    i2c = busio.I2C(board.IO20, board.IO21)
    i2c.try_lock()
    print(i2c.scan())
    i2c.unlock()
    i2c.deinit()

scanI2C()

SDA_pin = board.IO20
#SDA_pin.pull = digitalio.Pull.UP
SCL_pin = board.IO21
#SCL_pin.pull = digitalio.Pull.UP
# Create the I2C interface.
i2c = busio.I2C(SDA_pin, SCL_pin)

# Create the SSD1306 OLED class.
display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, addr=60)
# Alternatively you can change the I2C address of the device with an addr parameter:
#display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, addr=0x31)


display.fill(0)
display.text('Hello', 0, 0, 1)
display.text('World', 0, 10, 1)
display.show()

def poll_slot_sensor_until(timeout) -> bool:
    current_time_f = time.monotonic
    timeout_time = current_time_f() + timeout
    counter = 0
    while current_time_f() < timeout_time:
        counter+=1
        if slot_sensor.value:
            return True
    print(f"Numchecks:{counter}")
    return False

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

    if not button_a.value:  # Button A is pressed
        if not buttonPressed:
            buttonPressed = True
            button_click = True
    else:
        buttonPressed = False
    
    if poll_slot_sensor_until(1) or button_click:
        print("Object detected! 🚀")  # Fast response when object blocks sensor
        buttonPressed = True
        previous_lap_duration = last_lap_duration
        last_lap_duration = time.monotonic() - last_pressed_time
        last_pressed_time = time.monotonic()
        show_lap_time()
