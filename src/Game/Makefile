all: ex

ex: ../invLib/invBase.cpp ../invLib/invBase.h main.cpp ../invLib/invTex.cpp ../invLib/invTex.h ../invLib/invTalk.cpp ../invLib/invTalk.h ../invLib/invMath.cpp ../invLib/invMath.h ../invLib/invGeom.h ../invLib/invGeom.cpp
	g++ main.cpp ../invLib/invBase.cpp ../invLib/invGeom.cpp ../invLib/invTalk.cpp ../invLib/invTex.cpp ../invLib/invMath.cpp -o ex -lSDL -lSDLmain -lSDL_ttf -lSDL_image -lGL -lGLU

clean:
	rm *.o ex
