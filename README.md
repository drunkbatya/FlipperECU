# FlipperECU

## About

This project is the answer to the question "Can Flipper Zero act as an ECU?" that emerged in my head. Yes, it can.

It can control injection and ignition reading MAP, IAT, CLT, CKPS, TPS, and other sensors at at least 6k RPM, calculating injection time via speed-density algorithm, and you can tune volumetric efficiency, ignition angle, and any other available maps in real time directly from the Flipper (or via screen-streaming via BLE or USB connection, but this is optional).

Tested on ZAZ968M V4 engine in semi-sequential mode.

As a result of the initial goal, this project stuck at the PoC state right after I've reached "14.7" AFR in ~30% VE dots, but this is enough to let my car be on the road. You can test this app on your Flipper Zero if it has hardware number "f18" and doubled GPIO pins.

## Overview

This project consists of expansion module (schematic, gerber etc.) and FAP (Flipper App Package) sources available in [hardware](hardware/) and [firmware](firmware/) directories respectively.

## Expansion module

Schematic developed by [me](https://github.com/drunkbatya) and inspired by [microRusEfi hardware](https://github.com/rusefi/hw_microRusEfi). PCB completely developed by [AlexKaut](https://github.com/AlexKaut).

### EasyEDA render
<picture>
    <img
        alt="expansion module easyeda"
        src="/.github/assets/board_easyeda_2.png"
        width="480"
    >
</picture>

### Board IRL
<picture>
    <img
        alt="board ird"
        src="/.github/assets/board_irl_1.png"
        width="480"
    >
</picture>

### Board testing
<picture>
    <img
        alt="board testing"
        src="/.github/assets/board_testing_1.png"
        width="480"
    >
</picture>

## Software

Software developed by [me](https://github.com/drunkbatya) and inspired by [rusefi firmware](https://github.com/rusefi/rusefi) (guys, you're awesome!) and [Flipper Zero firmware](https://github.com/flipperdevices/flipperzero-firmware).

### App overview
<picture>
    <img
        alt="app overview"
        src="/.github/assets/gif_app_basic.gif">
</picture>
