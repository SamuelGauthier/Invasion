all: ex

ex: invBase.cpp invBase.h main.cpp invTex.cpp invTex.h invTalk.cpp invTalk.h invMath.cpp invMath.h invGeom.h invGeom.cpp
	g++ main.cpp invBase.cpp invGeom.cpp invTalk.cpp invTex.cpp invMath.cpp -o ex -lSDL -lSDLmain -lSDL_ttf -lSDL_image -lGL -lGLU

clean:
	rm *.o ex
