// C++ std
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include <unordered_set>

// SFML
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

// this project
#include "random.hpp"
#include "utils.hpp"



namespace app {

class Sprite: public sf::Drawable, public sf::Transformable {
public:
	Sprite(void);
	~Sprite(void) override = default;

	unsigned long count(void) const;
	virtual void update(void) = 0;

private:
	unsigned long m_count;

	static unsigned long global_count;
};

template <typename T>
class SpriteMatrix: public Sprite {
public:
	using Array = typename std::vector<T>;
	using Matrix = std::vector<Array>;

	template <typename... Args>
	SpriteMatrix(sf::Vector2st size, Args&&... args);
	~SpriteMatrix(void) override = default;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void update(void) override;

	typename Matrix::iterator begin(void);
	typename Matrix::const_iterator begin(void) const;
	typename Matrix::const_iterator cbegin(void) const;

	typename Matrix::iterator end(void);
	typename Matrix::const_iterator end(void) const;
	typename Matrix::const_iterator cend(void) const;

	Array& operator[](std::size_t index);
	const Array& operator[](std::size_t index) const;

	Array& at(std::size_t index);
	const Array& at(std::size_t index) const;

	const sf::Vector2st& getSize(void) const;

protected:
	Matrix m_matrix;
	sf::Vector2st m_size;
};

class Maze: public Sprite {
public:
	using PathfindSet = std::unordered_set<std::pair<unsigned long, unsigned long>>;

	Maze(sf::Vector2st size, float tile_size = 1, float destroy = 0.01);
	~Maze(void) override = default;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void update(void) override;

protected:
	sf::Clock m_clock;
	SpriteMatrix<sf::RectangleShape> m_matrix;
	Random<sf::Uint8> m_rand_u8;
	Random<unsigned long> m_rand_ul;
	bool m_construct_complete;
	bool m_pathfind_complete;
	bool m_pathfind2_complete;
	PathfindSet m_pathfind_set;
	float m_destroy;

	void construct(void);
	bool is_construct_complete(void) const;
	void pathfind(void);
	bool is_pathfind_complete(void) const;
	void pathfind2(void);
	bool is_pathfind2_complete(void) const;

	void change_color(std::size_t x, std::size_t y, const sf::Color& new_color);
	void break_some_walls(void);
	void pathfind_square(std::size_t x, std::size_t y, unsigned long val);
	bool pathfind2_square(std::size_t x, std::size_t y, unsigned long val = -1);
};

} // namespace app



namespace app {

Sprite::Sprite(void):
	m_count(Sprite::global_count++)
{
	//
}

unsigned long Sprite::count(void) const {
	return m_count;
}

unsigned long Sprite::global_count = 0;



template <typename T>
template <typename... Args>
SpriteMatrix<T>::SpriteMatrix(sf::Vector2st size, Args&&... args):
	Sprite(),
	m_matrix(),
	m_size(size)
{
	const T ref(args...);
	m_matrix.resize(m_size.x);
	for (Array& array: m_matrix)
		array.resize(m_size.y, ref);
	for (std::size_t x = 0; x < m_size.x; ++x)
		for (std::size_t y = 0; y < m_size.y; ++y)
			m_matrix[x][y].move(
				x * m_matrix[x][y].getSize().x,
				y * m_matrix[x][y].getSize().y
			);
}

template <typename T>
void SpriteMatrix<T>::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= this->getTransform();
	for (const Array& array: m_matrix)
		for (const T& sprite: array)
			target.draw(sprite, states);
}

template <typename T>
void SpriteMatrix<T>::update(void) {
	//
}

template <typename T>
typename SpriteMatrix<T>::Matrix::iterator SpriteMatrix<T>::begin(void) {
	return m_matrix.begin();
}

template <typename T>
typename SpriteMatrix<T>::Matrix::const_iterator SpriteMatrix<T>::begin(void) const {
	return m_matrix.cbegin();
}

template <typename T>
typename SpriteMatrix<T>::Matrix::const_iterator SpriteMatrix<T>::cbegin(void) const {
	return m_matrix.cbegin();
}

template <typename T>
typename SpriteMatrix<T>::Matrix::iterator SpriteMatrix<T>::end(void) {
	return m_matrix.end();
}

template <typename T>
typename SpriteMatrix<T>::Matrix::const_iterator SpriteMatrix<T>::end(void) const {
	return m_matrix.cend();
}

template <typename T>
typename SpriteMatrix<T>::Matrix::const_iterator SpriteMatrix<T>::cend(void) const {
	return m_matrix.cend();
}

template <typename T>
typename SpriteMatrix<T>::Array& SpriteMatrix<T>::operator[](std::size_t index) {
	return m_matrix[index];
}

template <typename T>
const typename SpriteMatrix<T>::Array& SpriteMatrix<T>::operator[](std::size_t index) const {
	return m_matrix[index];
}

