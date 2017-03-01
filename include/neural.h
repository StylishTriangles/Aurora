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
public:
    NeuralNetwork() {initialize();}
    ~NeuralNetwork() {cleanup();}
    NeuralNetwork(const NeuralNetwork & cpy) {clone(cpy);}

    Neuron& getNeuron(int id);
    void construct(int inputs, int outs, int levels, int count);
    void connect(int neuronIdSrc, int neuronIdDest, double weight);
    void initialize();
    NeuralNetwork breedS(NeuralNetwork const& sameSpecies);
    void cleanup();
    void clone(const NeuralNetwork& source);
    int size() {return vn.size();}
    void run();

    void setInput(int index, double val) {inData[index] = val;}
protected:
    void mutateWeights(Neuron& n);
protected:
    std::mt19937_64 rng;
    vec<Neuron> vn;
    vec<int> input;
    vec<int> out;
    vec<double> inData;
    vec<double> outData;
    vec<vec<int>> lvls;
    int uniqueID;
    int biasID;
    bool biasEnabled;
    double outTreshold;
    double weightMutationChance;
    double weightMutationMin;
    double weightMutationMax;
    double minWeight;
    double maxWeight;
    double fitness;
};

class Neuron
{
public:
    Neuron(NeuralNetwork* parent, int _id = 0) :
        p(parent), id(_id), sum(0.0), lastEval(0.0) {}
    void sendSignals();
    void receiveSignal(double val, int senderId);
    void addSource(int srcID, double weight);
    void addDestination(int destID);
    void mergeS(const Neuron& donor);

    void setParent(NeuralNetwork* newParent) {p = newParent;}

    double getLastEval() {return lastEval;}

    hashMap<int,double>& rInputData() {return in;}
    const hashMap<int,double>& rInputData() const {return in;}
protected:
    NeuralNetwork* p;
    hashMap<int,double> in; // input neurons with weights
    hashSet<int> out; // output neurons
    int id;
    double sum; // sum of current inputs
    double lastEval;
};

} // Aurora

#endif // AURORA_NEURAL_NETWORK
