#include "invInput.h"
#include <SDL/SDL.h>

invInput input_data; // <-- this variable is global because there is only one input source ( SDL )

void updateInputData()
{
	static SDL_Event event;

	input_data.keychar = 0;
	input_data.mousestate = false;
	input_data.keystate = 0;
	
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				if(event.key.keysym.sym < 512)
					input_data.keys[(int)event.key.keysym.sym] = true;
				if(!event.key.keysym.unicode)
					input_data.keychar = (int)event.key.keysym.sym;
				else
					input_data.keychar = (int)event.key.keysym.unicode;
				input_data.keystate = 1;
				break;
			case SDL_KEYUP:
				if(event.key.keysym.sym < 512)
					input_data.keys[(int)event.key.keysym.sym] = false;
				input_data.keystate = 2;
				break;
			case SDL_MOUSEBUTTONDOWN:
				input_data.mousestate = 1;
				input_data.mousebutton = event.button.button;
				break;
			case SDL_MOUSEBUTTONUP:
				input_data.mousestate = 2;
				input_data.mousebutton = event.button.button;
				break;
		}
	}

	SDL_GetMouseState(&input_data.mousex, &input_data.mousey);
}

invInput* getInputData() { return &input_data; }
