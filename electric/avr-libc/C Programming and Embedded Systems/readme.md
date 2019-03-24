# 课程

- [AVR嵌入式课程](https://www.csee.umbc.edu/~alnel1/cmpe311/)

- ATmega169p
    - From megaAVRfamily Families Similar group of devices
    - “P” of 169P means “Low Power”
    - “16” in 169P means 16k program memory▫
    - “9” in 169P means 9thdesign revision

- I/O Ports
    - Pxn: represents nth bit in Port x▫E.g. PA6 == 6th bit of Port A
    - If DDxnis a:
        - 1 –Pxn is configured to be an output pin
        - 0 –Pxn is configured to be an input pin
    - If DDxnis configured as output and PORTxnis:
        - 1 –Pxn is driven high (1)
        - 0 –Pxn is driven low  (0)
    - **Note: “writing” a logic 1 to a bit in the PINxRegister will toggle the corresponding bit in the data**

## Interrupts

Multiple events may be mapped to a single routine

    - i.e. any PORTA pin change

