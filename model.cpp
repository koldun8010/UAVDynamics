#include "model.h"
#include "cmath"
#include "qdebug.h"
#include "qlogging.h"

Model::Model() {
    g = 9.81;
    Eps = 0.7;
    Kv = 0.2;
    Ktheta = 1;
    Kvy = 0.05;
    KH = 0.05;
    Kgamma = 2;
    Kpsi = 1;

    T.push_back(10);
    T.push_back(0.05);
    T.push_back(10);
    T.push_back(1);

    for (int i = 0; i < 4; i++) {
        U.push_back(0);
    }

    for (int i = 0; i < 11; i++) {
        x.push_back(1);
        xDot.push_back(0);
    }
    x[3] = 1;
    x[8] = 1;
    x[5] = 1 * M_PI / 180;
}

void Model::updU()
{
    U[0] = Kv * (Vset - x[3]);
    //U[1] = Ktheta * (Kvy * (KH * (Hset - x[1]) - x[3] * sin(x[4])) - x[4]) + cos(x[4]);
    U[1] = 1.0 + (Kvy * (KH * (Hset - x[1]) - x[3] * sin(x[4])));
    U[2] = 0;
    U[3] = Kgamma * (Kpsi * (x[5] - PSIset) - x[6]);

    //qDebug() << U;
}

void Model::setDesired(double V, double H, double PSI)
{
    Vset = V;
    Hset = H;
    PSIset = PSI;
}

void Model::setCurrentX(std::vector<double> xVec) {
    x = xVec;
    xDot = xVec;
}

std::vector<double> Model::f(std::vector<double> xVec)
{
    xDot[0] = xVec[3] * cos(xVec[5]) * cos(xVec[4]);
    xDot[1] = xVec[3] * sin(xVec[4]);
    xDot[2] = -xVec[3] * sin(xVec[5]) * cos(xVec[4]);
    xDot[3] = g * (xVec[7] - sin(xVec[4]));
    xDot[4] = (g / xVec[3]) * (xVec[8] * cos(xVec[6]) - xVec[10] * sin(xVec[6]) - cos(xVec[4]));
    xDot[5] = (-g / (xVec[3] * cos(xVec[4]))) * (xVec[8] * sin(xVec[6]) + xVec[10] * cos(xVec[6]));
    xDot[6] = (U[3] - xVec[6]) / T[3];
    xDot[7] = (U[0] - xVec[7]) / T[0];
    xDot[8] = xVec[9];
    xDot[9] = (U[1] - xVec[8] - 2 * Eps * T[1] * xVec[9]) / (T[1] * T[1]);
    xDot[10] = (U[2] - xVec[10]) / T[2];

    return xDot;
}

std::vector<double> Model::rungeKutta(double dt)
{
    std::vector<double> k1_x(11), k2_x(11), k3_x(11), k4_x(11);
    std::vector<double> x_temp(11);
    updU();

    f(x);
    for (int i = 0; i < 11; i++) {
        k1_x[i] = dt * xDot[i];
        x_temp[i] = x[i] + 0.5 * k1_x[i];
    }

    f(x_temp);
    for (int i = 0; i < 11; i++) {
        k2_x[i] = dt * xDot[i];
        x_temp[i] = x[i] + 0.5 * k2_x[i];
    }

    f(x_temp);
    for (int i = 0; i < 11; i++) {
        k3_x[i] = dt * xDot[i];
        x_temp[i] = x[i] + k3_x[i];
    }

    f(x_temp);
    for (int i = 0; i < 11; i++) {
        k4_x[i] = dt * xDot[i];
    }

    for (int i = 0; i < 11; i++) {
        x[i] = x[i] + (1.0 / 6.0) * (k1_x[i] + 2 * k2_x[i] + 2 * k3_x[i] + k4_x[i]);
    }

    return x;
}
