#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

// Define update here, so it can be used now in eventHandler and redefined
// later.
static int update(void *userdata);

// Define fonts
const char *fontpath = "/System/Fonts/Asheville-Sans-24-Light.pft";
LCDFont *font = NULL;

// Magic Windows stuff for DLLs. Not needed on Linux.
//#ifdef _WINDLL
//__declspec(dllexport)
//#endif

// Handle events.
int eventHandler(PlaydateAPI *pd, PDSystemEvent event, uint32_t arg) {
  (void)arg; // arg is currently only used for event = kEventKeyPressed

  if (event == kEventInit) {
    const char *err;
    font = pd->graphics->loadFont(fontpath, &err);

    if (font == NULL)
      pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__,
                        fontpath, err);

    // Note: If you set an update callback in the kEventInit handler, the system
    // assumes the game is pure C and doesn't run any Lua code in the game
    pd->system->setUpdateCallback(update, pd);
  }

  return 0;
}

// Define variables
#define TEXT_WIDTH 135
#define TEXT_HEIGHT 32

int x = (400 - TEXT_WIDTH) / 2;
int y = (240 - TEXT_HEIGHT) / 2;
int dx = 2;
int dy = 3;

int inverted = 0;

static int update(void *userdata) {
  // Define pd as an instance of the PlaydateAPI.
  PlaydateAPI *pd = userdata;

  // Clear the screen with white.
  pd->graphics->clear(kColorWhite);

  // Draw the text "Hello World" at the current x and y positions.
  pd->graphics->setFont(font);
  char *text = "Hello World";
  pd->graphics->drawText(text, strlen(text), kASCIIEncoding, x, y);

  // Create a noise synth.
  PDSynth *synth = pd->sound->synth->newSynth();
  pd->sound->synth->setWaveform(synth, kWaveformNoise);

  // Move the Hello World text using the current velocities.
  x += dx;
  y += dy;

  // If the text hits the left or right sides of the screen, play a c5 note on
  // the noise channel, invert the screen and invert the velocity of the text.
  if (x < 0 || x > LCD_COLUMNS - TEXT_WIDTH) {
    dx = -dx;
    pd->sound->synth->playNote(synth, 523.25, 100, 0.05, 0);
    if (inverted == 1) {
      inverted = 0;
      pd->display->setInverted(inverted);
    } else {
      inverted = 1;
      pd->display->setInverted(inverted);
    }
  }

  // If the text hits the top or bottom sides of the screen, play a c5 note on
  // the noise channel, invert the screen and invert the velocity of the text.
  if (y < 0 || y > LCD_ROWS - TEXT_HEIGHT) {
    dy = -dy;
    pd->sound->synth->playNote(synth, 523.25, 100, 0.05, 0);
    if (inverted == 1) {
      inverted = 0;
      pd->display->setInverted(inverted);
    } else {
      inverted = 1;
      pd->display->setInverted(inverted);
    }
  }

  // Draw the current FPS in the top left corner of the screen.
  pd->system->drawFPS(0, 0);

  // If there is a note playing on the synth, wait for it to finish.
  do {
  } while (pd->sound->synth->isPlaying(synth));

  // Free the synth from memory.
  pd->sound->synth->freeSynth(synth);

  // Return with no errors.
  return 1;
}
