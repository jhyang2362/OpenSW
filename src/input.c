#include "input.h"

//does a check to ensure the given keycode is within the bounds of valid keys
//stops the program with an error if it is out of bounds
static void check_keycode(int keycode);

//gets the frame this direction was pressed.
//Uses both wasd and arrow keys.
static int frame_for_direction(Direction dir);

static bool keysHeld[MAX_KEYS] = {false};

static unsigned int keysPressedFrame[MAX_KEYS] = {0};
static unsigned int keysReleasedFrame[MAX_KEYS] = {0};
static unsigned int keysPressedFrame2[MAX_KEYS] = {0};
static unsigned int keysReleasedFrame2[MAX_KEYS] = {0};

static unsigned int curKeyFrame = 1;

void keyevents_finished(void)
{
	curKeyFrame++;
}

void handle_keydown(int keycode)
{
	check_keycode(keycode);

	if (!keysHeld[keycode]) keysPressedFrame[keycode] = curKeyFrame;

	keysHeld[keycode] = true;
}

void handle_keyup(int keycode)
{
	check_keycode(keycode);

	if (keysHeld[keycode]) keysReleasedFrame[keycode] = curKeyFrame;

	keysHeld[keycode] = false;
}

bool dir_key_held(Direction direction)
{
	switch (direction)
	{
		case Up:    return keysHeld[SDLK_UP];
		case Down:  return keysHeld[SDLK_DOWN];
		case Left:  return keysHeld[SDLK_LEFT];
		case Right: return keysHeld[SDLK_RIGHT];
	}

	printf("should never reach here\n");
	exit(1);
}

bool dir_key_held2(Direction direction)
{
	switch (direction)
	{
		case Up:    return keysHeld[SDLK_w];
		case Down:  return keysHeld[SDLK_s];
		case Left:  return keysHeld[SDLK_a];
		case Right: return keysHeld[SDLK_d];
	}

	printf("should never reach here\n");
	exit(1);
}

bool dir_pressed_now(Direction *dir)
{
	int highestPushed = 0;

	Direction dirs[4] = {Up, Left, Down, Right};

	for (int i = 3; i >= 0; i--)
	{
		if (!dir_key_held(dirs[i])) continue;

		int x = frame_for_direction(dirs[i]);

		if (x > highestPushed)
		{
			*dir = dirs[i];
			highestPushed = x;
		}
	}

	return highestPushed != 0;
}

bool dir_pressed_now2(Direction *dir)
{
	int highestPushed = 0;

	Direction dirs[4] = {Up, Left, Down, Right};

	for (int i = 3; i >= 0; i--)
	{
		if (!dir_key_held2(dirs[i])) continue;

		int x = frame_for_direction2(dirs[i]);

		if (x > highestPushed)
		{
			*dir = dirs[i];
			highestPushed = x;
		}
	}

	return highestPushed != 0;
}

bool key_held(int keycode)
{
	check_keycode(keycode);

	return keysHeld[keycode];
}

bool key_pressed(int keycode)
{
	check_keycode(keycode);

	return keysPressedFrame[keycode] == (curKeyFrame - 1);
}

bool key_released(int keycode)
{
	check_keycode(keycode);

	return keysReleasedFrame[keycode] == (curKeyFrame - 1);
}

#define max(a, b) (a) > (b) ? (a) : (b)
#define min(a, b) max((b), (a))

static int frame_for_direction(Direction dir)
{
	switch (dir)
	{
		case Up:    return keysPressedFrame[SDLK_UP];
		case Down:  return keysPressedFrame[SDLK_DOWN];
		case Left:  return keysPressedFrame[SDLK_LEFT];
		case Right: return keysPressedFrame[SDLK_RIGHT];
	}

	printf("should never reach here\n");
	exit(1);
}
static int frame_for_direction2(Direction dir)
{
	switch (dir)
	{
		case Up:    return keysPressedFrame2[SDLK_w]);
		case Down:  return keysPressedFrame2[SDLK_s]);
		case Left:  return keysPressedFrame2[SDLK_a]);
		case Right: return keysPressedFrame2[SDLK_d]);
	}

	printf("should never reach here\n");
	exit(1);
}

static void check_keycode(int keycode)
{
	if (keycode >= MAX_KEYS)
	{
		printf("Keycode %d is out of range.\n", keycode);
		printf("Aborting\n");
		exit(1);
	}
}
