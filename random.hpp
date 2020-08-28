#ifndef RANDOM_HPP
#define RANDOM_HPP

// C++ std
#include <random>

#include <SFML/System.hpp>

namespace app {

template <class T, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()>
class Random {
public:
	Random(void);
	virtual ~Random(void);

	T operator()(void);

protected:
	std::default_random_engine m_gen;
	std::uniform_int_distribution<T> m_distrib;
};

} // namespace app




namespace app {

template <class T, T min, T max>
Random<T, min, max>::Random(void):
	m_gen(std::random_device()()),
	m_distrib(min, max)
{
	//
}

template <class T, T min, T max>
Random<T, min, max>::~Random(void) {
	//
}

template <class T, T min, T max>
T Random<T, min, max>::operator()(void) {
	return m_distrib(m_gen);
}

} // namespace app

#endif // RANDOM_HPP