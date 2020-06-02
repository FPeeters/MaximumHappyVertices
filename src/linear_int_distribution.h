#ifndef MAXHAPPYVERTS_LINEAR_INT_DISTRIBUTION_H
#define MAXHAPPYVERTS_LINEAR_INT_DISTRIBUTION_H

#include <random>

template<class IntType = int>
class linear_int_distribution {

public:
    explicit linear_int_distribution(double alpha = 0, IntType min = 0, IntType max = 1) : alpha(alpha),
                                                                                           min(min),
                                                                                           max(max) {
        std::uniform_int_distribution<IntType> typeCheck(0, 1);
        if (alpha > 2)
            this->alpha = 2;
        else if (alpha < -2)
            this->alpha = -2;
        this->min = min;
    }

    template<class Engine>
    IntType operator()(Engine &engine);

private:
    double alpha;
    IntType min;
    IntType max;
};

template<class Engine>
double linear_distribution(double alpha, Engine &engine) {
    std::uniform_real_distribution<double> distribution(0, 1);
    return ((sqrt(alpha * alpha - 2. * alpha + 4. * alpha * distribution(engine) + 1.) - 1.) / alpha);
}


template<class IntType>
template<class Engine>
IntType linear_int_distribution<IntType>::operator()(Engine &engine) {
    if (this->alpha == 0) {
        std::uniform_int_distribution<IntType> distribution(this->min, this->max - 1);
        return distribution(engine);
    }

    double value;
    if (this->alpha > 1)
        value = ((linear_distribution(1, engine) + 1) * (2 - alpha) + 2 * alpha - 2) / 2.;
    else if (this->alpha < -1)
        value = (linear_distribution(-1, engine) + 1) * (alpha + 2) / 2.;
    else
        value = (linear_distribution(alpha, engine) + 1) / 2.;

    IntType result = static_cast<IntType>(this->min + round(value * (this->max - this->min) - 0.5));
    if (result == this->max)
        result = this->max - 1;
    if (result == this->min - 1)
        result = this->min;

    return result;
}

#endif //MAXHAPPYVERTS_LINEAR_INT_DISTRIBUTION_H
