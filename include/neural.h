#ifndef AURORA_NEURAL_NETWORK
#define AURORA_NEURAL_NETWORK

#ifdef QT_CORE_LIBB
#include <QVector>
#include <QHash>
#include <GL/gl.h>
#include <cmath>

namespace Aurora {
template <class T>
using vec = QVector<T>;
template <class K, class V>
using hashMap = QHash<K,V>;
}
#else
#include <deque>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <random>

namespace Aurora {
template<class T>
using vec = std::deque<T>;
template <class K, class V>
using hashMap = std::unordered_map<K,V>;
template <class T>
using hashSet = std::unordered_set<T>;
}
#endif // QT_CORE_LIB
namespace Aurora {

class Neuron;
class NeuralNetwork;

class NeuralNetwork
{
    NeuralNetwork() : uniqueID(0) {}
public:
    Neuron& getNeuron(int id);
    void construct(int inputs, int outs, int levels, int count);
    void connect(int neuronIdSrc, int neuronIdDest, double weight);
protected:
    vec<Neuron> vn;
    vec<int> input;
    vec<int> out;
    vec<vec<int>> lvls;
    int uniqueID;
    bool biasEnabled;
};

class Neuron
{
public:
    Neuron(NeuralNetwork* parent, int _id = 0) :
        p(parent), id(_id), sum(0.0) {}
    void sendSignal();
    void receiveSignal(double val, int senderId);
    void addSource(int srcID, double weight);
    void addDestination(int destID);

protected:
    NeuralNetwork* p;
    hashMap<int,double> in; // input neurons with weights
    hashSet<int> out; // output neurons
    int id;
    double sum; // sum of current inputs
};

} // Aurora

#endif // AURORA_NEURAL_NETWORK
