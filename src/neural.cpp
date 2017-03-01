//#include <algorithm> wait until c++ 17 (std::clamp)
#include "../include/neural.h"
namespace Aurora {

Neuron& NeuralNetwork::getNeuron(int id)
{
    return vn[id];
}

void NeuralNetwork::construct(int inputs, int outs, int levels, int perLevelCount)
{
    cleanup();
    std::uniform_real_distribution<double> urd(0.0, 0.1);
    for (int i = 0; i < inputs; i++) {
        vn.push_back(Neuron(this,uniqueID));
        input.push_back(uniqueID++);
    }
    for (int i = 0; i < outs; i++) {
        vn.push_back(Neuron(this,uniqueID));
        out.push_back(uniqueID++);
    }
    // outData setup
    outData.resize(out.size());
    lvls.resize(levels);
    for (int i = 0; i < levels; i++) {
        for (int j = 0; j < perLevelCount; j++) {
            vn.push_back(Neuron(this, uniqueID));
            lvls[i].push_back(uniqueID);
            if (i == 0) {
                for (int k = 0; k < (int)input.size(); k++) {
                    connect(input[k],uniqueID,urd(rng));
                }
            }
            if (i==levels-1) {
                for (int k = 0; k < (int)out.size(); k++) {
                    connect(uniqueID,out[k],urd(rng));
                }
            }
            if (i > 0 and i < levels-1) {
                for (int k = 0; k < (int)lvls[i-1].size(); k++) {
                    connect(lvls[i-1][k], uniqueID, urd(rng));
                }
            }

            uniqueID++;
        }
    }
    biasID = uniqueID;
    vn.push_back(Neuron(this, uniqueID));
    for (const vec<int> & vLv: lvls)
        for (int i: vLv)
            connect(uniqueID,i,urd(rng));
    for (int i : out)
        connect(uniqueID,i,urd(rng));
    uniqueID++;
    for (Neuron& n: vn) {
        n.in.rehash(n.in.size());
        n.out.rehash(n.out.size());
    }
}

void NeuralNetwork::connect(int idA, int idB, double weight)
{
    vn[idA].addDestination(idB);
    vn[idB].addSource(idA, weight);
}

void NeuralNetwork::initialize()
{
    uniqueID = 0;
    rng = std::mt19937_64((long long)this);
    biasEnabled = true;
    outTreshold = 0.5;
    weightMutationChance = 0.05;
    weightMutationMin = -0.01;
    weightMutationMax = 0.01;
    minWeight = 0.0;
    maxWeight = 1.0;
    fitness = 0.0;
}

NeuralNetwork NeuralNetwork::breedS(const NeuralNetwork &rnn)
{
    NeuralNetwork ret(rnn);
    for (int i = 0; i < (int)ret.vn.size(); i++)
    {
        ret.vn[i].mergeS(vn[i]);
        mutateWeights(ret.vn[i]);
    }
    return ret;
}

void NeuralNetwork::cleanup()
{
    // placeholder
}

void NeuralNetwork::setInput(int index, double val)
{
    vn[input[index]].setInput(val);
    //qDebug() << "Setting input" << index << "to" << val;
}

void NeuralNetwork::clone(const NeuralNetwork &source)
{
    rng = std::mt19937_64((long long)this);
    vn = source.vn;
    for (Neuron& rn: vn)
        rn.setParent(this);
    input =                 source.input;
    out =                   source.out;
    outData =               source.outData;
    lvls =                  source.lvls;
    uniqueID =              source.uniqueID;
    biasID =                source.biasID;
    biasEnabled =           source.biasEnabled;
    outTreshold =           source.outTreshold;
    weightMutationChance =  source.weightMutationChance;
    weightMutationMin =     source.weightMutationMin;
    weightMutationMax =     source.weightMutationMax;
    minWeight =             source.minWeight;
    maxWeight =             source.maxWeight;
    fitness =               source.fitness;
}

void NeuralNetwork::run()
{
    if (biasEnabled) {
        vn[biasID].sum = 1.0;
        vn[biasID].sendSignals();
    }
    for (int i: input)
        vn[i].sendSignals();
    for (vec<int>& vTop: lvls)
        for (int j: vTop)
            vn[j].sendSignals();
    for (int i: out)
        vn[i].sendSignals();
    // generate outputs
    for (int i = 0; i < (int)out.size(); i++)
        outData[i] = vn[out[i]].getLastEval();
}

void NeuralNetwork::mutateWeights(Neuron &n)
{
    std::uniform_real_distribution<double> chance(0.0, 1.0);
    std::uniform_real_distribution<double> dWeight(weightMutationMin, weightMutationMax);
    hashMap<int,double>& rawData = n.rInputData();
    auto it = rawData.begin();
    while (it != rawData.end()) {
        if (chance(rng) < weightMutationChance) {
            double dW = dWeight(rng);
            it->second += dW;
            //it->second = std::clamp(it->second, minWeight, maxWeight); // wait until c++17
            it->second = (it->second > maxWeight)?maxWeight:((it->second<minWeight)?minWeight:it->second);
            //
        }
        ++it;
    }
}

/// Neuron::

void Neuron::sendSignals()
{
    double eval = tanh(sum);
//    qDebug() << eval;
//    if (eval == 0)
//        qDebug() << id << in.size();
    lastEval = eval;
    for (int dst: out) {
        p->getNeuron(dst).receiveSignal(eval, id);
    }
    sum = 0.0f;
}

void Neuron::receiveSignal(double val, int senderId) {
    sum += val*in[senderId];
//    if (val != 0.0)
//        qDebug() << "Received" << val << "from" << senderId;
}

void Neuron::addSource(int srcID, double weight)  {
    in.insert({srcID, weight});
}

void Neuron::addDestination(int destID) {
    out.insert(destID);
}

void Neuron::mergeS(const Neuron& donor)
{
    auto it = in.begin();
    while (it != in.end()) {
        auto it2 = donor.in.find(it->first);
        if (it2 != donor.in.end())
            it->second = (it->second + it2->second)/2.0;
        ++it;
    }
}

}
