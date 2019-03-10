
# using arduino lib for regular AVR code

[using-the-arduino-libraries-for-regular-avr-code](https://stackoverflow.com/questions/13260508/using-the-arduino-libraries-for-regular-avr-code)

[arduino-base.mk](https://git.vnks.de/uli/arduino_core/raw/branch/master/arduino-base.mk)

- demo

`c:\home\notes\电子\arduino\avrgcc_using_arduinolib>C:\tools\msys64\mingw64\bin\mingw32-make.exe`

- vscode 编辑环境

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "C:/prog/Arduino/hardware/tools/avr/avr/include",
                "${workspaceFolder}/**",
                "C:/prog/Arduino/hardware/tools/avr/lib/gcc/avr/4.9.2/include",
                "C:/prog/Arduino/hardware/arduino/avr/cores/arduino",
                "C:/prog/Arduino/hardware/arduino/avr/variants/standard",
                "C:/prog/Arduino"
            ],
            "defines": [
                "__AVR_ATmega16__",
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "windowsSdkVersion": "10.0.17134.0",
            "compilerPath": "C:/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/VC/Tools/MSVC/14.15.26726/bin/Hostx64/x64/cl.exe",
            "cStandard": "c99",
            "cppStandard": "c++17",
            "intelliSenseMode": "msvc-x64"
        }
    ],
    "version": 4
}

```