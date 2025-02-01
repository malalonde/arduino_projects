"""Simple test for 16x2 character lcd connected to an MCP23008 I2C LCD backpack."""
import time
import board
import busio
from lcd.lcd import LCD
from lcd.i2c_pcf8574_interface import I2CPCF8574Interface

# Modify this if you have a different sized Character LCD
lcd_columns = 16
lcd_rows = 2

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
import board


from lcd.lcd import CursorMode

# Talk to the LCD at I2C address 0x27.
# The number of rows and columns defaults to 4x20, so those
# arguments could be omitted in this case.
lcd = LCD(I2CPCF8574Interface(i2c, 0x27), num_rows=4, num_cols=20)

# Start at the second line, fifth column (numbering from zero).
lcd.set_cursor_pos(1, 4)
lcd.print("Here I am")
time.sleep(2)
lcd.clear()

lcd.set_cursor_pos(0, 0)
lcd.print("Hi Patrick!")
time.sleep(2)

for i in range(100):

    lcd.set_cursor_pos(1, 0)
    timeStr = f"({i})Time:{time.monotonic()}"
    lcd.print(timeStr)
    time.sleep(1.0)