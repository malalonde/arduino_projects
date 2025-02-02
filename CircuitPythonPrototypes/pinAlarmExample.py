import board
import digitalio
import alarm
import time

# Set up the sensor pin
SENSOR_PIN = board.D5  # Change this to match your ESP32 pin
sensor = digitalio.DigitalInOut(SENSOR_PIN)
sensor.direction = digitalio.Direction.INPUT
sensor.pull = digitalio.Pull.UP  # Enable pull-up resistor (optional)

# Configure a PinAlarm to trigger on LOW (object blocking sensor)
pin_alarm = alarm.pins.PinAlarm(pin=SENSOR_PIN, value=False, edge=True)

print("Monitoring LM393 sensor with PinAlarm...")

while True:
    # Check if the pin alarm has been triggered
    if alarm.wake_alarm == pin_alarm:
        print("Object detected! 🚀")  # Fast response when object blocks sensor
        alarm.wake_alarm = None  # Reset alarm so it can trigger again

    # Main program can continue running here (non-blocking)
    print("Main loop running...")  
    time.sleep(0.01)  # Small delay to prevent CPU overload