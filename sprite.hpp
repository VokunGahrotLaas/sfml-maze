// C++ std
#include <iostream>
#include <stack>
#include <vector>
#include <unordered_set>

// SFML
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

// this project
#include "random.hpp"

namespace app {

sf::Uint64 color_to_int(const sf::Color& color) {
	return (sf::Uint64(color.r) << 24)
		| (sf::Uint64(color.g) << 16)
		| (sf::Uint64(color.b) << 8)
		| (sf::Uint64(color.a));
}

sf::Uint64 rgb_to_int(const sf::Color& color) {
	return (sf::Uint64(color.r) << 16)
		| (sf::Uint64(color.g) << 8)
		| (sf::Uint64(color.b));
}

sf::Color int_to_color(const sf::Uint64& i) {
	return sf::Color(
		i >> 24,
		i >> 16,
		i >> 8,
		i
	);
}

sf::Color int_to_rgb(const sf::Uint64& i) {
	return sf::Color(
		i >> 16,
		i >> 8,
		i
	);
}

template <class T>
bool in_stack(const std::stack<T>& stack, const T& t) {
	std::stack<T> tmp = stack;
	while (!tmp.empty()) {
		if (t == tmp.top())
			return true;
		tmp.pop();
	}
	return false;
}

} // namesapce app


namespace std {

template <>
class hash<sf::Color> {
public:
	size_t operator()(const sf::Color& color) const {
		return app::color_to_int(color);
	}
};

} // namespace std



namespace app {

class App;

class Sprite {
public:
	Sprite(void);
	virtual ~Sprite(void);

	sf::Uint64 count(void) const;

	virtual void draw(App& app) = 0;
	virtual void update(App& app) = 0;

protected:
	sf::Uint64 m_count;

	static sf::Uint64 global_count;
};

class Square: public Sprite {
public:
	Square(sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(1, 1), sf::Color color = sf::Color::Black);
	virtual ~Square(void);

	virtual void draw(App& app);
	virtual void update(App& app);

	void setPosition(sf::Vector2f pos);
	sf::Vector2f getPosition(void) const;
	void setSize(sf::Vector2f size);
	sf::Vector2f getSize(void) const;
	void setColor(sf::Color color);
	sf::Color getColor(void) const;

protected:
	sf::RectangleShape m_shape;
};

class SquareMatrix: public Sprite {
public:
	using Matrix = std::vector<std::vector<Square>>;
	using Array = std::vector<Square>;

	SquareMatrix(sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2u size = sf::Vector2u(1, 1), sf::Vector2f square_size = sf::Vector2f(1, 1), sf::Color square_color = sf::Color::Black);
	virtual ~SquareMatrix(void);

	virtual void draw(App& app);
	virtual void update(App& app);

	sf::Uint64 size(void) const;

	Matrix::iterator begin(void);
	Matrix::const_iterator begin(void) const;
	Matrix::const_iterator cbegin(void) const;

	Matrix::iterator end(void);
	Matrix::const_iterator end(void) const;
	Matrix::const_iterator cend(void) const;

	Array& operator[](sf::Uint64 index);
	const Array& operator[](sf::Uint64 index) const;

protected:
	Matrix m_matrix;
};

class Labyrinthe: public Sprite {
public:
	using PathfindStack = std::stack<std::pair<sf::Uint64, sf::Uint64>>;

	Labyrinthe(sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2u size = sf::Vector2u(1, 1), sf::Vector2f square_size = sf::Vector2f(1, 1));
	virtual ~Labyrinthe(void);

	virtual void draw(App& app);
	virtual void update(App& app);

protected:
	SquareMatrix m_matrix;
	Random<sf::Uint8> m_rand_uint8;
	Random<sf::Uint64> m_rand_uint64;
	bool m_construct_complete;
	bool m_pathfind_complete;
	bool m_pathfind2_complete;
	sf::Color m_pathfind_color;
	sf::Uint64 m_pathfind_counter;
	PathfindStack m_pathfind_stack;

