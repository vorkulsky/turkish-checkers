@echo off
if not exist game.obj tasm /m5 /l game
if exist gamec.asm del gamec.asm
if exist gamec.obj del gamec.obj
bcc -S -3 -O2 -d gamec.cpp
if exist gamec.asm bcc -c -3 -O2 -d gamec.cpp
if exist gamec.obj tlink /t /x game.obj gamec.obj,,,game.lib