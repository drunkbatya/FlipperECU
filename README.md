# FlipperECU

## About

This project is the answer to the question "Can Flipper Zero act as an ECU?" that emerged in my head. Yes, it can.

It can control injection and ignition reading MAP, IAT, CLT, CKPS, TPS, and other sensors at at least 6k RPM, calculating injection time via speed-density algorithm, and you can tune volumetric efficiency, ignition angle, and any other available maps in real time directly from the Flipper (or via screen-streaming via BLE or USB connection, but this is optional).

Tested on ZAZ968M V4 engine in semi-sequential mode.

As a result of the initial goal, this project stuck at the PoC state right after I've reached "14.7" AFR in ~30% VE dots, but this is enough to let my car be on the road. You can test this app on your Flipper Zero if it has hardware number "f18" and doubled GPIO pins.

## Overview

This project consists of expansion module (schematic, gerber etc.) and FAP (Flipper App Package) sources available in [hardware](hardware/) and [firmware](firmware/) directories respectively.

TODO: add a photos and GIFs
