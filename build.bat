tasm /m5 /l com.asm
tasm /m5 /l int9h.asm
tasm /m5 /l mouse.asm
tasm /m5 /l timer.asm
if exist game.lib del game.lib
tlib game.lib +mouse +timer +int9h +com