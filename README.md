# RAVPower RP-WD009 MCU Utility

The RAVPower RP-WD009 has a communication interface to the power management IC via the MT7628 I2C bus.

The power management controller is located at address `0x0a`.

## Commands
### Read Device Status (0x01)
Reads the status of a requested Shutdown.
BIT 2 indicates low temperature - Code isn't clear on this (I do not want to freeze my device)
BIT 3 indivates high temperature
BIT 5 indicates low voltage
BIT 6 indicates charging
BIT 7 indicates a requested shutdown.

### Send Shutdown (0x03)
Powers down MCU and CPU.
Only works when called after or prior to Power LED (0x61).
Returns 0x0

### Send Powerdown  (0x05)
Does nothing (?)

### Read Battery Capacity (0x10)
Reads the Battery Capacity. Returns Values 0 - 100 as unsigned 8-bit integer.

### Disable Poweroff Key (0x40)
Will enable an interrupt (change to HIGH) on GPIO39.
Indicates the Power button is pressed for 3 seconds.
Returns 0x42.

### Enabled Poweroff Key (0x41)
Returns 0x43

### Restart (0x60)
Resets the CPU.

### Power LED (0x61)
Blinks the Power LED.
Return 0x0
