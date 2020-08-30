#ifndef APP_RANDOM_HPP
#define APP_RANDOM_HPP

// C++ std
#include <random>
#include <ctime>

namespace app {

template <typename T, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()>
class Random {
public:
	Random(void);
	virtual ~Random(void) = default;

	T operator()(void);

protected:
	std::default_random_engine m_gen;
	std::uniform_int_distribution<T> m_distrib;
};

} // namespace app



namespace app {

template <typename T, T min, T max>
Random<T, min, max>::Random(void):
	m_gen(std::time(nullptr)), // std::random_device()() not working on my side
	m_distrib(min, max)
{
	//
}

template <typename T, T min, T max>
T Random<T, min, max>::operator()(void) {
	return m_distrib(m_gen);
}

} // namespace app

#endif // APP_RANDOM_HPP