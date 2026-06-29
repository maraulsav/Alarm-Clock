# Alarm-Clock
Hello guysss, I have tried to make the alarm clock. But sadly it cannot work properly. I have tried everything that I can think of and the result is still like the picture below.. I have tried to documented every trial and error that I encounter in my Builder Journal.md file
<img width="1171" height="604" alt="Screenshot 2026-06-29 at 13 06 16" src="https://github.com/user-attachments/assets/92b37a3a-3ceb-4a30-87ff-8f02a9baa4ac" />
As I have tried everything that possible for me like fixing  wiring diagra and code, fixing soldering issue, and yet its still not working. I think this project can be not working because of component issue, especially the TFT display because one of the things that I tried is to switch the power supply to 5V because I thought perhaps 3.3V is too low for this.. and it didnot work it jsut shut down completely at that trial (I think if thats the cause it can damage the internal component). Some suggestion for you guys is that dont be in rush to switch up your voltage supply because it may messed with the internal component signal. I think thats why the display that shown is gibberish.

but, I still wanted to said a special thanks to hackclub especially stasis that allow me to build the idea that I had. Thankss also for anyone of you guys that read this documentation.
Youtube documentation: https://youtu.be/MWKya9hIei4

Hii in this project I wanted to built an alarm clock that can display in real time (so it needed to connect to a wifi that can connect to my phone), also can gave sound to alert me, detect air quality and temperature and also I want it to function as a night lamp (I kind of scared of the dark wkwkwk).
Here is the look of my overal project: 
<img width="472" height="496" alt="Screenshot 2026-06-16 at 17 05 35" src="https://github.com/user-attachments/assets/708b45c6-ab57-4117-986a-0be0fa10ed43" />

## Features : 
- Screen Display: TFT Display
- Humidity and Temperature Sensor: ENS160+AHT21 Sensor 
- Microcontroller: ESP32C3 Super Mini
- Type C charging module
- Speaker box
- LED lamp

## Hardware
Here I made the connection of the alarm clock, I dont use PCB because as I am not working in a tight space I think its kinda unecesary to use a PCB, and it can also cut cost anyway
Here's wiring diagram

<img width="517" height="290" alt="Screenshot 2026-06-16 at 17 18 09" src="https://github.com/user-attachments/assets/448767f7-ad97-4205-b812-5256644b087b" />


## Case overview
In creating my case, I had measured the component and like designing the case so the connection wont lose, especially for the type c charging module
Below is the pincture of my case, there I plan to connect the two part (front and back) using the heat set insert

This is the front part of my case:
<img width="648" height="633" alt="Screenshot 2026-06-16 at 17 09 26" src="https://github.com/user-attachments/assets/6df2ee0d-ce4e-4a47-b9e5-24f251bcc952" />


This is the back part of my case:

<img width="556" height="634" alt="Screenshot 2026-06-16 at 17 08 55" src="https://github.com/user-attachments/assets/eec624dd-f2ed-451e-8044-a1bf74cc0bc0" />


## Code Overview
This project used the Arduino IDE as the main platform to wrote the codes. 
The main feature of my code is:
- User can select menu / mode
- User can set the alarm
- The clock is connected to wifi from the phone so can update in real time
- The clock can also display the current air quality and temperature

## BOM
| Name | Purpose | Qty | Cost (USD) | Distributor |
|------|----------|-----|------------|-------------|
| M4 bolts with flat head | To connect the 3D printed parts | 1 | $0.93 | Tokopedia |
| Heat insert | To securely connect the 3D print | 1 | $3.78 | Tokopedia |
| Type-C charging module | Safely recharges single-cell lithium-ion batteries | 1 | $0.68 | Tokopedia |
| Switch | Toggle components on and off | 3 | $0.30 | Tokopedia |
| Jumper wire (male-to-male) | Connect one part to another | 2 | $1.41 | Tokopedia |
| LED COB Lamp | Light source for the clock (mirror lighting/night lamp) | 1 | $0.80 | Tokopedia |
| Speaker box (2x) | Producing sound | 1 | $3.04 | Tokopedia |
| 3.7 V Battery | Power supply | 2 | $1.61 | Tokopedia |
| ENS160 + AHT21 Sensor | Detect air quality and temperature | 1 | $5.04 | Tokopedia |
| ESP32-C3 | Main microcontroller | 1 | $2.00 | Tokopedia |
| TFT Display Module with Rotary Encoder | Display information and adjust the alarm | 1 | $19.15 | Tokopedia |

