#ifndef AURORA_NEURAL_NETWORK
#define AURORA_NEURAL_NETWORK

#ifdef QT_CORE_LIB
#include <QVector>
#include <GL/gl.h>

namespace Aurora {
template <class T>
using vec = QVector<T>;
}
#else
#include <deque>

namespace Aurora {
template<class T>
using vec = std::deque<T>;
}
#endif // QT_CORE_LIB
#endif // AURORA_NEURAL_NETWORK
