# SuperKeeks's NES Emulator #

I've always found emulators fascinating. The fact you can play one platform within a completely different hardware architecture was magic to me. For this reason, I decided to write my own emulator, so I could get a decent grasp of how they work under the hood.

As for the platform, I picked the NES. It felt the right balance between complexity for the job (easy and well documented hardware and features) and nostalgia (the NES was my first ever console)

During this trip I learnt things like:

* How old game systems used to work and how games were made for them.
* Refreshed my assembly language knowledge (which I haven't used since uni) and learnt 6502 assembly.
* Also refreshed my hardware knowledge (again not used since uni)
* Implemented low level audio code for the first time in my life.
* Learned the basics of the SDL2 game engine (input, graphics and sound)
* Writing unit tests from day 1 (and writing unit tests in general, which I didn't do much before this)

Overall it was an amazing experience I recommend to everyone that has a good programming level (not because any of its code is particularly complex, but it has to be done VERY CAREFULLY and cleanly) and basic hardware knowledge.

After having the Emulator basics (graphics and input but no audio) working, I gave a couple of tech talks both within the company I work for (Mediatonic Games) and weekend-long Madrid event "Freakend". You can find the last version of the slides [here](https://github.com/SuperKeeks/NESemu/blob/readme/doc/Kike%20Alcor%20-%20Creating%20a%20NES%20emulator%20v2.pdf). Since this presentation, I've implemented audio (APU) and a couple extra "memory mappers".

**Disclaimer:** Most of the code has been written from scratch without following any specific tutorial, papers, etc.. Because of this, there's probably (100% sure!) better ways of doing this. Please also keep in mind that despite I've done some optimizations to the code, it wasn't created with performance as a main objective and it is not intended to be used in "real world" applications.

# Features #

* iNES (.nes) rom parsing.
* 6502 CPU implementation. It respects original CPU cycles, so timing should be fairly good in Release mode.
* PPU (graphics chip) implementation.
* APU (audio chip) implementation.
* SRAM (save RAM) supported, but untested.
* Support for 001, 003 and 004 [memory mappers](https://wiki.nesdev.com/w/index.php/Mapper) which allow to play games like Super Mario Bros 1 and 3, Galaga, Donkey Kong, Original Mario Bros...
* Unit testing of all of the CPU instructions.

# Default input #
Keyboard input (hardcoded):
* D-Pad: Arrow keys
* A: 'x' key
* B: 'z' key
* Select: Right shift
* Start: Enter

There's also gamepad support (mappings thanks to [SDL_GameControllerDB](https://github.com/gabomdq/SDL_GameControllerDB))

# Architecture #
TODO

# Project setup #

I coded most of the emulator coding using Windows 10 and **Visual Studio Professional 2017**, although I currently use **Visual Studio Community 2019** and everything seems to work fine.

You'll need to install support for **C++ desktop applications** as well as the following components:
* Windows 10 SDK (10.0.17134.0)
* v141 VS 2017 C++ Build Tools for x64/x86

Please note that currently the code only supports compiling for 32 bit. Adding support for 64 bit shouldn't be too problematic though.
Both Debug and Release builds compile and work fine, but Debug builds run at a VERY slow framerate, so to properly play the game you'll need to use Release mode.

# Bibliography #
During the development of the emulator I mostly used these resources:

* [NesDev Wiki](http://wiki.nesdev.com/w/index.php/NES_reference_guide)
* [David A. Wheeler's Assembly In One Step guide](https://dwheeler.com/6502/oneelkruns/asm1step.html)
* [6502.org's opcode list](http://www.6502.org/tutorials/6502opcodes.html)
* [Nestech.txt](https://wiki.nesdev.com/w/index.php/Nestech.txt)

# Contact #
You can find me on Twitter [@SuperKeeks](https://twitter.com/SuperKeeks) or send me an email to ealcor(AT)gmail(DOTCOM)
