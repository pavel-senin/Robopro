#ifndef DH_MODEL_H
#define DH_MODEL_H

#include "Eigen/Dense"
#include <cmath>
#include <vector>



class dh_model
{
public:
    dh_model(const std::vector<double>& theta, const std::vector<double>& a, const std::vector<double>& d, const std::vector<double>& alpha);


    std::vector<double> calculateEndPosition();


private:
    std::vector<double> thetas; // Углы сочленений (в градусах)
    std::vector<double> as; // Длины (m)
    std::vector<double> ds; // Смещения (m)
    std::vector<double> alphas; // Углы поворота (рад)

    Eigen::Matrix4d computeTransformationMatrix(double theta, double a, double d, double alpha);

};

#endif // DH_MODEL_H
