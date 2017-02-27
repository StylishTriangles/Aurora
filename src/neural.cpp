#include "../include/neural.h"
namespace Aurora {

Neuron& NeuralNetwork::getNeuron(int id)
{
    return vn[id];
}

void NeuralNetwork::construct(int inputs, int outs, int levels, int perLevelCount)
{
    std::mt19937_64 rng((long long)this);
    std::uniform_real_distribution<double> urd(0.0, 1.0);
    for (int i = 0; i < inputs; i++) {
        vn.push_back(Neuron(this,uniqueID++));
        input.push_back(i);
    }
    for (int i = inputs; i < inputs+outs; i++) {
        vn.push_back(Neuron(this,uniqueID++));
        out.push_back(i);
    }
    lvls.resize(levels);
    for (int i = 1; i <= levels; i++) {
        for (int j = 0; j < perLevelCount; j++) {
            vn.push_back(Neuron(this, uniqueID));
            lvls[i].push_back(uniqueID);
            if (i == 1) {
                for (int k = 0; k < (int)input.size(); k++) {
                    connect(input[k],uniqueID,urd(rng));
                }
            }
            if (i==levels) {
                for (int k = 0; k < (int)out.size(); k++) {
                    connect(uniqueID,out[k],urd(rng));
                }
            }
            if (i > 1 and i < levels) {
                for (int k = 0; k < (int)lvls[i-1].size(); k++) {
                    connect(lvls[i-1][k], uniqueID, urd(rng));
                }
            }

            uniqueID++;
        }
    }
}

void NeuralNetwork::connect(int idA, int idB, double weight)
{
    vn[idA].addDestination(idB);
    vn[idB].addSource(idA, weight);
}

void Neuron::sendSignal()
{
    double eval = 1.0/(1.0+exp(-sum));
    for (int dst: out) {
        p->getNeuron(dst).receiveSignal(eval, id);
    }
    sum = 0.0f;
}

void Neuron::receiveSignal(double val, int senderId) {
    sum += val*in[senderId];
}

void Neuron::addSource(int srcID, double weight)  {
    in.insert({srcID, weight});
}

void Neuron::addDestination(int destID) {
    out.insert(destID);
}

}
