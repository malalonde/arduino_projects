import busio
import displayio
import terminalio
import fourwire
import board
import digitalio
import time
import busio
from lcd.lcd import LCD
from lcd.i2c_pcf8574_interface import I2CPCF8574Interface

# Import the SSD1306 module.
import adafruit_ssd1306

button_a = digitalio.DigitalInOut(board.IO10)
button_a.direction = digitalio.Direction.INPUT
button_a.pull = digitalio.Pull.UP

slot_sensor_a = digitalio.DigitalInOut(board.IO5)
slot_sensor_a.direction = digitalio.Direction.INPUT
slot_sensor_a.pull = digitalio.Pull.DOWN

slot_sensor_b = digitalio.DigitalInOut(board.IO7)
slot_sensor_b.direction = digitalio.Direction.INPUT
slot_sensor_b.pull = digitalio.Pull.DOWN

def scanI2C():
    i2c = busio.I2C(board.IO21, board.IO20)
    i2c.try_lock()
    print(i2c.scan())
    i2c.unlock()
    i2c.deinit()

#scanI2C()

SDA_pin = board.IO21
SCL_pin = board.IO20
#set update frequency higher to help with slow lcd refresh rate
i2c = busio.I2C(SDA_pin, SCL_pin, frequency=200_000)

lcd = LCD(I2CPCF8574Interface(i2c, 39), num_rows=2, num_cols=16)

print("Starting program.")

lcd.set_cursor_pos(0, 0)
lcd.print("Hello racers!")
time.sleep(2)
lcd.clear()

start_time = time.monotonic()
lap_time_a = start_time
lap_duration_a = lap_time_a - start_time
lap_time_b = start_time
lap_duration_b = lap_time_a - start_time
best_lap_duration = 0.0
buttonPressed = False

def show_best_lap_time(time, track):
    timeStr = "Best:"
    if best_lap_duration > 0:
        timeStr += f"{time:6.3f} ({track})"
    else:
        timeStr += "           " #empty space to clear these characters on the LCD
    lcd.set_cursor_pos(1, 0)
    lcd.print(timeStr)

def poll_slot_sensor_until(timeout) -> bool:
    global lap_duration_a
    global lap_time_a
    global lap_duration_b
    global lap_time_b
    global best_lap_duration
    
    current_time_f = time.monotonic
    timeout_time = time.monotonic() + timeout
    counter = 0
    nothing_in_track_a = False
    nothing_in_track_b = False
    while time.monotonic() < timeout_time:
        counter+=1
        if slot_sensor_a.value and nothing_in_track_a:
            lap_duration_a = time.monotonic() - lap_time_a
            lap_time_a = time.monotonic()
            lcd.set_cursor_pos(0, 1)
            lcd.print(f"{lap_duration_a:6.3f}")
            if(lap_duration_a < best_lap_duration) or best_lap_duration == 0.0:
                best_lap_duration = lap_duration_a
                show_best_lap_time(best_lap_duration, "A")
            return True
        if not slot_sensor_a.value:
            nothing_in_track_a = True
            
        if slot_sensor_b.value and nothing_in_track_b:
            lap_duration_b = time.monotonic() - lap_time_b
            lap_time_b = time.monotonic()
            lcd.set_cursor_pos(0, 9)
            lcd.print(f"{lap_duration_b:6.3f}")
            if(lap_duration_b < best_lap_duration) or best_lap_duration == 0.0:
                best_lap_duration = lap_duration_b
                show_best_lap_time(best_lap_duration, "B")
            return True
        if not slot_sensor_b.value:
            nothing_in_track_b = True
            
    return False

# Initialise LCD text
lcd.clear()
timeStr = f"A{lap_duration_a:6.3f} B{lap_duration_b:6.3f}"

timeStr += "\nBest:"
if best_lap_duration > 0:
    timeStr += f"{best_lap_duration:6.3f} ({best_track_str})"
lcd.set_cursor_pos(0, 0)
lcd.print(timeStr)

# Update loop
while True:

    if not button_a.value:  # Button A is pressed
        if not buttonPressed:
            buttonPressed = True
            best_lap_duration = 0.0
            show_best_lap_time(best_lap_duration, " ")
    else:
        buttonPressed = False

    detected = poll_slot_sensor_until(5)
    #if detected:
        #print("Object detected! 🚀")  # Fast response when object blocks sensor
