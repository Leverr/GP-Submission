#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


#include <windows.h>
#include "windowOGL.h"
#include "GameConstants.h"
#include "cWNDManager.h"
#include "cInputMgr.h"
#include "cSoundMgr.h"
#include "cFontMgr.h"
#include "cSprite.h"
#include "asteroidsGame.h"

bool checkForCollision();

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR cmdLine,
	int cmdShow)
{

	int astInc = 0;
	int asteroidClock = 0;

	//Set our window settings
	const int windowWidth = 1024;
	const int windowHeight = 768;
	const int windowBPP = 16;




	//This is our window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	// This is the input manager
	static cInputMgr* theInputMgr = cInputMgr::getInstance();

	// This is the sound manager
	static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();

	// This is the Font manager
	static cFontMgr* theFontMgr = cFontMgr::getInstance();

	//The example OpenGL code
	windowOGL theOGLWnd;

	//Attach our the OpenGL window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	// Attach the keyboard manager
	pgmWNDMgr->attachInputMgr(theInputMgr);

	//Attempt to create the window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
	{
		//If it fails

		MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
		return 1;
	}

	if (!theOGLWnd.initOGL(windowWidth, windowHeight)) //Initialize our example
	{
		MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
		return 1;
	}

	//Clear key buffers
	theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);

	/* initialize random seed: */
	srand((unsigned int)time(NULL));

	// Create vector array of textures
	LPCSTR texturesToUse[] = { "Images\\enemy1.png", "Images\\enemy2.png", "Images\\enemy3.png", "Images\\enemy4.png", "Images\\bullet.png", "Images\\menuScreen.png" };
	for (int tCount = 0; tCount < 5; tCount++)
	{
		theGameTextures.push_back(new cTexture());
		theGameTextures[tCount]->createTexture(texturesToUse[tCount]);
	}

	// load game sounds
	// Load Sound
	LPCSTR gameSounds[3] = { "Audio/Space.wav", "Audio/shot007.wav", "Audio/explosion2.wav" };

	theSoundMgr->add("Theme", gameSounds[0]);
	theSoundMgr->add("Shot", gameSounds[1]);
	theSoundMgr->add("Explosion", gameSounds[2]);

	// load game fontss
	// Load Fonts
	LPCSTR gameFonts[2] = { "Fonts/digital-7.ttf", "Fonts/space age.ttf" };

	theFontMgr->addFont("SevenSeg", gameFonts[0], 24);
	theFontMgr->addFont("Space", gameFonts[1], 24);


	FTGLPixmapFont font("Fonts/digital-7.ttf");
	font.FaceSize(50);


	cTexture spriteStartBkgd;
	spriteStartBkgd.createTexture("Images\\background.png");
	cBkGround spriteStart;
	spriteStart.attachInputMgr(theInputMgr);
	spriteStart.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteStart.setTexture(spriteStartBkgd.getTexture());
	spriteStart.setTextureDimensions(spriteStartBkgd.getTWidth(), spriteStartBkgd.getTHeight());


	cTexture textureBkgd;
	textureBkgd.createTexture("Images\\background.png");
	cBkGround spriteBkgd;
	spriteBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteBkgd.setTexture(textureBkgd.getTexture());
	spriteBkgd.setTextureDimensions(textureBkgd.getTWidth(), textureBkgd.getTHeight());

	cTexture rocketTxt;
	rocketTxt.createTexture("Images\\playership.png");
	cRocket rocketSprite;
	rocketSprite.attachInputMgr(theInputMgr); // Attach the input manager to the sprite
	rocketSprite.setSpritePos(glm::vec2(35, windowHeight/2));
	rocketSprite.setTexture(rocketTxt.getTexture());
	rocketSprite.setTextureDimensions(rocketTxt.getTWidth(), rocketTxt.getTHeight());
	rocketSprite.setSpriteCentre();
	rocketSprite.setRocketVelocity(glm::vec2(0.0f, 0.0f));

	// Attach sound manager to rocket sprite
	rocketSprite.attachSoundMgr(theSoundMgr);

	gameState theGameState = PLAYING;

	//This is the mainloop, we render frames until isRunning returns false ************
	while (pgmWNDMgr->isWNDRunning())
	{
		pgmWNDMgr->processWNDEvents(); //Process any window events

		//We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		switch (theGameState)
		{
		case MENU:
			rocketSprite.render();
			spriteStart.render();

			break;

		case PLAYING:
		{spriteBkgd.render();

		
		rocketSprite.update(elapsedTime);


		vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();

		while (asteroidIterator != theAsteroids.end())
		{
			if ((*asteroidIterator)->isActive() == false || (*asteroidIterator)->getSpritePos().y > 1500)
			{
				asteroidIterator = theAsteroids.erase(asteroidIterator);
				astInc--;
			}
			else if (rocketSprite.collidedWith(rocketSprite.getBoundingRect(), (*asteroidIterator)->getBoundingRect())){

				exit(0);

			}
			else
			{

				(*asteroidIterator)->update(elapsedTime);
				(*asteroidIterator)->render();
				++asteroidIterator;
		}
		 }

		asteroidClock++;

		if (asteroidClock > 300){

			theAsteroids.push_back(new cAsteroid);
			theAsteroids[astInc]->setSpritePos(glm::vec2(windowWidth, rand() % windowHeight));
			theAsteroids[astInc]->setSpriteTranslation(glm::vec2((rand() % 4 + 1), (rand() % 4 + 1)));
			int randAsteroid = rand() % 4;  
			theAsteroids[astInc]->setTexture(theGameTextures[randAsteroid]->getTexture());
			theAsteroids[astInc]->setTextureDimensions(theGameTextures[randAsteroid]->getTWidth(), theGameTextures[randAsteroid]->getTHeight());
			theAsteroids[astInc]->setSpriteCentre();
			theAsteroids[astInc]->setAsteroidVelocity(glm::vec2(glm::vec2(0.0f, 0.0f)));
			theAsteroids[astInc]->setActive(true);
			theAsteroids[astInc]->setMdlRadius();
			astInc++;
			asteroidClock = 0;
		}



		rocketSprite.render();

		pgmWNDMgr->swapBuffers();
		theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);
		}
			break;
		case END:
			
			break;
		}


	}

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

	return 0; //Return success
}



