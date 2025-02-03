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

SDA_pin = board.IO26
SCL_pin = board.IO25

# List I2C devices
def scanI2C():
    i2c = busio.I2C(SDA_pin, SCL_pin)
    i2c.try_lock()
    print(i2c.scan())
    i2c.unlock()
    i2c.deinit()

#scanI2C()

# Initialise I2C bus.
i2c = busio.I2C(board.IO26, board.IO25)  # uses board.SCL and board.SDA
from lcd.lcd import CursorMode

# Talk to the LCD at I2C address 0x27.
# The number of rows and columns defaults to 4x20, so those
# arguments could be omitted in this case.
lcd = LCD(I2CPCF8574Interface(i2c, 0x27), num_rows=4, num_cols=20)

lcd.set_cursor_pos(0, 0)
lcd.print("Hello racers!")
time.sleep(2)
lcd.clear()

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
    lcd.set_cursor_pos(0, 0)
    timeStr = f"Last Lap:{previous_lap_duration}"
    lcd.print(timeStr)
    
    lcd.set_cursor_pos(1, 0)
    timeStr = f"Lap: {last_lap_duration}"
    lcd.print(timeStr)

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