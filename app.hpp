#ifndef APP_HPP
#define APP_HPP

// C++ std
#include <string>
#include <vector>
#include <unordered_set>
#include <queue>

// SFML
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

namespace app {

class Sprite;

using Layer = std::unordered_set<Sprite*>;

class App {
public:
	App(int argc, char** argv);
	virtual ~App(void);

	void start(void);
	void event(void);
	void update(void);
	void draw(void);

	friend class Sprite;
	friend class Square;
	friend class SquareMatrix;

protected:
	std::vector<std::string> m_args;
	sf::RenderWindow m_window;
	sf::Event m_event;
	std::vector<Layer> m_layers;
};

} // namesapce app



#include "sprite.hpp"

namespace app {

App::App(int argc, char** argv):
	m_args(),
	m_window(
		sf::VideoMode::getDesktopMode(),
		"sfml-app",
		sf::Style::Fullscreen
	),
	m_event(),
	m_layers(3)
{
	// parse args
	m_args.reserve(argc);
	for (int i = 0; i < argc; ++i)
		m_args.push_back(argv[i]);
	// setup window
	m_window.setVerticalSyncEnabled(true);
	// summon sprites
	m_layers[0].insert(new Labyrinthe(sf::Vector2f(16, 16), sf::Vector2u(33, 33), sf::Vector2f(32, 32)));
}

App::~App(void) {
	for (Layer& layer: m_layers)
		for (Sprite* sprite: layer)
			if (sprite != nullptr)
				delete sprite;
}

void App::start(void) {
	while (m_window.isOpen()) {
		this->event();
		this->update();
		this->draw();
		//sf::sleep(sf::seconds(0.1));
	}
}

void App::event(void) {
	while (m_window.pollEvent(m_event)) {
		if (m_event.type == sf::Event::Closed)
			m_window.close();
	}
}

void App::update(void) {
	for (Layer& layer: m_layers)
		for (Sprite* sprite: layer)
			sprite->update(*this);
}

void App::draw(void) {
	m_window.clear(sf::Color::White);
	for (Layer& layer: m_layers)
		for (Sprite* sprite: layer)
		       sprite->draw(*this);	
	m_window.display();
}

} // namespace app


#endif // APP_HPP



