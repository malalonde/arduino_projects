import board
import neopixel
import digitalio
import time

print(dir(board))

print("Hello Patrick")

# Pin where NeoPixel is connected
PIN = board.IO12
# Number of LEDs in the strip
NUM_PIXELS = 64
# Button pins
BUTTON_A_PIN = board.IO14
BUTTON_B_PIN = board.IO27

# Initialize NeoPixel strip
strip = neopixel.NeoPixel(PIN, NUM_PIXELS, brightness=0.5, auto_write=False)

# Button states
button_a = digitalio.DigitalInOut(BUTTON_A_PIN)
button_a.direction = digitalio.Direction.INPUT
button_a.pull = digitalio.Pull.UP

button_b = digitalio.DigitalInOut(BUTTON_B_PIN)
button_b.direction = digitalio.Direction.INPUT
button_b.pull = digitalio.Pull.UP

# Colors
red_color = (50, 0, 0)
green_color = (0, 50, 0)
yellow_color = (50, 50, 0)
black_color = (0, 0, 0)

# State variable
BWasPressed = False

def show_range(start, length, color):
    for i in range(start, start + length):
        strip[i] = color
    strip.show()

def initialize():
    strip.fill(black_color)
    strip.show()

def go():
    strip.fill(black_color)
    strip.show()

    ranges = [0, 8, 16, 24, 32, 40, 48, 56]
    for i in range(0, 8, 2):
        show_range(ranges[i], 8, red_color)
        show_range(ranges[i+1], 8, red_color)
        time.sleep(1)

    strip.fill(green_color)
    strip.show()
    time.sleep(2)

    strip.fill(black_color)
    strip.show()

# Initialize the strip
initialize()

while True:
    if not button_a.value:  # Button A is pressed
        go()

    if not button_b.value:  # Button B is pressed
        BWasPressed = True
        strip.fill(yellow_color)
        strip.show()
        time.sleep(0.1)
        strip.fill(black_color)
        strip.show()
        time.sleep(0.1)
    elif BWasPressed:
        BWasPressed = False
        strip.fill(red_color)
        strip.show()
        time.sleep(1)




        