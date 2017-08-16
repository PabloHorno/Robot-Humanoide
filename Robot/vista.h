#include <SFML/Graphics.hpp>
#include <iostream>
#define MARGEN 10
namespace vista {

	class Vista {
	public:
		sf::RenderWindow control_ui;

		void render() {
			control_ui.setTitle("Ventana de Control");

			sf::RenderWindow window(sf::VideoMode(800, 600), "Ventana de Control");
			//Motor motor;
			sf::Font fuente;
			if (!fuente.loadFromFile("sansation.ttf"))
				std::cout << "Error" << std::endl;
			sf::Text texto;
			texto.setFont(fuente);
			texto.setString("MAXIMO");

			texto.setPosition(400 - texto.getGlobalBounds().width / 2, 0);
			texto.setFillColor(sf::Color::Blue);

			sf::RectangleShape rect({ 350.f, 500.f });
			rect.setFillColor(sf::Color::White);
			rect.setPosition(0 + MARGEN, texto.getGlobalBounds().height * 2);

			while (window.isOpen())
			{
				sf::Event event;
				while (window.pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
						window.close();
				}

				window.clear(sf::Color(30, 30, 30));
				window.draw(texto);
				window.draw(rect);
				for (unsigned i = 0; i < 18; i++)
				{
					//motor.move(sf::Vector2f(0.f, 1.f));
					//window.draw(motor);
				}
				window.display();
			}
		}
	private:

	};

	class Motor: public sf::Sprite {
	public:
		Motor():Sprite()
		{
			if (textura.loadFromFile("AX-12.png"))
			{
				textura.setSmooth(true);
				setTexture(textura);
				setPosition(sf::Vector2f(20, 70));
				setTexture(textura);
				setScale(sf::Vector2f(0.2f, 0.2f));
			}
		}
	private:
		static sf::Texture textura;
	};
}