	void construct(void);
	bool is_construct_complete(void);
	void pathfind(void);
	bool is_pathfind_complete(void);
	void pathfind2(void);
	bool is_pathfind2_complete(void);

	void change_color(sf::Uint64 x, sf::Uint64 y, sf::Color new_color);
	void pathfind_square(sf::Uint64 x, sf::Uint64 y, sf::Uint64 val);
	bool pathfind2_square(sf::Uint64 x, sf::Uint64 y, sf::Uint64 val = -1);
};

} // namespace app




// this project
#include "app.hpp"

namespace app {

Sprite::Sprite(void):
	m_count(Sprite::global_count++)
{
	//
}

Sprite::~Sprite(void) {
	//
}

sf::Uint64 Sprite::count(void) const {
	return m_count;
}

sf::Uint64 Sprite::global_count = 0;



Square::Square(sf::Vector2f pos, sf::Vector2f size, sf::Color color):
	Sprite(),
	m_shape()
{
	m_shape.setPosition(pos);
	m_shape.setSize(size);
	m_shape.setFillColor(color);
}

Square::~Square(void) {
	//
}

void Square::update(App& app) {
	//
}

void Square::draw(App& app) {
	app.m_window.draw(m_shape);
}

void Square::setPosition(sf::Vector2f pos) {
	m_shape.setPosition(pos);
}

sf::Vector2f Square::getPosition(void) const {
	return m_shape.getPosition();
}

void Square::setSize(sf::Vector2f size) {
	m_shape.setSize(size);
}

sf::Vector2f Square::getSize(void) const {
	return m_shape.getSize();
}

void Square::setColor(sf::Color color) {
	m_shape.setFillColor(color);
}

sf::Color Square::getColor(void) const {
	return m_shape.getFillColor();
}

SquareMatrix::SquareMatrix(sf::Vector2f pos, sf::Vector2u size, sf::Vector2f square_size, sf::Color square_color):
	Sprite(),
	m_matrix()
{
	const Square ref(pos, square_size, square_color);
	m_matrix.resize(size.x, Array());
	for (Array& subarray: m_matrix)
		subarray.resize(size.y, ref);
	for (sf::Uint64 x = 0; x < m_matrix.size(); ++x)
		for (sf::Uint64 y = 0; y < m_matrix[x].size(); ++y)
			m_matrix[x][y].setPosition(sf::Vector2f(
				pos.x + x * square_size.x,
				pos.y + y * square_size.y
			));
}

SquareMatrix::~SquareMatrix(void) {
	//
}

void SquareMatrix::draw(App& app) {
	for (Array& array: m_matrix)
		for (Square& square: array)
			square.draw(app);
}

void SquareMatrix::update(App& app) {
	for (Array& array: m_matrix)
		for (Square& square: array)
			square.update(app);
}

sf::Uint64 SquareMatrix::size(void) const {
	return m_matrix.size();
}

SquareMatrix::Matrix::iterator SquareMatrix::begin(void) {
	return m_matrix.begin();
}

SquareMatrix::Matrix::const_iterator SquareMatrix::begin(void) const {
	return m_matrix.begin();
}

SquareMatrix::Matrix::const_iterator SquareMatrix::cbegin(void) const {
	return m_matrix.cbegin();
}

SquareMatrix::Matrix::iterator SquareMatrix::end(void) {
	return m_matrix.end();
}

SquareMatrix::Matrix::const_iterator SquareMatrix::end(void) const {
	return m_matrix.end();
}

SquareMatrix::Matrix::const_iterator SquareMatrix::cend(void) const {
	return m_matrix.cend();
}

SquareMatrix::Array& SquareMatrix::operator[](sf::Uint64 index) {
	return m_matrix[index];
}

const SquareMatrix::Array& SquareMatrix::operator[](sf::Uint64 index) const {
	return m_matrix[index];
}

Labyrinthe::Labyrinthe(sf::Vector2f pos, sf::Vector2u size, sf::Vector2f square_size):
	m_matrix(pos, size, square_size, sf::Color::Black),
	m_rand_uint8(),
	m_rand_uint64(),
	m_construct_complete(false),
	m_pathfind_complete(false),
	m_pathfind2_complete(false),
	m_pathfind_stack()
{
	std::unordered_set<sf::Color> set;
	set.insert(sf::Color::Black);
	set.insert(sf::Color::White);
	for (sf::Uint64 x = 1; x < m_matrix.size(); x += 2) {
		for (sf::Uint64 y = 1; y < m_matrix[x].size(); y += 2) {
			sf::Color color = sf::Color::Black;
			while (set.count(color) == 1)
				color = sf::Color(m_rand_uint8(), m_rand_uint8(), m_rand_uint8());
			m_matrix[x][y].setColor(color);
			set.insert(color);
		}
	}
}

Labyrinthe::~Labyrinthe(void) {
	//
}

void Labyrinthe::draw(App& app) {
	m_matrix.draw(app);
}

void Labyrinthe::update(App& app) {
	m_matrix.update(app);
	if (!this->is_construct_complete()) {
		this->construct();
	} else if (!this->is_pathfind_complete()) {
		this->pathfind();
	} else if (!this->is_pathfind2_complete()) {
		this->pathfind2();
	}
}

void Labyrinthe::construct(void) {
	sf::Color old_color = sf::Color::Black, new_color = sf::Color::Black;
	sf::Uint64 x = 1, y = 1;
	bool sens = false;
	while (old_color == new_color) {
		x = 1;
		y = 1;
		while (m_matrix[x][y].getColor() != sf::Color::Black) {
			if ((m_rand_uint64() & 1) == 1) {
				sens = true;
				x = m_rand_uint64() % (m_matrix.size() / 2 - 1) * 2 + 2;
				y = m_rand_uint64() % (m_matrix.size() / 2) * 2 + 1;
			} else {
				sens = false;
				x = m_rand_uint64() % (m_matrix.size() / 2) * 2 + 1;
				y = m_rand_uint64() % (m_matrix.size() / 2 - 1) * 2 + 2;
			}
		}
		if (sens) {
			old_color = m_matrix[x - 1][y].getColor();
			new_color = m_matrix[x + 1][y].getColor();
		} else {
			old_color = m_matrix[x][y - 1].getColor();
			new_color = m_matrix[x][y + 1].getColor();
		}
	}
	m_matrix[x][y].setColor(new_color);
	if (sens)
		this->change_color(x - 1, y, new_color);
	else
		this->change_color(x, y - 1, new_color);
	if (this->is_construct_complete()) {
		m_construct_complete = true;
		this->change_color(1, 1, int_to_color(-1));
		m_matrix[0][1].setColor(sf::Color::White);
		m_matrix[m_matrix.size() - 1][m_matrix[0].size() - 2].setColor(sf::Color(64, 128, 0));
		m_pathfind_stack.push({m_matrix.size() - 1, m_matrix[0].size() - 2});
	}
}

bool Labyrinthe::is_construct_complete(void) {
	if (m_construct_complete)
		return true;
	sf::Color color = m_matrix[1][1].getColor();
	for (SquareMatrix::Array& array: m_matrix)
		for (Square& square: array)
			if (square.getColor() != color && square.getColor() != sf::Color::Black)
				return false;
	return true;
}

void Labyrinthe::pathfind(void) {
	PathfindStack stack;
	m_pathfind_stack.swap(stack);
	while (!stack.empty()) {
		sf::Uint64 x = stack.top().first, y = stack.top().second;
		sf::Uint64 val = rgb_to_int(m_matrix[x][y].getColor()) + 1;
		this->pathfind_square(x - 1, y, val);
		this->pathfind_square(x + 1, y, val);
		this->pathfind_square(x, y - 1, val);
		this->pathfind_square(x, y + 1, val);
		stack.pop();
	}
	if (this->is_pathfind_complete()) {
		m_pathfind_complete = true;
		while (!m_pathfind_stack.empty())
			m_pathfind_stack.pop();
		m_pathfind_stack.push({0, 1});
		m_matrix[0][1].setColor(sf::Color::Red);
	}
}

bool Labyrinthe::is_pathfind_complete(void) {
	if (m_pathfind_complete)
		return true;
	return m_matrix[0][1].getColor() != sf::Color::White;
}

void Labyrinthe::pathfind2(void) {
	sf::Uint64 x = m_pathfind_stack.top().first, y = m_pathfind_stack.top().second;
	m_pathfind_stack.pop();
	sf::Uint64 i = 0, j = 0, val = -1;
	if (pathfind2_square(x - 1, y, val)) {
		i = x - 1;
		j = y;
		val = rgb_to_int(m_matrix[i][j].getColor());
	}
	if (pathfind2_square(x + 1, y, val)) {
		i = x + 1;
		j = y;
		val = rgb_to_int(m_matrix[i][j].getColor());
	}
	if (pathfind2_square(x, y - 1, val)) {
		i = x;
		j = y - 1;
		val = rgb_to_int(m_matrix[i][j].getColor());
	}
	if (pathfind2_square(x, y + 1, val)) {
		i = x;
		j = y + 1;
		val = rgb_to_int(m_matrix[i][j].getColor());
	}
	m_pathfind_stack.push({i, j});
	m_matrix[i][j].setColor(sf::Color::Red);
	if (this->is_pathfind2_complete())
		m_pathfind2_complete = true;
}

bool Labyrinthe::is_pathfind2_complete(void) {
	if (m_pathfind2_complete)
		return true;
	return m_pathfind_stack.top().first == m_matrix.size() - 1 && m_pathfind_stack.top().second == m_matrix[0].size() - 2;
}

void Labyrinthe::change_color(sf::Uint64 x, sf::Uint64 y, sf::Color new_color) {
	sf::Color old_color = m_matrix[x][y].getColor();
	if (old_color == new_color || old_color == sf::Color::Black)
		return;
	m_matrix[x][y].setColor(new_color);
	this->change_color(x - 1, y, new_color);
	this->change_color(x + 1, y, new_color);
	this->change_color(x, y - 1, new_color);
	this->change_color(x, y + 1, new_color);
}

void Labyrinthe::pathfind_square(sf::Uint64 x, sf::Uint64 y, sf::Uint64 val) {
	if (0 > x || x >= m_matrix.size())
		return;
	if (0 > y || y >= m_matrix[x].size())
		return;
	if (m_matrix[x][y].getColor() == sf::Color::Black)
		return;
	if (rgb_to_int(m_matrix[x][y].getColor()) <= val)
		return;
	m_matrix[x][y].setColor(int_to_rgb(val));
	if (in_stack(m_pathfind_stack, {x, y}) == 0)
		m_pathfind_stack.push({x, y});
}

bool Labyrinthe::pathfind2_square(sf::Uint64 x, sf::Uint64 y, sf::Uint64 val) {
	if (0 > x || x >= m_matrix.size())
		return false;
	if (0 > y || y >= m_matrix[x].size())
		return false;
	if (m_matrix[x][y].getColor() == sf::Color::Black)
		return false;
	if (rgb_to_int(m_matrix[x][y].getColor()) > val)
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

template <>
class hash<app::Square> {
public:
	size_t operator()(const app::Square& square) const {
		return square.count();
	}
};

template <>
class hash<app::SquareMatrix> {
public:
	size_t operator()(const app::SquareMatrix& matrix) const {
		return matrix.count();
	}
};

template <>
class hash<app::Labyrinthe> {
public:
	size_t operator()(const app::Labyrinthe& labyrinthe) const {
		return labyrinthe.count();
	}
};

} // namespace std



