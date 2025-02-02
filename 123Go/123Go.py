import board
import neopixel
import digitalio
import time

#print(dir(board))

# Button pins
BUTTON_A_PIN = board.IO14
BUTTON_B_PIN = board.IO27

# Initialize NeoPixel led_matrix
led_matrix = neopixel.NeoPixel(board.IO12, 64, brightness=0.5, auto_write=False)

num_leds_in_strip = 150
strip = neopixel.NeoPixel(board.IO26, num_leds_in_strip, brightness=0.1, auto_write=False)

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
blue_color = (0, 0, 50)

# State variable
BWasPressed = False

animation_start = time.monotonic()
animation_duration = 10
animation_moving = True
last_animation_time = animation_start
led_index = 0
led_color = red_color
led_enabled = True
led_distance = 5

def show_led_strip(index = 0, color = None):
    global led_color
    if color is not None:
        led_color = color
    strip.fill(black_color)
    for i in range(index,num_leds_in_strip,led_distance):
        strip[i] = led_color
    strip.show() 
    

def show_range(start, length, color):
    for i in range(start, start + length):
        led_matrix[i] = color
    led_matrix.show()

def initialize():
    led_matrix.fill(black_color)
    led_matrix.show()

# Initialize the led_matrix
initialize()

def start_animation(i_color, duration, moving):
    global animation_moving
    global animation_start
    global animation_duration
    global led_color
    global led_enabled
    animation_moving = moving
    animation_duration = duration
    animation_start = time.monotonic()
    led_enabled = True
    led_color = i_color

start_animation(blue_color, 10, True)

def update_led_strip():
    global last_animation_time
    global led_index
    global led_enabled
    if time.monotonic() - animation_start > animation_duration:
        led_enabled = False;
        show_led_strip(0, black_color)
    
    if time.monotonic() - last_animation_time > 0.05:
        print(f"turning on led {led_index}")
        led_distance = 5
        last_animation_time = time.monotonic()
        if animation_moving:
            led_index += 1
        led_index  = led_index % led_distance
        if led_enabled:
            show_led_strip(led_index)

def go():
    led_matrix.fill(black_color)
    led_matrix.show()

    strip.fill(black_color)
    show_led_strip(0, red_color)

    ranges = [0, 8, 16, 24, 32, 40, 48, 56]
    for i in range(0, 8, 2):
        show_range(ranges[i], 8, red_color)
        show_range(ranges[i+1], 8, red_color)
        time.sleep(1)

    led_matrix.fill(green_color)
    led_matrix.show()
    
    start_animation(green_color, 10, True)
    
    start = time.monotonic()
    while time.monotonic() - start < 2:
        update_led_strip()
    
    led_matrix.fill(black_color)
    led_matrix.show()

while True:
    if not button_a.value:  # Button A is pressed
        go()

    if not button_b.value:  # Button B is pressed
        BWasPressed = True
        led_matrix.fill(yellow_color)
        led_matrix.show()
        time.sleep(0.1)
        led_matrix.fill(black_color)
        led_matrix.show()
        time.sleep(0.1)
        
        start_animation(yellow_color, 1, True)
    
    elif BWasPressed:
        BWasPressed = False
        led_matrix.fill(red_color)
        led_matrix.show()
        strip.fill(black_color)
        show_led_strip(0, red_color)
        start_animation(red_color, 10, False)
        time.sleep(1)
        
    update_led_strip()



        