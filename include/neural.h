#ifndef AURORA_NEURAL_NETWORK
#define AURORA_NEURAL_NETWORK

//#ifdef QT_CORE_LIB
#include <QDebug>
//#include <QVector>
//#include <QHash>
//#include <GL/gl.h>
//#include <cmath>

//namespace Aurora {
//template <class T>
//using vec = QVector<T>;
//template <class K, class V>
//using hashMap = QHash<K,V>;
//}
//#else
//#include <deque>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <random>

namespace Aurora {
template<class T>
using vec = std::vector<T>;
template <class K, class V>
using hashMap = std::unordered_map<K,V>;
template <class T>
using hashSet = std::unordered_set<T>;
}
//#endif // QT_CORE_LIB
namespace Aurora {

class Neuron;
class NeuralNetwork;

class NeuralNetwork
{
public:
    NeuralNetwork() {initialize();}
    virtual ~NeuralNetwork() {cleanup();}
    NeuralNetwork(const NeuralNetwork & cpy) {clone(cpy);}

    Neuron& getNeuron(int id);
    bool binaryOutput(int index) {return outData[index] > outTreshold;}
    void construct(int inputs, int outs, int levels, int count);
    void connect(int neuronIdSrc, int neuronIdDest, double weight);
    void initialize();
    NeuralNetwork breedS(NeuralNetwork const& sameSpecies);
    void cleanup();
    void clone(const NeuralNetwork& source);
    double getOutput(int index) {return outData[index];}
    void setFitness(double val) {fitness = val;}
    void setInput(int index, double val);
    int size() {return vn.size();}
    void run();

    static bool fitnessCompare(const NeuralNetwork &lhs, const NeuralNetwork &rhs)
    {return lhs.fitness < rhs.fitness;}
    static bool fitnessComparePtr(NeuralNetwork* lhs, NeuralNetwork *rhs)
    {return lhs->fitness < rhs->fitness;}

    NeuralNetwork& operator = (NeuralNetwork& op) {clone(op); return *this;}
protected:
    void mutateWeights(Neuron& n);
protected:
    std::mt19937_64 rng;
    vec<Neuron> vn;
    vec<int> input;
    vec<int> out;
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
    Neuron(const Neuron&) = default;
    void sendSignals();
    void receiveSignal(double val, int senderId);
    void addSource(int srcID, double weight);
    void addDestination(int destID);
    void mergeS(const Neuron& donor);

    void setParent(NeuralNetwork* newParent) {p = newParent;}
    void setInput(double val) {sum = val;}

    double getLastEval() {return lastEval;}

    hashMap<int,double>& rInputData() {return in;}
    const hashMap<int,double>& rInputData() const {return in;}
    friend class NeuralNetwork;
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