template <typename T>
typename SpriteMatrix<T>::Array& SpriteMatrix<T>::at(std::size_t index) {
	return m_matrix.at(index);
}

template <typename T>
const typename SpriteMatrix<T>::Array& SpriteMatrix<T>::at(std::size_t index) const {
	return m_matrix.at(index);
}

template <typename T>
const sf::Vector2st& SpriteMatrix<T>::getSize(void) const {
	return m_size;
}

Maze::Maze(sf::Vector2st size, float tile_size, float destroy):
	m_clock(),
	m_matrix(size, sf::Vector2f(tile_size, tile_size)),
	m_rand_u8(),
	m_rand_ul(),
	m_construct_complete(false),
	m_pathfind_complete(false),
	m_pathfind2_complete(false),
	m_pathfind_set(),
	m_destroy(destroy)
{
	for (auto& array: m_matrix)
		for (auto& tile: array)
			tile.setFillColor(sf::Color::Black);
	std::unordered_set<sf::Color> already_used;
	already_used.insert(sf::Color::Black);
	already_used.insert(sf::Color::White);
	for (std::size_t x = 1; x < m_matrix.getSize().x; x += 2) {
		for (std::size_t y = 1; y < m_matrix.getSize().y; y += 2) {
			sf::Color color = sf::Color::Black;
			while (already_used.count(color) == 1)
				color = sf::Color(m_rand_u8(), m_rand_u8(), m_rand_u8());
			m_matrix[x][y].setFillColor(color);
			already_used.insert(color);
		}
	}
}

void Maze::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= this->getTransform();
	target.draw(m_matrix, states);
}

void Maze::update(void) {
	if (!this->is_construct_complete()) {
		this->construct();
	} else if (!this->is_pathfind_complete()) {
		this->pathfind();
	} else if (!this->is_pathfind2_complete()) {
		this->pathfind2();
	}
}

void Maze::construct(void) {
	sf::Color old_color = sf::Color::Black, new_color = sf::Color::Black;
	unsigned long x = 1, y = 1;
	bool sens = false;
	while (old_color == new_color) {
		x = 1;
		y = 1;
		while (m_matrix[x][y].getFillColor() != sf::Color::Black) {
			if ((m_rand_u8() & 1) == 1) {
				sens = true;
				x = m_rand_ul() % (m_matrix.getSize().x / 2 - 1) * 2 + 2;
				y = m_rand_ul() % (m_matrix.getSize().y / 2) * 2 + 1;
			} else {
				sens = false;
				x = m_rand_ul() % (m_matrix.getSize().x / 2) * 2 + 1;
				y = m_rand_ul() % (m_matrix.getSize().y / 2 - 1) * 2 + 2;
			}
		}
		if (sens) {
			old_color = m_matrix[x - 1][y].getFillColor();
			new_color = m_matrix[x + 1][y].getFillColor();
		} else {
			old_color = m_matrix[x][y - 1].getFillColor();
			new_color = m_matrix[x][y + 1].getFillColor();
		}
	}
	m_matrix[x][y].setFillColor(new_color);
	if (sens)
		this->change_color(x - 1, y, new_color);
	else
		this->change_color(x, y - 1, new_color);
	if (this->is_construct_complete()) {
		this->break_some_walls();
		m_construct_complete = true;
		this->change_color(1, 1, int_to_color(-1));
		m_matrix[0][1].setFillColor(sf::Color::White);
		m_matrix[m_matrix.getSize().x - 1][m_matrix.getSize().y - 2].setFillColor(sf::Color(64, 128, 0));
		m_pathfind_set.insert({m_matrix.getSize().x - 1, m_matrix.getSize().y - 2});
	}
}

bool Maze::is_construct_complete(void) const {
	if (m_construct_complete)
		return true;
	sf::Color color = m_matrix[1][1].getFillColor();
	for (auto& array: m_matrix)
		for (auto& tile: array)
			if (tile.getFillColor() != color && tile.getFillColor() != sf::Color::Black)
				return false;
	return true;
}

void Maze::pathfind(void) {
	PathfindSet set;
	m_pathfind_set.swap(set);
	for (auto& pair: set) {
		std::size_t x = pair.first, y = pair.second;
		unsigned long val = rgb_to_int(m_matrix[x][y].getFillColor()) + 1;
		this->pathfind_square(x - 1, y, val);
		this->pathfind_square(x + 1, y, val);
		this->pathfind_square(x, y - 1, val);
		this->pathfind_square(x, y + 1, val);
	}
	if (this->is_pathfind_complete()) {
		m_pathfind_complete = true;
		m_pathfind_set.clear();
		m_pathfind_set.insert({0, 1});
		m_matrix[0][1].setFillColor(sf::Color::Red);
	}
}

bool Maze::is_pathfind_complete(void) const {
	if (m_pathfind_complete)
		return true;
	return m_matrix[0][1].getFillColor() != sf::Color::White;
}

