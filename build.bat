tasm /m5 /l com.asm
tasm /m5 /l int9h.asm
if exist game.lib del game.lib
tlib game.lib +int9h +com