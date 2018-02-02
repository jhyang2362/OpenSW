{"changed":true,"filter":false,"title":"game.c","tooltip":"/src/game.c","value":"#include \"game.h\"\n\nstatic void process_player(PacmanGame *game);\nstatic void process_fruit(PacmanGame *game);\nstatic void process_ghosts(PacmanGame *game);\nstatic void process_pellets(PacmanGame *game);\n\nstatic bool check_pacghost_collision(PacmanGame *game);     //return true if pacman collided with any ghosts\nstatic void enter_state(PacmanGame *game, GameState state); //transitions to/ from a state\nstatic bool resolve_telesquare(PhysicsBody *body);          //wraps the body around if they've gone tele square\n\nvoid game_tick(PacmanGame *game)\n{\n\tunsigned dt = ticks_game() - game->ticksSinceModeChange;\n\n\tswitch (game->gameState)\n\t{\n\t\tcase GameBeginState:\n\t\t\t// plays the sound, has the \"player 1\" image, has the \"READY!\" thing\n\n\t\t\tbreak;\n\t\tcase LevelBeginState:\n\t\t\t// similar to game begin mode except no sound, no \"Player 1\", and slightly shorter duration\n\n\t\t\tbreak;\n\t\tcase GamePlayState:\n\t\t\t// everyone can move and this is the standard 'play' game mode\n\t\t\tprocess_player(game);\n\t\t\tprocess_ghosts(game);\n\n\t\t\tprocess_fruit(game);\n\t\t\tprocess_pellets(game);\n\n\t\t\tif (game->pacman.score > game->highscore) game->highscore = game->pacman.score;\n\n\t\t\tbreak;\n\t\tcase WinState:\n\t\t\t//pacman eats last pellet, immediately becomes full circle\n\t\t\t//everything stays still for a second or so\n\t\t\t//monsters + pen gate disappear\n\t\t\t//level flashes between normal color and white 4 times\n\t\t\t//screen turns dark (pacman still appears to be there for a second before disappearing)\n\t\t\t//full normal map appears again\n\t\t\t//pellets + ghosts + pacman appear again\n\t\t\t//go to start level mode\n\n\t\t\tbreak;\n\t\tcase DeathState:\n\t\t\t// pacman has been eaten by a ghost and everything stops moving\n\t\t\t// he then does death animation and game starts again\n\n\t\t\t//everything stops for 1ish second\n\n\t\t\t//ghosts disappear\n\t\t\t//death animation starts\n\t\t\t//empty screen for half a second\n\n\t\t\tbreak;\n\t\tcase GameoverState:\n\t\t\t// pacman has lost all his lives\n\t\t\t//it displays \"game over\" briefly, then goes back to main menu\n\t\t\tbreak;\n\t}\n\n\t//\n\t// State Transitions - refer to gameflow for descriptions\n\t//\n\n\tbool allPelletsEaten = game->pelletHolder.numLeft == 0;\n\tbool collidedWithGhost = check_pacghost_collision(game);\n\tint lives = game->pacman.livesLeft;\n\n\tswitch (game->gameState)\n\t{\n\t\tcase GameBeginState:\n\t\t\tif (dt > 2200) enter_state(game, LevelBeginState);\n\n\t\t\tbreak;\n\t\tcase LevelBeginState:\n\t\t\tif (dt > 1800) enter_state(game, GamePlayState);\n\n\t\t\tbreak;\n\t\tcase GamePlayState:\n\n\t\t\t//TODO: remove this hacks\n\t\t\tif (key_held(SDLK_k)) enter_state(game, DeathState);\n\n\t\t\telse if (allPelletsEaten) enter_state(game, WinState);\n\t\t\telse if (collidedWithGhost) enter_state(game, DeathState);\n\n\t\t\tbreak;\n\t\tcase WinState:\n\t\t\t//if (transitionLevel) //do transition here\n\t\t\tif (dt > 4000) enter_state(game, LevelBeginState);\n\n\t\t\tbreak;\n\t\tcase DeathState:\n\t\t\tif (dt > 4000)\n\t\t\t{\n\t\t\t\tif (lives == 0) enter_state(game, GameoverState);\n\t\t\t\telse enter_state(game, LevelBeginState);\n\t\t\t}\n\n\t\t\tbreak;\n\t\tcase GameoverState:\n\t\t\tif (dt > 2000)\n\t\t\t{\n\t\t\t\t//TODO: go back to main menu\n\n\t\t\t}\n\t\t\tbreak;\n\t}\n}\n\nvoid game_render(PacmanGame *game)\n{\n\tunsigned dt = ticks_game() - game->ticksSinceModeChange;\n\n\t//common stuff that is rendered in every mode:\n\t// 1up + score, highscore, base board, lives, small pellets, fruit indicators\n\tdraw_common_oneup(true, game->pacman.score);\n\tdraw_common_highscore(game->highscore);\n\n\tdraw_pacman_lives(game->pacman.livesLeft);\n\n\tdraw_small_pellets(&game->pelletHolder);\n\tdraw_fruit_indicators(game->currentLevel);\n\n\t//in gameover state big pellets don't render\n\t//in gamebegin + levelbegin big pellets don't flash\n\t//in all other states they flash at normal rate\n\n\tswitch (game->gameState)\n\t{\n\t\tcase GameBeginState:\n\t\t\tdraw_game_playerone_start();\n\t\t\tdraw_game_ready();\n\n\t\t\tdraw_large_pellets(&game->pelletHolder, false);\n\t\t\tdraw_board(&game->board);\n\t\t\tbreak;\n\t\tcase LevelBeginState:\n\t\t\tdraw_game_ready();\n\n\t\t\t//we also draw pacman and ghosts (they are idle currently though)\n\t\t\tdraw_pacman_static(&game->pacman);\n\t\t\tfor (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);\n\n\t\t\tdraw_large_pellets(&game->pelletHolder, false);\n\t\t\tdraw_board(&game->board);\n\t\t\tbreak;\n\t\tcase GamePlayState:\n\t\t\tdraw_large_pellets(&game->pelletHolder, true);\n\t\t\tdraw_board(&game->board);\n\n\t\t\tif (game->gameFruit1.fruitMode == Displaying) draw_fruit_game(game->currentLevel);\n\t\t\tif (game->gameFruit2.fruitMode == Displaying) draw_fruit_game(game->currentLevel);\n\n\t\t\tif (game->gameFruit1.eaten && ticks_game() - game->gameFruit1.eatenAt < 2000) draw_fruit_pts(&game->gameFruit1);\n\t\t\tif (game->gameFruit2.eaten && ticks_game() - game->gameFruit2.eatenAt < 2000) draw_fruit_pts(&game->gameFruit2);\n\n\t\t\tdraw_pacman(&game->pacman);\n\n\t\t\tfor (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);\n\t\t\tbreak;\n\t\tcase WinState:\n\t\t\tdraw_pacman_static(&game->pacman);\n\n\t\t\tif (dt < 2000)\n\t\t\t{\n\t\t\t\tfor (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);\n\t\t\t\tdraw_board(&game->board);\n\t\t\t}\n\t\t\telse\n\t\t\t{\n\t\t\t\t//stop rendering the pen, and do the flash animation\n\t\t\t\tdraw_board_flash(&game->board);\n\t\t\t}\n\n\t\t\tbreak;\n\t\tcase DeathState:\n\t\t\t//draw everything the same for 1ish second\n\t\t\tif (dt < 1000)\n\t\t\t{\n\t\t\t\t//draw everything normally\n\n\t\t\t\t//TODO: this actually draws the last frame pacman was on when he died\n\t\t\t\tdraw_pacman_static(&game->pacman);\n\n\t\t\t\tfor (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);\n\t\t\t}\n\t\t\telse\n\t\t\t{\n\t\t\t\t//draw the death animation\n\t\t\t\tdraw_pacman_death(&game->pacman, dt - 1000);\n\t\t\t}\n\n\t\t\tdraw_large_pellets(&game->pelletHolder, true);\n\t\t\tdraw_board(&game->board);\n\t\t\tbreak;\n\t\tcase GameoverState:\n\t\t\tdraw_game_gameover();\n\t\t\tdraw_board(&game->board);\n\t\t\tdraw_credits(num_credits());\n\t\t\tbreak;\n\t}\n}\n\nstatic void enter_state(PacmanGame *game, GameState state)\n{\n\t//process leaving a state\n\tswitch (game->gameState)\n\t{\n\t\tcase GameBeginState:\n\t\t\tgame->pacman.livesLeft--;\n\n\t\t\tbreak;\n\t\tcase WinState:\n\t\t\tgame->currentLevel++;\n\t\t\tgame->gameState = LevelBeginState;\n\t\t\tlevel_init(game);\n\t\t\tbreak;\n\t\tcase DeathState:\n\t\t\t// Player died and is starting a new game, subtract a life\n\t\t\tif (state == LevelBeginState)\n\t\t\t{\n\t\t\t\tgame->pacman.livesLeft--;\n\t\t\t\tpacdeath_init(game);\n\t\t\t}\n\t\tdefault: ; //do nothing\n\t}\n\n\t//process entering a state\n\tswitch (state)\n\t{\n\t\tcase GameBeginState:\n\t\t\tplay_sound(LevelStartSound);\n\n\t\t\tbreak;\n\t\tcase LevelBeginState:\n\t\t\t\n\t\t\tbreak;\n\t\tcase GamePlayState:\n\t\t\tbreak;\n\t\tcase WinState:\n\n\t\t\tbreak;\n\t\tcase DeathState:\n\t\t\tbreak;\n\t\tcase GameoverState:\n\t\t\tbreak;\n\t}\n\n\tgame->ticksSinceModeChange = ticks_game();\n\tgame->gameState = state;\n}\n\n//checks if it's valid that pacman could move in this direction at this point in time\nbool can_move(Pacman *pacman, Board *board, Direction dir)\n{\n\t//easy edge cases, tile has to be parallal with a direction to move it\n\tif ((dir == Up   || dir == Down ) && !on_vert(&pacman->body)) return false;\n\tif ((dir == Left || dir == Right) && !on_horo(&pacman->body)) return false;\n\n\t//if pacman wants to move on an axis and he is already partially on that axis (not 0)\n\t//it is always a valid move\n\tif ((dir == Left || dir == Right) && !on_vert(&pacman->body)) return true;\n\tif ((dir == Up   || dir == Down ) && !on_horo(&pacman->body)) return true;\n\n\t//pacman is at 0/0 and moving in the requested direction depends on if there is a valid tile there\n\tint x = 0;\n\tint y = 0;\n\n\tdir_xy(dir, &x, &y);\n\n\tint newX = pacman->body.x + x;\n\tint newY = pacman->body.y + y;\n\n\treturn is_valid_square(board, newX, newY) || is_tele_square(newX, newY);\n}\n\nstatic void process_player(PacmanGame *game)\n{\n\tPacman *pacman = &game->pacman;\n\tBoard *board = &game->board;\n\n\tif (pacman->missedFrames != 0)\n\t{\n\t\tpacman->missedFrames--;\n\t\treturn;\n\t}\n\n\tDirection oldLastAttemptedDir = pacman->lastAttemptedMoveDirection;\n\n\tDirection newDir;\n\n\tbool dirPressed = dir_pressed_now(&newDir);\n\n\tif (dirPressed)\n\t{\n\t\t//user wants to move in a direction\n\t\tpacman->lastAttemptedMoveDirection = newDir;\n\n\t\t//if player holds opposite direction to current walking dir\n\t\t//we can always just switch current walking direction\n\t\t//since we're on parallel line\n\t\tif (newDir == dir_opposite(pacman->body.curDir))\n\t\t{\n\t\t\tpacman->body.curDir = newDir;\n\t\t\tpacman->body.nextDir = newDir;\n\t\t}\n\n\t\t//if pacman was stuck before just set his current direction as pressed\n\t\tif (pacman->movementType == Stuck)\n\t\t{\n\t\t\tpacman->body.curDir = newDir;\n\t\t}\n\n\t\tpacman->body.nextDir = newDir;\n\t}\n\telse if (pacman->movementType == Stuck)\n\t{\n\t\t//pacman is stuck and player didn't move - player should still be stuck.\n\t\t//don't do anything\n\t\treturn;\n\t}\n\telse\n\t{\n\t\t//user doesn't want to change direction and pacman isn't stuck\n\t\t//pacman can move like normal\n\n\t\t//just set the next dir to current dir\n\t\tpacman->body.nextDir = pacman->body.curDir;\n\t}\n\n\tpacman->movementType = Unstuck;\n\n\tint curDirX = 0;\n\tint curDirY = 0;\n\tint nextDirX = 0;\n\tint nextDirY = 0;\n\n\tdir_xy(pacman->body.curDir, &curDirX, &curDirY);\n\tdir_xy(pacman->body.nextDir, &nextDirX, &nextDirY);\n\n\tint newCurX = pacman->body.x + curDirX;\n\tint newCurY = pacman->body.y + curDirY;\n\tint newNextX = pacman->body.x + nextDirX;\n\tint newNextY = pacman->body.y + nextDirY;\n\n\tbool canMoveCur =  is_valid_square(board, newCurX, newCurY) || is_tele_square(newCurX, newCurY);\n\tbool canMoveNext = is_valid_square(board, newNextX, newNextY) || is_tele_square(newNextX, newNextY);\n\n\t//if pacman is currently on a center tile and can't move in either direction\n\t//don't move him\n\tif (on_center(&pacman->body) && !canMoveCur && !canMoveNext)\n\t{\n\t\tpacman->movementType = Stuck;\n\t\tpacman->lastAttemptedMoveDirection = oldLastAttemptedDir;\n\n\t\treturn;\n\t}\n\n\tmove_pacman(&pacman->body, canMoveCur, canMoveNext);\n\n\t//if pacman is on the center, and he couldn't move either of  his last directions\n\t//he must be stuck now\n\tif (on_center(&pacman->body) && !canMoveCur && !canMoveNext)\n\t{\n\t\tpacman->movementType = Stuck;\n\t\treturn;\n\t}\n\n\tresolve_telesquare(&pacman->body);\n}\n\nstatic void process_ghosts(PacmanGame *game)\n{\n\tfor (int i = 0; i < 4; i++)\n\t{\n\t\tGhost *g = &game->ghosts[i];\n\n\t\tif (g->movementMode == InPen)\n\t\t{\n\t\t\t//ghosts bob up and down - move in direction. If they hit a square, change direction\n\t\t\tbool moved = move_ghost(&g->body);\n\n\t\t\tif (moved && (g->body.y == 13 || g->body.y == 15))\n\t\t\t{\n\t\t\t\tg->body.nextDir = g->body.curDir;\n\t\t\t\tg->body.curDir = dir_opposite(g->body.curDir);\n\t\t\t}\n\n\t\t\tcontinue;\n\t\t}\n\n\t\tif (g->movementMode == LeavingPen)\n\t\t{\n\t\t\t//ghost is in center of tile\n\t\t\t//move em to the center of the pen (in x axis)\n\t\t\t//then more em up out the gate\n\t\t\t//when they are out of the gate, set them to be in normal chase mode then set them off on their way\n\n\t\t\tcontinue;\n\t\t}\n\n\t\t//all other modes can move normally (I think)\n\t\tMovementResult result = move_ghost(&g->body);\n\t\tresolve_telesquare(&g->body);\n\n\t\tif (result == NewSquare)\n\t\t{\n\t\t\t//if they are in a new tile, rerun their target update logic\n\t\t\texecute_ghost_logic(g, g->ghostType, &game->ghosts[0], &game->pacman);\n\n\t\t\tg->nextDirection = next_direction(g, &game->board);\n\t\t}\n\t\telse if (result == OverCenter)\n\t\t{\n\t\t\t//they've hit the center of a tile, so change their current direction to the new direction\n\t\t\tg->body.curDir = g->transDirection;\n\t\t\tg->body.nextDir = g->nextDirection;\n\t\t\tg->transDirection = g->nextDirection;\n\t\t}\n\t}\n}\n\nstatic void process_fruit(PacmanGame *game)\n{\n\tint pelletsEaten = game->pelletHolder.totalNum - game->pelletHolder.numLeft;\n\n\tGameFruit *f1 = &game->gameFruit1;\n\tGameFruit *f2 = &game->gameFruit2;\n\n\tint curLvl = game->currentLevel;\n\n\tif (pelletsEaten >= 70 && f1->fruitMode == NotDisplaying)\n\t{\n\t\tf1->fruitMode = Displaying;\n\t\tregen_fruit(f1, curLvl);\n\t}\n\telse if (pelletsEaten == 170 && f2->fruitMode == NotDisplaying)\n\t{\n\t\tf2->fruitMode = Displaying;\n\t\tregen_fruit(f2, curLvl);\n\t}\n\n\tunsigned int f1dt = ticks_game() - f1->startedAt;\n\tunsigned int f2dt = ticks_game() - f2->startedAt;\n\n\tPacman *pac = &game->pacman;\n\n\tif (f1->fruitMode == Displaying)\n\t{\n\t\tif (f1dt > f1->displayTime) f1->fruitMode = Displayed;\n\t}\n\n\tif (f2->fruitMode == Displaying)\n\t{\n\t\tif (f2dt > f2->displayTime) f2->fruitMode = Displayed;\n\t}\n\n\t//check for collisions\n\n\tif (f1->fruitMode == Displaying && collides_obj(&pac->body, f1->x, f1->y))\n\t{\n\t\tf1->fruitMode = Displayed;\n\t\tf1->eaten = true;\n\t\tf1->eatenAt = ticks_game();\n\t\tpac->score += fruit_points(f1->fruit);\n\t}\n\n\tif (f2->fruitMode == Displaying && collides_obj(&pac->body, f2->x, f2->y))\n\t{\n\t\tf2->fruitMode = Displayed;\n\t\tf2->eaten = true;\n\t\tf2->eatenAt = ticks_game();\n\t\tpac->score += fruit_points(f2->fruit);\n\t}\n}\n\nstatic void process_pellets(PacmanGame *game)\n{\n\t//if pacman and pellet collide\n\t//give pacman that many points\n\t//set pellet to not be active\n\t//decrease num of alive pellets\n\tPelletHolder *holder = &game->pelletHolder;\n\n\tfor (int i = 0; i < holder->totalNum; i++)\n\t{\n\t\tPellet *p = &holder->pellets[i];\n\n\t\t//skip if we've eaten this one already\n\t\tif (p->eaten) continue;\n\n\t\tif (collides_obj(&game->pacman.body, p->x, p->y))\n\t\t{\n\t\t\tholder->numLeft--;\n\n\t\t\tp->eaten = true;\n\t\t\tgame->pacman.score += pellet_points(p);\n\n\t\t\t//play eat sound\n\n\t\t\t//eating a small pellet makes pacman not move for 1 frame\n\t\t\t//eating a large pellet makes pacman not move for 3 frames\n\t\t\tgame->pacman.missedFrames = pellet_nop_frames(p);\n\n\t\t\t//can only ever eat 1 pellet in a frame, so return\n\t\t\treturn;\n\t\t}\n\t}\n\n\t//maybe next time, poor pacman\n}\n\nstatic bool check_pacghost_collision(PacmanGame *game)\n{\n\tfor (int i = 0; i < 4; i++)\n\t{\n\t\tGhost *g = &game->ghosts[i];\n\n\t\tif (collides(&game->pacman.body, &g->body)) return true;\n\t}\n\n\treturn false;\n}\n\nvoid gamestart_init(PacmanGame *game)\n{\n\tlevel_init(game);\n\n\tpacman_init(&game->pacman);\n\t//we need to reset all fruit\n\t//fuit_init();\n\tgame->highscore = 0; //TODO maybe load this in from a file..?\n\tgame->currentLevel = 1;\n\n\t//invalidate the state so it doesn't effect the enter_state function\n\tgame->gameState = -1;\n\tenter_state(game, GameBeginState);\n}\n\nvoid level_init(PacmanGame *game)\n{\n\t//reset pacmans position\n\tpacman_level_init(&game->pacman);\n\n\t//reset pellets\n\tpellets_init(&game->pelletHolder);\n\n\t//reset ghosts\n\tghosts_init(game->ghosts);\n\n\t//reset fruit\n\treset_fruit(&game->gameFruit1);\n\treset_fruit(&game->gameFruit2);\n}\n\nvoid pacdeath_init(PacmanGame *game)\n{\n\tpacman_level_init(&game->pacman);\n\tghosts_init(game->ghosts);\n\n\treset_fruit(&game->gameFruit1);\n\treset_fruit(&game->gameFruit2);\n}\n\n//TODO: make this method based on a state, not a conditional\n//or make the menu system the same. Just make it consistant\nbool is_game_over(PacmanGame *game)\n{\n\tunsigned dt = ticks_game() - game->ticksSinceModeChange;\n\n\treturn dt > 2000 && game->gameState == GameoverState;\n}\n\nint int_length(int x)\n{\n    if (x >= 1000000000) return 10;\n    if (x >= 100000000)  return 9;\n    if (x >= 10000000)   return 8;\n    if (x >= 1000000)    return 7;\n    if (x >= 100000)     return 6;\n    if (x >= 10000)      return 5;\n    if (x >= 1000)       return 4;\n    if (x >= 100)        return 3;\n    if (x >= 10)         return 2;\n    return 1;\n}\n\n// 바꿔야 할 부분 1\nstatic bool resolve_telesquare(PhysicsBody *body)\n{\n\t//TODO: chuck this back in the board class somehow\n\n\tif (body->y != 14) return false;\n\tif (body->y != 3) return false;\n\tif (body->y != 28) return false;\n\n\tif (body->x == -1) { body->x = 27; return true; }\n\tif (body->x == 28) { body->x =  0; return true; }\n\n\treturn false;\n}\n","undoManager":{"mark":-2,"position":33,"stack":[[{"start":{"row":591,"column":0},"end":{"row":592,"column":0},"action":"insert","lines":["",""],"id":2}],[{"start":{"row":592,"column":0},"end":{"row":592,"column":1},"action":"insert","lines":["/"],"id":3}],[{"start":{"row":592,"column":1},"end":{"row":592,"column":2},"action":"insert","lines":["/"],"id":4}],[{"start":{"row":592,"column":2},"end":{"row":592,"column":3},"action":"insert","lines":[" "],"id":5}],[{"start":{"row":592,"column":3},"end":{"row":592,"column":4},"action":"insert","lines":["바"],"id":9}],[{"start":{"row":592,"column":4},"end":{"row":592,"column":5},"action":"insert","lines":["꿔"],"id":13}],[{"start":{"row":592,"column":5},"end":{"row":592,"column":6},"action":"insert","lines":["야"],"id":14}],[{"start":{"row":592,"column":6},"end":{"row":592,"column":7},"action":"insert","lines":[" "],"id":15}],[{"start":{"row":592,"column":7},"end":{"row":592,"column":8},"action":"insert","lines":["할"],"id":18}],[{"start":{"row":592,"column":8},"end":{"row":592,"column":9},"action":"insert","lines":[" "],"id":19}],[{"start":{"row":592,"column":9},"end":{"row":592,"column":10},"action":"insert","lines":["부"],"id":23}],[{"start":{"row":592,"column":10},"end":{"row":592,"column":11},"action":"insert","lines":["분"],"id":25}],[{"start":{"row":592,"column":11},"end":{"row":592,"column":12},"action":"insert","lines":[" "],"id":26}],[{"start":{"row":592,"column":12},"end":{"row":592,"column":13},"action":"insert","lines":["1"],"id":27}],[{"start":{"row":597,"column":33},"end":{"row":598,"column":0},"action":"insert","lines":["",""],"id":28},{"start":{"row":598,"column":0},"end":{"row":598,"column":1},"action":"insert","lines":["\t"]}],[{"start":{"row":598,"column":1},"end":{"row":598,"column":34},"action":"insert","lines":["\tif (body->y != 14) return false;"],"id":29}],[{"start":{"row":598,"column":34},"end":{"row":599,"column":0},"action":"insert","lines":["",""],"id":30},{"start":{"row":599,"column":0},"end":{"row":599,"column":2},"action":"insert","lines":["\t\t"]}],[{"start":{"row":599,"column":2},"end":{"row":599,"column":35},"action":"insert","lines":["\tif (body->y != 14) return false;"],"id":31}],[{"start":{"row":598,"column":1},"end":{"row":598,"column":2},"action":"remove","lines":["\t"],"id":32}],[{"start":{"row":599,"column":2},"end":{"row":599,"column":3},"action":"remove","lines":["\t"],"id":33}],[{"start":{"row":599,"column":1},"end":{"row":599,"column":2},"action":"remove","lines":["\t"],"id":34}],[{"start":{"row":597,"column":33},"end":{"row":598,"column":0},"action":"insert","lines":["",""],"id":35},{"start":{"row":598,"column":0},"end":{"row":598,"column":1},"action":"insert","lines":["\t"]}],[{"start":{"row":599,"column":33},"end":{"row":600,"column":0},"action":"insert","lines":["",""],"id":36},{"start":{"row":600,"column":0},"end":{"row":600,"column":1},"action":"insert","lines":["\t"]}],[{"start":{"row":600,"column":0},"end":{"row":600,"column":1},"action":"remove","lines":["\t"],"id":37}],[{"start":{"row":599,"column":33},"end":{"row":600,"column":0},"action":"remove","lines":["",""],"id":38}],[{"start":{"row":598,"column":0},"end":{"row":598,"column":1},"action":"remove","lines":["\t"],"id":39}],[{"start":{"row":597,"column":33},"end":{"row":598,"column":0},"action":"remove","lines":["",""],"id":40}],[{"start":{"row":598,"column":17},"end":{"row":598,"column":18},"action":"remove","lines":["4"],"id":41}],[{"start":{"row":598,"column":16},"end":{"row":598,"column":17},"action":"remove","lines":["1"],"id":42}],[{"start":{"row":598,"column":16},"end":{"row":598,"column":17},"action":"insert","lines":["3"],"id":43}],[{"start":{"row":599,"column":17},"end":{"row":599,"column":18},"action":"remove","lines":["4"],"id":44}],[{"start":{"row":599,"column":16},"end":{"row":599,"column":17},"action":"remove","lines":["1"],"id":45}],[{"start":{"row":599,"column":16},"end":{"row":599,"column":17},"action":"insert","lines":["2"],"id":46}],[{"start":{"row":599,"column":17},"end":{"row":599,"column":18},"action":"insert","lines":["8"],"id":47}]]},"ace":{"folds":[],"scrolltop":7983,"scrollleft":0,"selection":{"start":{"row":594,"column":1},"end":{"row":594,"column":1},"isBackwards":false},"options":{"guessTabSize":true,"useWrapMode":false,"wrapToView":true},"firstLineState":0},"timestamp":1517550643188}