void Maze::pathfind2(void) {
	std::size_t x = m_pathfind_set.begin()->first, y = m_pathfind_set.begin()->second;
	m_pathfind_set.erase({x, y});
	std::size_t i = 0, j = 0;
	unsigned long val = -1;
	if (pathfind2_square(x - 1, y, val)) {
		i = x - 1;
		j = y;
		val = rgb_to_int(m_matrix[i][j].getFillColor());
	}
	if (pathfind2_square(x + 1, y, val)) {
		i = x + 1;
		j = y;
		val = rgb_to_int(m_matrix[i][j].getFillColor());
	}
	if (pathfind2_square(x, y - 1, val)) {
		i = x;
		j = y - 1;
		val = rgb_to_int(m_matrix[i][j].getFillColor());
	}
	if (pathfind2_square(x, y + 1, val)) {
		i = x;
		j = y + 1;
		val = rgb_to_int(m_matrix[i][j].getFillColor());
	}
	m_pathfind_set.insert({i, j});
	m_matrix[i][j].setFillColor(sf::Color::Red);
	if (this->is_pathfind2_complete()) {
		m_pathfind2_complete = true;
		sf::Time t = m_clock.getElapsedTime();
		std::cout
			<< "Time elapsed: " << std::endl
			<< "  " << t.asSeconds() << " s" << std::endl
			<< "  " << t.asMilliseconds() << " ms" << std::endl
			<< "  " << t.asMicroseconds() << " µs" << std::endl;
	}
}

bool Maze::is_pathfind2_complete(void) const {
	if (m_pathfind2_complete)
		return true;
	return m_pathfind_set.count({m_matrix.getSize().x - 1, m_matrix.getSize().y - 2}) == 1;
}

void Maze::change_color(std::size_t x, std::size_t y, const sf::Color& new_color) {
	sf::Color old_color = m_matrix[x][y].getFillColor();
	if (old_color == new_color || old_color == sf::Color::Black)
		return;
	m_matrix[x][y].setFillColor(new_color);
	this->change_color(x - 1, y, new_color);
	this->change_color(x + 1, y, new_color);
	this->change_color(x, y - 1, new_color);
	this->change_color(x, y + 1, new_color);
}

void Maze::break_some_walls(void) {
	if (m_destroy < 0)
		m_destroy = 0;
	if (m_destroy > 1)
		m_destroy = 1;
	unsigned long to_break = (m_matrix.getSize().x / 2 - 1) * (m_matrix.getSize().y / 2 - 1);
	to_break *= m_destroy;
	std::cout << "extra walls broken: " << to_break << std::endl;
	unsigned long x = 1, y = 1;
	for (; to_break > 0; --to_break) {
		x = 1;
		y = 1;
		while (m_matrix[x][y].getFillColor() != sf::Color::Black) {
			if ((m_rand_u8() & 1) == 1) {
				x = m_rand_ul() % (m_matrix.getSize().x / 2 - 1) * 2 + 2;
				y = m_rand_ul() % (m_matrix.getSize().y / 2) * 2 + 1;
			} else {
				x = m_rand_ul() % (m_matrix.getSize().x / 2) * 2 + 1;
				y = m_rand_ul() % (m_matrix.getSize().y / 2 - 1) * 2 + 2;
			}
		}
		m_matrix[x][y].setFillColor(sf::Color::White);
	}
}

void Maze::pathfind_square(std::size_t x, std::size_t y, unsigned long val) {
	if (0 > x || x >= m_matrix.getSize().x)
		return;
	if (0 > y || y >= m_matrix.getSize().y)
		return;
	if (m_matrix[x][y].getFillColor() == sf::Color::Black)
		return;
	if (rgb_to_int(m_matrix[x][y].getFillColor()) <= val)
		return;
	m_matrix[x][y].setFillColor(int_to_rgb(val));
	if (m_pathfind_set.count({x, y}) == 0)
		m_pathfind_set.insert({x, y});
}

bool Maze::pathfind2_square(std::size_t x, std::size_t y, unsigned long val) {
	if (0 > x || x >= m_matrix.getSize().x)
		return false;
	if (0 > y || y >= m_matrix.getSize().y)
		return false;
	if (m_matrix[x][y].getFillColor() == sf::Color::Black)
		return false;
	if (rgb_to_int(m_matrix[x][y].getFillColor()) > val)
		return false;
	return true;
}

} // namespace app



namespace std {

template <>
class hash<app::Sprite> {
public:
	size_t operator()(const app::Sprite& sprite) const {
		return sprite.count();
	}
};

template <typename T>
class hash<app::SpriteMatrix<T>> {
public:
	size_t operator()(const app::SpriteMatrix<T>& matrix) const {
		return matrix.count();
	}
};

template <>
class hash<app::Maze> {
public:
	size_t operator()(const app::Maze& maze) const {
		return maze.count();
	}
};

} // namespace std



