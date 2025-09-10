#ifndef PIPE_H
#define PIPE_H

#include <algorithm> // para min
#include <cmath>     // para sqrt, log10, pow

#define M_PI 3.14159265358979323846
#define RHO 998.0          // kg/m^3
#define MU 1.002e-3       // Pa·s
#define G 9.80665         // m/s^2

class Pipe {

public:
    Pipe(float diameter, float length, float roughness); 

    float getDiameter() const;
    float getLength() const;
    float getRoughness() const;
    float getFlowRate() const;
    float getMaxFlow() const;

    // Retorna vazão (m^3/s) para uma queda de pressão deltaP (Pa)
    double maxFlowForDeltaP(double deltaP,
                            double rho = RHO,          // kg/m^3
                            double mu  = MU,       // Pa·s
                            double g   = G) const;  // m/s^2

    void setFlowRate(float flowRate);

private:
    float flowRate;
    float maxFlow;
    float diameter;
    float length;
    float roughness;
};


#endif // PIPE_H