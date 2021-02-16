#include "obstacle.h"
#include <src/utils/utils.h>
#include <src/constants/constants.h>
#include <iostream>

Obstacle::Obstacle(){
    _initialAngle = _finalAngle = 0;
    _radius = 0;
}

void Obstacle::calcAnglesFrom(const Position &watcher, float radiusFactor){
    if(_radius == 0){
        std::cout << "Raio = 0";
        return;
    }
    const float R = radiusFactor * _radius;

    // angulação/distancia do obtaculo
    const float dist = Utils::distance(watcher, _position);
    const float fix = atan2(R, dist);

    // calcular o angulo
    float angle = Utils::getAngle(watcher, _position);
    _initialAngle = angle - fix;
    _finalAngle = angle + fix;

    // ter certeza que os angulos calculados estao entre 0-2*M_PI graus
    while(_initialAngle < 0){
        _initialAngle += 2*M_PI;
    }
    while(_initialAngle > 2*M_PI){
        _initialAngle -= 2*M_PI;
    }
    while(_initialAngle < 0){
        _initialAngle += 2*M_PI;
    }
    while(_initialAngle > 2*M_PI){
        _initialAngle -= 2*M_PI;
    }
}
