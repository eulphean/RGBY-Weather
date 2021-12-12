# Current State notation
# Sunrise (A)
# Noon (B)
# Sunset (C)
# Midnight (D)

import configparser
import requests
import sys
import json

import serial
from datetime import datetime
from datetime import time as faketime
import time
import tzlocal

# Global variables
lastSunsetSunriseTime = -1;
currentState = "A"
timeToWait = 1 # time to wait before cecking for current time (every second)
timeToWaitSunriseSunset = 10 # 120 seconds or 2 minutes

def get_weather(location):
    api_key = '674113184e46c62a71940e4a543e4ea7'
    url = "https://api.openweathermap.org/data/2.5/weather?q={}&units=metric&appid={}".format(location, api_key)
    r = requests.get(url)
    return r.json()

# Get current time and evaluate what state to the Arduino
def update_State(curTime, sunrise, sunset): 
    global lastSunsetSunriseTime
    global currentState

    # Movement of the states between mighnight, sunrise, noon, and sunset
    if (currentState == 'D'):
        if (curTime.hour == sunrise.hour and curTime.minute == sunrise.minute): # Begin sunrise rhythm
            currentState = 'A'
            lastSunsetSunriseTime = time.time() # Start tracking time

    if (currentState == 'A'):
        if (time.time() - lastSunsetSunriseTime >= timeToWaitSunriseSunset):
            currentState = 'B'
            lastSunsetSunriseTime = -1 #Reset this counter

    if (currentState == 'B'):
        if (curTime.hour == sunset.hour and curTime.minute == sunset.minute): # Beging sunset rhythm
            currentState = 'C'
            lastSunsetSunriseTime = time.time() # Start tracking time

    if (currentState == 'C'):
        elapsedTime = time.time() - lastSunsetSunriseTime
        if (elapsedTime >= timeToWaitSunriseSunset):
            currentState = 'D'
            lastSunsetSunriseTime = -1

    return currentState

# Get a rough estimate about what's the current state of the system. 
def get_currentState(sunrise, sunset): 
    global currentState

    # Current time.
    now = datetime.now()

    # Sunrise
    if (now.hour == sunrise.hour and now.minute == sunrise.minute):
        currentState = 'A'

    # Noon
    if (now.hour == sunrise.hour and now.minute > sunrise.minute):
        currentState = 'B'

    if (now.hour > sunrise.hour):
        currentState = 'B'

    # Sunset
    if (now.hour == sunset.hour and now.minute == sunset.minute):
        currentState = 'C'

    # Midnight
    if (now.hour == sunset.hour and now.minute > sunset.minute):
        currentState = 'D'

    if (now.hour > sunset.hour or now.hour < sunrise.hour):
        currentState = 'D'

    if (now.hour == sunrise.hour and now.minute < sunrise.minute):
        currentState = 'D'

    return currentState

def get_sunriseTime(weather): 
    time = weather['sys']['sunrise']
    time = datetime.fromtimestamp(float(time), tzlocal.get_localzone())
    time = faketime(23, 11, 0)
    return time

def get_sunsetTime(weather):
    time = weather['sys']['sunset']
    time = datetime.fromtimestamp(float(time), tzlocal.get_localzone())
    time = faketime(23, 13, 0)
    return time

def main():
    global lastSunsetSunriseTime
    global currentState

    # Define serial communication with Arduino
    ser = serial.Serial('/dev/cu.usbmodem1421', 9600)

    lastTime = time.time() # To track elapsed time. 
    lastSunsetSunriseTime = -1

    # Set location and get the weather. 
    location = "Chicago"
    weather = get_weather(location)

    # Get sunrise/sunset times and convert them into local timezone times
    sunrise = get_sunriseTime(weather)
    sunset = get_sunsetTime(weather)

    print("Sunrise, Sunset: {}, {}".format(sunrise, sunset))

    # Get the current state before evaluating the next state.
    currentState = get_currentState(sunrise, sunset)
    if (currentState == 'A' or currentState == 'C'):
        lastSunsetSunriseTime = time.time()

    # Get current time, evaluate the state and send it to Arduino
    while (1):
       # Current time
       curTime = datetime.now()
       
       elapsedTime = time.time() - lastTime
       if (elapsedTime >= timeToWait): # Check current time and evaluate state
            state = update_State(curTime, sunrise, sunset)
            print ("Current State: {}".format(state))
            lastTime = time.time() # Reset time because it has elapsed. 
            ser.write(state)

       # Everyday at 3am, get the new sunset and sunrise times and keep looping
       if (curTime.hour == 3 and curTime.minute == 0 and curTime.second == 0):
            # Set location and get the weather. 
            weather = get_weather(location)
            sunrise = get_sunriseTime()
            sunset = get_sunsetTime()


if __name__ == '__main__':
    main()