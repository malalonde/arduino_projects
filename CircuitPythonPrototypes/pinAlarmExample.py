import board
import digitalio
import alarm
import time

# Set up the sensor pin
SENSOR_PIN = board.IO32  # Change this to match your ESP32 pin
sensor = digitalio.DigitalInOut(SENSOR_PIN)
sensor.direction = digitalio.Direction.INPUT
sensor.pull = digitalio.Pull.UP  # Enable pull-up resistor (optional)

sensor.deinit()

# Configure a PinAlarm to trigger on LOW (object blocking sensor)
pin_alarm = alarm.pin.PinAlarm(board.IO32, value=True, pull=False)
future_time = time.monotonic() + 1
timer_alarm = alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 1)

print("Monitoring LM393 sensor with PinAlarm...")

#while True:
for i in range(5):
    # Check if the pin alarm has been triggered
    alarm.light_sleep_until_alarms(pin_alarm, timer_alarm)
    print(alarm.wake_alarm)
    if alarm.wake_alarm == pin_alarm:
        print("Object detected! 🚀")  # Fast response when object blocks sensor
    if alarm.wake_alarm == timer_alarm:
        print("Wake from timer!")

    future_time = time.monotonic() + 1
    timer_alarm = alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 1)
        #alarm.wake_alarm = None  # Reset alarm so it can trigger again
    


    #if sensor.value:
    #    print("Value UP") 
    
    # Main program can continue running here (non-blocking)
    print("Main loop running...")  
    time.sleep(0.1)  # Small delay to prevent CPU overload