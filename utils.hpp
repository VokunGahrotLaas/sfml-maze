#ifndef APP_UTILS_HPP
#define APP_UTILS_HPP

// C++ std
#include <functional>

// SFML
#include <SFML/Graphics.hpp>



namespace sf {

using Vector2ul = Vector2<unsigned long>;
using Vector2st = Vector2<std::size_t>;

}



namespace app {

unsigned long color_to_int(const sf::Color& color) {
	return (sf::Uint64(color.r) << 24)
		| (sf::Uint64(color.g) << 16)
		| (sf::Uint64(color.b) << 8)
		| (sf::Uint64(color.a));
}

unsigned long rgb_to_int(const sf::Color& color) {
	return (sf::Uint64(color.r) << 16)
		| (sf::Uint64(color.g) << 8)
		| (sf::Uint64(color.b));
}

sf::Color int_to_color(unsigned long i) {
	return sf::Color(
		i >> 24,
		i >> 16,
		i >> 8,
		i
	);
}

sf::Color int_to_rgb(unsigned long i) {
	return sf::Color(
		i >> 16,
		i >> 8,
		i
	);
}

std::size_t combine_hash(std::size_t a, std::size_t b) {
	return a ^ (b << 1);
}

}



namespace std {

template <>
class hash<sf::Color> {
public:
	std::size_t operator()(const sf::Color& color) const {
		sf::Uint64 hash = app::color_to_int(color);
		if (sizeof(std::size_t) == 2)
			return app::combine_hash(hash, hash >> 16);
		else
			return hash;
	}
};

template <typename Ta, typename Tb>
class hash<std::pair<Ta, Tb>> {
public:
	std::size_t operator()(const std::pair<Ta, Tb>& pair) const {
		return app::combine_hash(std::hash<Ta>()(pair.first), std::hash<Tb>()(pair.second));
	}
};

} // namespace std

#endif // APP_UTILS_HPP