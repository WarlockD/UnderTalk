#include "Global.h"
#include "Effect.hpp"
#include <vector>
#include <cmath>
#include "obj_writer.h"
#include "obj_vaporized_new.h"
#include "room.h"


void windowLoop(gm::DataWinFile& undertale_file) {
	
#if 0
	sf::Font debugFont;
	if (!debugFont.loadFromFile("resources\\DTM-Mono.otf"))
	{
		printf("Could not load debug font\n");
		exit(1);
	}
#endif
	sf::View battleView;
	battleView.reset(sf::FloatRect(0, 0, 640, 480));

	sf::View overLand(sf::FloatRect(0, 0, 320, 240));


	//sf::Text fpsText;
//	fpsText.setFont(debugFont);
	//fpsText.setCharacterSize(30);
//	fpsText.setPosition(0.0f, 0.0f);
	auto res = GetUndertale();
	size_t room_num = 20;

	Room testRoom(undertale_file);
	testRoom.loadRoom(room_num);
	size_t num = 0;

	//GSprite test_head(230);
	//test_head.setPosition(10, 10);
	//obj_vaporized_new vtest(test_head,true);
//	obj_vaporized_new vtest(num, false);
	//vtest.setPosition(100, 100);
	//vtest.setScale(2.0f, 2.0f);
	OBJ_WRITER writer(testRoom);
	
	writer.DebugSetFace(1, 99);
	///global.msc = 0;
//	global.typer = 5;
//	global.facechoice = 0;
//	global.faceemotion = 0;
	//writer.setTy
	writer.SetTextType(5);
	writer.AddText("* (It\'s a carefully decorated&  tree.)/");
	writer.AddText("* (Some of the presents are&  addressed from \"Santa\" to&  various locals.)/%%");
	//writer.AddText("*This is a \\Ytest\\W&* and another \\z%%");
	writer.setPosition(100, 100);
	writer.Reset();
	//testRoom.addChild(&writer);
	// Start the game loop
	sf::Clock clock;
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "UnderTalk", sf::Style::Titlebar | sf::Style::Close);
	sf::RenderTexture render_texture;
	assert(render_texture.create(640, 480));
	window.setVerticalSyncEnabled(true);
	sf::View temp = render_texture.getView();
	temp.zoom(4.0f);
	render_texture.setView(temp);
	//window.setViewPort
	while (window.isOpen())
	{
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				printf("window closed done\r\n");
				return; // all done
			case sf::Event::KeyPressed:
				if (NodeEvent<sf::Event>::brodcast(event)) break;
				switch (event.key.code)
				{
					// Escape key: exit
				case sf::Keyboard::Escape:
					window.close();

					continue;
					break;
				case sf::Keyboard::D:
					room_num++;
					testRoom.loadRoom(room_num);
					break;
				case sf::Keyboard::A:
					room_num--;
					testRoom.loadRoom(room_num);
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
				break;
			case sf::Event::KeyReleased:
				NodeEvent<sf::Event>::brodcast(event); break;
			case sf::Event::MouseButtonPressed:
				NodeEvent<sf::Event>::brodcast(event); break;
			case sf::Event::MouseButtonReleased:
				NodeEvent<sf::Event>::brodcast(event); break;
			case sf::Event::MouseMoved:
				NodeEvent<sf::Event>::brodcast(event); break;

			}
		}
		float current = clock.getElapsedTime().asSeconds();
		if (clock.getElapsedTime().asSeconds() > (1.0f / 30.0f)) {
			float elapsed = clock.restart().asSeconds();
			writer.step(elapsed);
			testRoom.step(elapsed);
			//vtest.step(elapsed);
			float ffps = 1.0f / elapsed;
			//	fpsText.setString(std::to_string(std::floorf(ffps)));
		}
		//window.setView(overLand);
		// Clear the window
		window.clear(sf::Color(0, 0, 0));
		render_texture.clear(sf::Color(0, 0, 0));
	//	render_texture.setView(window.getView());
		//render_texture.setView(overLand);
		render_texture.draw(testRoom);
		render_texture.display();
		sf::Sprite sprite(render_texture.getTexture());
		window.setView(battleView);
		window.draw(sprite);
		//window.draw(testRoom);
		//window.draw(fpsText);

		window.display();
	}
}

int main(int argc, const char* argv[])
{
	// undertail combat views are in 640x480 but overowrd is in 320 240 fyi
	//String test = "booob!";
	gm::DataWinFile undertale_file;
	




	//Undertale::UndertaleFile dataWin;
	if (argc != 2) exit(-1);
	undertale_file.load(argv[1]);
	gm::Room test = undertale_file.resource_at<gm::Room>(34);
	gm::Sprite stest = undertale_file.resource_at<gm::Sprite>(34);
	std::cerr << "room: " << test << std::endl;
	std::cerr << "sprite: " << stest << std::endl;
	windowLoop(undertale_file);

	return 0;
}
