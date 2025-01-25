#ifndef MODEL_H
#define MODEL_H

#include "vector"

class Model
{
public:
    Model();
    void setDesired(double V, double H, double PSI);
    void setCurrentX(std::vector<double> xVec);
    std::vector<double> rungeKutta(double dt);

private:
    void updU();
    std::vector<double> f(std::vector<double> xVec);

private:
    double Vset;
    double Hset;
    double PSIset;
    std::vector<double> x;
    std::vector<double> xDot;
    std::vector<double> U;
    std::vector<double> T;

private:
    double g;
    double Eps;
    double Kv;
    double Ktheta;
    double Kvy;
    double KH;
    double Kgamma;
    double Kpsi;
};

#endif // MODEL_H
