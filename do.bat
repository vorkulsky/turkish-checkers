tasm /m5 /l game
tcc -S game_c.c
tcc -c game_c.c
tlink /t /x game.obj game_c.obj,,,game.lib