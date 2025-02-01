"""Simple test for 16x2 character lcd connected to an MCP23008 I2C LCD backpack."""
import time
import board
import busio
from lcd.lcd import LCD
from lcd.i2c_pcf8574_interface import I2CPCF8574Interface
import digitalio

button_a = digitalio.DigitalInOut(board.IO27)
button_a.direction = digitalio.Direction.INPUT
button_a.pull = digitalio.Pull.UP

# List I2C devices
def scanI2C():
    i2c = busio.I2C(board.IO14, board.IO12)
    i2c.try_lock()
    print(i2c.scan())
    i2c.unlock()
    i2c.deinit()

scanI2C()

# Initialise I2C bus.

i2c = busio.I2C(board.IO14, board.IO12)  # uses board.SCL and board.SDA
from lcd.lcd import CursorMode

# Talk to the LCD at I2C address 0x27.
# The number of rows and columns defaults to 4x20, so those
# arguments could be omitted in this case.
lcd = LCD(I2CPCF8574Interface(i2c, 0x27), num_rows=4, num_cols=20)

lcd.set_cursor_pos(0, 0)
lcd.print("Hi Patrick!")
time.sleep(2)

start_time = time.monotonic()
last_pressed_time = start_time
last_lap_duration = last_pressed_time - start_time
previous_lap_duration = last_lap_duration
buttonPressed = False

while True:
    if not button_a.value:  # Button A is pressed
        if not buttonPressed:
            buttonPressed = True
            previous_lap_duration = last_lap_duration
            last_lap_duration = time.monotonic() - last_pressed_time
            last_pressed_time = time.monotonic()
    else:
        buttonPressed = False
    
    lcd.set_cursor_pos(0, 0)
    timeStr = f"Last Lap:{previous_lap_duration}"
    lcd.print(timeStr)
    time.sleep(0.1)
    
    lcd.set_cursor_pos(1, 0)
    timeStr = f"Lap: {last_lap_duration}"
    lcd.print(timeStr)
    

    