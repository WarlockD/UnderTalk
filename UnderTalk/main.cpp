////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Global.h"
#include "Effect.hpp"
#include <vector>
#include <cmath>
#include "obj_writer.h"
#include "obj_vaporized_new.h"
#include "room.h"


static sf::Font debugFont;

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////

void windowLoop() {
	sf::View battleView;
	battleView.reset(sf::FloatRect(0, 0, 640, 480));

	sf::View overLand;
	overLand.reset(sf::FloatRect(0, 0, 320, 240));
	sf::Text fpsText;
	fpsText.setFont(debugFont);
	fpsText.setCharacterSize(30);
	fpsText.setPosition(0.0f, 0.0f);
	auto res = GetUndertale();
	size_t room_num = 20;
	auto uroom = res.LookupRoom(room_num);
	
	TileMap testMap( uroom);
	size_t num = 0;

	//GSprite test_head(230);
	//test_head.setPosition(10, 10);
	//obj_vaporized_new vtest(test_head,true);
//	obj_vaporized_new vtest(num, false);
	//vtest.setPosition(100, 100);
	//vtest.setScale(2.0f, 2.0f);
	OBJ_WRITER writer;
	writer.AddText("*\\TT\\F1This is a \\Ytest\\W&* and another \\z%%");
	writer.setPosition(100, 100);


	// Start the game loop
	sf::Clock clock;
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "UnderTalk", sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	while (window.isOpen())
	{
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
					// Escape key: exit
				case sf::Keyboard::Escape:
					window.close();

					break;
				case sf::Keyboard::D:
					room_num++;
					testMap.loadRoom(room_num);
					break;
				case sf::Keyboard::A:
					room_num--;
					testMap.loadRoom(room_num);
					break;

				case sf::Keyboard::Q:
				//	vtest.debugStart();
					break;

				case sf::Keyboard::Right:
				//	vtest.move(5.0f, 0.0f);
					break;
				case sf::Keyboard::Num1:
					num++;
				//	vtest.setPixels(num, false);
					break;
				case sf::Keyboard::Num2:
					num++;
				//	vtest.setPixels(test_head, false);
					break;
				}
			}
		}
		if (clock.getElapsedTime().asSeconds() > (1.0f / 30.0f)) {
			float elapsed = clock.restart().asSeconds();
				writer.frame();
			//vtest.step(elapsed);
			float ffps = 1.0f / elapsed;
			fpsText.setString(std::to_string(std::floorf(ffps)));
		}
		// Clear the window
		window.clear(sf::Color(0, 0, 0));
		//window.setView(battleView);
		window.draw(testMap);
		window.draw(fpsText);
		window.draw(writer);
	//	window.draw(test_head);
	//	window.draw(vtest); // fps is always over eveything
		window.display();
	}
	printf("All done\r\n");

}
int main(int argc, const char* argv[])
{
	// undertail combat views are in 640x480 but overowrd is in 320 240 fyi

	sf::View battleView;
	battleView.reset(sf::FloatRect(0, 0, 640, 480));

	sf::View overLand;
	overLand.reset(sf::FloatRect(0, 0, 320, 240));

	//Undertale::UndertaleFile dataWin;
	if (argc != 2) exit(-1);
	LoadUndertaleResources(argv[1]);
	Undertale::LoadAllFonts();


	if (!debugFont.loadFromFile("resources\\DTM-Mono.otf"))
	{
		printf("Could not load debug font\n");
		exit(1);
	}

	//auto spr_icewolf = dataWin.LookupSprite(1302);


	

	windowLoop();



	
	
	return 0;
}
