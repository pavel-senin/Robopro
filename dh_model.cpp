#include "dh_model.h"

dh_model::dh_model(const std::vector<double>& theta, const std::vector<double>& a,
                   const std::vector<double>& d, const std::vector<double>& alpha)
                    : thetas(theta), as(a), ds(d), alphas(alpha) {}


std::vector<double> dh_model::calculateEndPosition(){
    Eigen::Matrix4d T = Eigen::Matrix4d::Identity();

    for (size_t i = 0; i < thetas.size(); ++i) {

        // Преобразуем theta из градусов в радианы
        double theta_rad = thetas[i] * M_PI / 180.0;
        double alpha_rad = alphas[i];

        // Вычисляем матрицу трансформации для текущего сочленения
        Eigen::Matrix4d A = computeTransformationMatrix(theta_rad, as[i], ds[i], alpha_rad);
        T *= A; // Умножаем текущую матрицу на общую
    }


    std::vector<double> res;
    res.push_back(T(0, 3));
    res.push_back(T(1, 3));
    res.push_back(T(2, 3));
    return res;
}


// Zi * Xi
Eigen::Matrix4d dh_model::computeTransformationMatrix(double theta, double a, double d, double alpha) {
    // Матрица Z (вращение вокруг Z и смещение вдоль Z)
    Eigen::Matrix4d Z;
    Z << cos(theta), -sin(theta), 0, 0,
        sin(theta),  cos(theta), 0, 0,
        0,           0,          1, d,
        0,           0,          0, 1;

    // Матрица X (вращение вокруг X и смещение вдоль X)
    Eigen::Matrix4d X;
    X << 1, 0,          0,           a,
        0, cos(alpha), -sin(alpha), 0,
        0, sin(alpha),  cos(alpha), 0,
        0, 0,          0,           1;

    // Общая трансформация: T = Z * X
    Eigen::Matrix4d A = Z * X;

    return A;
}
