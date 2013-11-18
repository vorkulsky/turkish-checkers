tasm /m5 /l com.asm
tasm /m5 /l int9h.asm
tasm /m5 /l mouse.asm
if exist game.lib del game.lib
tlib game.lib +mouse +int9h +com