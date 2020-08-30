#ifndef APP_HPP
#define APP_HPP

// C++ std
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

// SFML
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

// this project
#include "sprite.hpp"
#include "utils.hpp"

namespace app {

class App {
public:
	using SpriteSet = std::unordered_set<Sprite*>;

	App(int argc = 0, char** argv = nullptr);
	virtual ~App(void);

	void start(void);
	void event(void);
	void update(void);
	void draw(void);

protected:
	std::vector<std::string> m_args;
	sf::RenderWindow m_window;
	sf::Event m_event;
	SpriteSet m_sprites;
	sf::Thread m_update_thread;
	sf::Mutex m_sprites_mutex;
	float m_update_fps;
};

} // namesapce app



namespace app {

App::App(int argc, char** argv):
	m_args(),
	m_window(
		sf::VideoMode::getDesktopMode(),
		"sfml-app",
		sf::Style::Fullscreen
	),
	m_event(),
	m_sprites(),
	m_update_thread(&App::update, this),
	m_sprites_mutex()
{
	// parse args
	m_args.reserve(argc);
	for (int i = 0; i < argc; ++i)
		m_args.push_back(argv[i]);
	// setup window
	m_window.setVerticalSyncEnabled(true);
	// create sprites
	sf::Vector2u screen_size = m_window.getSize();
	sf::Vector2st maze_size(256, 144);
	if (m_args.size() > 1)
		maze_size.y = std::stoul(m_args[1]);
	maze_size.y += (~maze_size.y & 1);
	float tile_size = (float)screen_size.y / (float)(maze_size.y - 1);
	maze_size.x = (float)screen_size.x / tile_size;
	maze_size.x += (~maze_size.x & 1);
	float destroy = 0.01;
	if (m_args.size() > 3)
		destroy = std::stof(m_args[3]);
	std::cout
		<< "usage: sfml-maze [<maze y size>=144] [<update max fps>=960]" << std::endl
		<< "advised sizes: [144, 240, 360, 432, 450, 480, 504, 540, 576, 648, 720, 768, 900, 1080]" << std::endl
		<< "maze size: (" << maze_size.x << ", " << maze_size.y << ")" << std::endl
		<< "tile size: " << tile_size << std::endl
	;
	m_sprites.insert(new Maze(maze_size, tile_size, destroy));
}

App::~App(void) {
	for (Sprite* sprite: m_sprites)
		delete sprite;
}

void App::start(void) {
	m_update_thread.launch();
	while (m_window.isOpen()) {
		this->event();
		this->draw();
	}
	m_update_thread.wait();
}

void App::event(void) {
	while (m_window.pollEvent(m_event)) {
		if (m_event.type == sf::Event::Closed)
			m_window.close();
		if (m_event.type == sf::Event::KeyPressed && m_event.key.code == sf::Keyboard::Escape)
			m_window.close();
	}
}

void App::update(void) {
	sf::Clock clock;
	float fps = 60 * 16;
	if (m_args.size() > 2)
		fps = std::stof(m_args[2]);
	sf::Time spf;
	if (fps > 0.0) {
		spf = sf::seconds(1.0 / fps);
		while (m_window.isOpen()) {
			{
				sf::Lock lock(m_sprites_mutex);
				for (Sprite* sprite: m_sprites)
					sprite->update();
			}
			if (clock.getElapsedTime() < spf)
				sf::sleep(spf - clock.getElapsedTime());
			clock.restart();
		}
	} else if (fps == 0.0) {
		while (m_window.isOpen()) {
			sf::Lock lock(m_sprites_mutex);
			for (Sprite* sprite: m_sprites)
				sprite->update();
		}
	} else {
		while (m_window.isOpen()) {
			for (Sprite* sprite: m_sprites)
				sprite->update();
		}
	}
}

void App::draw(void) {
	m_window.clear(sf::Color::White);
	{
		sf::Lock lock(m_sprites_mutex);
			for (Sprite* sprite: m_sprites)
				m_window.draw(*sprite);
	}
	m_window.display();
}

} // namespace app


#endif // APP_HPP



