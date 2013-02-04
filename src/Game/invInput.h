#ifndef INVINPUT_H
#define INVINPUT_H

#define sinput getInputData()

// TODO: Do the same thing for the output ( singleton ) ( width height...)

struct invInput
{
	int keychar;
	bool keys[512];
	int keystate;

	int mousestate;
	int mousebutton;

	int mousex;
	int mousey;

	invInput()
	{
		keychar = 0;
		for(int i=0;i<512;i++)
			keys[i] = false;
		keystate = 0;
		mousestate = 0;
		mousebutton = 0;
		mousex = mousey = 0;
	}
};


void updateInputData();
invInput* getInputData();

#endif
