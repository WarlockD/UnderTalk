////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Effect.hpp"
#include <vector>
#include <cmath>
#include "obj_writer.h"


const sf::Font* Effect::s_font = NULL;

int old_main() {
	std::srand(static_cast<unsigned int>(std::time(NULL)));
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Shader",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	// Load the application font and pass it to the Effect class
	sf::Font font;
	if (!font.loadFromFile("resources/sansation.ttf"))
		return EXIT_FAILURE;
	Effect::setFont(font);

	// Create the effects
	std::vector<Effect*> effects;
	effects.push_back(new Pixelate);
	effects.push_back(new WaveBlur);
	effects.push_back(new StormBlink);
	effects.push_back(new Edge);
	effects.push_back(new Geometry);
	std::size_t current = 0;

	// Initialize them
	for (std::size_t i = 0; i < effects.size(); ++i)
		effects[i]->load();

	// Create the messages background
	sf::Texture textBackgroundTexture;
	if (!textBackgroundTexture.loadFromFile("resources/text-background.png"))
		return EXIT_FAILURE;
	sf::Sprite textBackground(textBackgroundTexture);
	textBackground.setPosition(0, 520);
	textBackground.setColor(sf::Color(255, 255, 255, 200));

	// Create the description text
	sf::Text description("Current effect: " + effects[current]->getName(), font, 20);
	description.setPosition(10, 530);
	description.setFillColor(sf::Color(80, 80, 80));

	// Create the instructions text
	sf::Text instructions("Press left and right arrows to change the current shader", font, 20);
	instructions.setPosition(280, 555);
	instructions.setFillColor(sf::Color(80, 80, 80));

	// Start the game loop
	sf::Clock clock;
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

					// Left arrow key: previous shader
				case sf::Keyboard::Left:
					if (current == 0)
						current = effects.size() - 1;
					else
						current--;
					description.setString("Current effect: " + effects[current]->getName());
					break;

					// Right arrow key: next shader
				case sf::Keyboard::Right:
					if (current == effects.size() - 1)
						current = 0;
					else
						current++;
					description.setString("Current effect: " + effects[current]->getName());
					break;

				default:
					break;
				}
			}
		}

		// Update the current example
		float x = static_cast<float>(sf::Mouse::getPosition(window).x) / window.getSize().x;
		float y = static_cast<float>(sf::Mouse::getPosition(window).y) / window.getSize().y;
		effects[current]->update(clock.getElapsedTime().asSeconds(), x, y);

		// Clear the window
		window.clear(sf::Color(255, 128, 0));

		// Draw the current example
		window.draw(*effects[current]);

		// Draw the text
		window.draw(textBackground);
		window.draw(instructions);
		window.draw(description);

		// Finally, display the rendered frame on screen
		window.display();
	}

	// delete the effects
	for (std::size_t i = 0; i < effects.size(); ++i)
		delete effects[i];

	return EXIT_SUCCESS;
}


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////


int main()
{
	OBJ_WRITER::LoadAllFonts();
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Shader",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	OBJ_WRITER writer;
	writer.AddText("*This is a \\Ytest\\W&* and another%%");
	writer.SetTextType(1);
	// Start the game loop
	sf::Clock clock;
	float current = clock.getElapsedTime().asSeconds() + (1 / 30.0f);
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
				}
			}
		}
		float now = clock.getElapsedTime().asSeconds();
		if (now > current) {
			writer.frame();
			current = now + (1 / 30.0f);
		}
		// Clear the window
		window.clear(sf::Color(0, 0, 0));
		//effects[current]->update(clock.getElapsedTime().asSeconds(), x, y);
		
		window.draw(writer);
		// Finally, display the rendered frame on screen
		window.display();
	}
}
