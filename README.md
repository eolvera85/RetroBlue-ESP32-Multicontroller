# RetroBlue-ESP32-Multicontroller
Convert an ESP32 as ProController/NES Classic/SNES Classic/N64 Classic

As soon as the ESP32 is turned on it syncs with the first controller and each time the sync button is pressed it will switch to each of the controllers listed, on the next reboot esp32 syncs with the last controller

### Emulated controllers
| |Emulated controller |
| ----------------- | ----- |
| 1 | Pro Controller (Splatoon Type) |
| 2 | Nintendo |
| 3 | Super Nintendo |
| 4 | Pro Controller (Trigger button ZR/ZL *Developing*) |
| 5 | Nintendo 64 (Stick original *Developing*) |
| 6 | Famicom |
| 7 | Super Famicom |
| 8 | Nintendo 64 (Stick mod type PSX/PS2) |

### Default GPIOs
| GPIO | Button |
| ----------------- | ----- |
| 3 | Stick Button Left |
| 4 | Capture |
| 5 | Y |
| 12 | - |
| 13 | + |
| 14 | Dpad Right |
| 15 | Sync |
| 16 | Home |
| 17 | X |
| 18 | A |
| 19 | B |
| 21 | Stick Button Right |
| 22 | R |
| 23 | L  |
| 25 | Dpad Up |
| 26 | Dpad Down |
| 27 | Dpad Left |
| 32 | ZL |
| 33 | ZR |
| 34 | Right Stick X Axis |
| 35 | Right Stick Y Axis |
| 36 | Left Stick X Axis |
| 39 | Left Stick Y Axis |

### Soft-buttons:
To avoid making unnecessary holes in the case, virtual buttons are added through a combination of these

| Button/Controller | PRO | NES | SNES | N64 |
| ----------------- | ----- | ----- | ----- | ----- |
| L | | (+) + B | | |
| R | | (+) + A | | |
| ZL | | | (+) + L | (+) + L |
| ZR | | | (+) + R | (+) + R |
| Capture | (+) + L + R | | | (+) + L + R |
| Home | | | |  L + R |


### Notes:
- It is possible to have to invert polarity in the sticks
- For the SYNC Led put a 220 ohm resistor to GND (GPIO 1)

Thanks to [@David](https://twitter.com/XGAMES_VJ) for collaborating in the implementation of each of the controllers.... ¡ Camarada a seguirle dando !
- https://youtu.be/ALRYgAbv74g
- https://youtu.be/ZtqH2gqiSqs
- https://youtu.be/MgMhc6KOfMQ
- https://youtu.be/NFHStgUDQbE

Resources used:
- https://github.com/HandHeldLegend/RetroBlue-ESP32

Thank you to [@Mitchell](https://github.com/mitchellcairns) for the developed API.... ¡ Gracias amigo, Éxito !