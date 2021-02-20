#include "freeangles.h"
#include <iostream>
#include <src/utils/utils.h>
#include <src/entities/coach/role/role.h>
#include <src/entities/coach/behavior/behaviors.h>
#include <src/entities/world/worldmap.h>
#include <src/entities/world/world.h>
#include <src/entities/coach/coordinator/coordinator.h>

QList<Obstacle> FreeAngles::getObstacles(const Position watcher, float distanceRadius, QList<Object> players){
    if(players.isEmpty()){
        std::cout << "[FREEANGLES] Sem times";
        return QList<Obstacle>();
    }

    QList<Obstacle> obstacles;
    QList<Object>::const_iterator it;

    for(it = players.constBegin(); it!= players.constEnd(); it++){

        Object player = *it;
        Position posPlayer = player.getPosition();

        // Player == watcher?
        if(watcher.x() == posPlayer.x() && watcher.y() == posPlayer.y())
            continue;

        //verificar o raio
        float distPos = Utils::distance(watcher, posPlayer);
        if(distPos > distanceRadius) continue;

        // criar um obstaculo com as infos
        Obstacle obst;
        Position _posAux = player.getPosition();
        obst._position = _posAux;
        obst._radius = 0.054f;      // Distância do vértice do quadrado (robô) ao centro do robô

        obstacles.push_back(obst);
    }


    return obstacles;
}

QList<FreeAngles::Interval> FreeAngles::getFreeAngles(const Position watcher, const Position initialPos, const Position finalPos, const QList<Obstacle> obstacles, bool returnObstructed) {
    float minPosAngle = Utils::getAngle(watcher, initialPos);
    float maxPosAngle = Utils::getAngle(watcher, finalPos);
    return _getFreeAngles(watcher, minPosAngle, maxPosAngle, obstacles, returnObstructed);
}

QList<FreeAngles::Interval> FreeAngles::_getFreeAngles(const Position watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> obstacles, bool returnObstructed){
    QMap<float, quint8> freeAngles;

    //coloca os angulos recebidos entre 0-2*M_PI
    while(initialPosAngle < 0){
        initialPosAngle += (2*M_PI);
    }
    while(initialPosAngle > (2*M_PI)){
        initialPosAngle -= (2*M_PI);
    }
    while(finalPosAngle < 0){
        finalPosAngle += (2*M_PI);
    }
    while(finalPosAngle > (2*M_PI)){
        finalPosAngle -= (2*M_PI);
    }
//    std::cout << "inital " << initialPosAngle << "\n";
//    std::cout << "endhue " << finalPosAngle << "\n";
    //se init == final
    if(initialPosAngle == finalPosAngle) return QList<Interval>();

    //se Init > final
    if(initialPosAngle > finalPosAngle){
        //perguntar a zilde
        if(finalPosAngle == 0){
            finalPosAngle = 2*M_PI;
            freeAngles.insert(initialPosAngle, 0);
            freeAngles.insert(finalPosAngle, 0);
        } else {
            freeAngles.insert(0, 0);
            freeAngles.insert(finalPosAngle, 0);
            freeAngles.insert(initialPosAngle, 0);
            freeAngles.insert(2*M_PI, 0);
        }
    //se init < final
    } else {
        freeAngles.insert(initialPosAngle, 0);
        freeAngles.insert(finalPosAngle, 0);
    }

    //calcula os angulos de obstrução de cada obstaculo
    QList<Obstacle> tmpObstacles = calcObstaclesObstruction(watcher, obstacles);
//    std::cout << "init " << freeAngles.keys().at(0) << "\n";
//    std::cout << "end " << freeAngles.keys().at(1) << "\n";
    //para cada obstaculo
    for(int i = 0; i < tmpObstacles.size(); i++){
        Obstacle obst = tmpObstacles.at(i);

        //se init > final
        if(obst.initialAngle() > obst.finalAngle()){
            Obstacle newObst1 = obst;
            newObst1._initialAngle = 0;
            newObst1._finalAngle = obst.finalAngle();

            Obstacle newObst2 = obst;
            newObst2._initialAngle = obst.initialAngle();
            newObst2._finalAngle = 2*M_PI;

            obst = newObst1;
            tmpObstacles.push_back(newObst2);
        }

        // adiciona a obstrução no Map freeAngles
        //std::cout << "init " << freeAngles.keys().at(0) << "\n";
        //std::cout << "end " << freeAngles.keys().at(1) << "\n";
        QMap<float, quint8> tmpFreeAngles(freeAngles);
        //std::cout << "initi " << tmpFreeAngles.keys().at(0) << "\n";
        //std::cout << "endi " << tmpFreeAngles.keys().at(1) << "\n";
        int size = tmpFreeAngles.size();
        if(size%2 == 0){
            for(int fa = 0; fa < size; fa += 2){
                const float angI = tmpFreeAngles.keys().at(fa);
                const float angF = tmpFreeAngles.keys().at(fa + 1);
                //printf("Valores dos const %f \t %f\n",angI, angF);
                //obstaculo fora da area de varredura
                if(obst.initialAngle() < angI && obst.finalAngle() <= angI){
                    //std::cout << "caso 1 \n";

                //começo do obstaculo fora e final dentro da area de varredura
                } else if(obst.initialAngle() <= angI && obst.finalAngle() > angI && obst.finalAngle() < angF){
                    freeAngles.remove(angI);
                    freeAngles.insert(obst.finalAngle(), 0);
                //obstaculo 100% dentro da area de varredura
                } else if(obst.initialAngle() > angI && obst.initialAngle() < angF && obst.finalAngle() <= angF){
                    freeAngles.insert(obst.initialAngle(), 0);
                    freeAngles.insert(obst.finalAngle(), 0);
                //começo do obstaculo dentro e final fora da area de varredura
                } else if(obst.initialAngle() > angI && obst.initialAngle() < angF && obst.finalAngle() >= angF){
                    freeAngles.remove(angF);
                    freeAngles.insert(obst.initialAngle(), 0);
                //obstaculo fora da area de varredura
                } else if(obst.initialAngle() >= angF && obst.finalAngle() > angF){
                    //std::cout << "caso 5\n";
                //obstaculo obstruindo 100% da area de varredura
                } else if(obst.initialAngle() <= angI && obst.finalAngle() > angF){
                    freeAngles.remove(angI);
                    freeAngles.remove(angF);
                }
            }
        }
    }

    /*
        i == 3
        i%2 == 1
        obs =

        i == 4
        i
    */

    //criando lista para retornar
    QList<FreeAngles::Interval> retn;
    for(int i = 0; i < freeAngles.size(); i++){
        int nextI = i + 1;
        if(nextI >= freeAngles.size()) nextI = 0;

        float angI = freeAngles.keys().at(i);
        float angF = freeAngles.keys().at(nextI);
        bool obstructed = (i%2) != 0;

        if(!obstructed || (obstructed && returnObstructed)){
            // remove
            if(angI == 2*M_PI && angF == 0.0)
                continue;

            retn.push_back(Interval(angI, angF, obstructed));
        }
    }

    //fazer o merge dos intervalos
    if(!retn.empty()){
        int size = retn.size();
        int i = 0;
        for(int c = 0; c < size+1; c++){
            int nextI = i + 1;
            if(nextI >= retn.size())
                nextI = 0;

            //verificar o merge
            Interval curr = retn.at(i);
            Interval next = retn.at(nextI);
            if((curr.angFinal() == next.angInitial() || (curr.angFinal() - 2*M_PI == next.angInitial())) && curr.obstructed() == next.obstructed()){
                retn.removeAt(i);
                retn.removeAt(nextI);
                retn.insert(i, Interval(curr.angInitial(), next.angFinal(), curr.obstructed()));
            }

            i++;
            if(i >= retn.size())
                i = 0;
        }
    }

    //retorna free angles
    return retn;
}

QList<Obstacle> FreeAngles::calcObstaclesObstruction(const Position &watcher, const QList<Obstacle> &obstacles){
    //calcular o angulo de obstrução
    QList<Obstacle> retnObstacles;
    for(int i = 0; i < obstacles.size(); i++){
        Obstacle obst = obstacles.at(i);

        if(watcher.x() == obst._position.x() && watcher.y() == obst.position().y())
            continue;

        obst.calcAnglesFrom(watcher);
        retnObstacles.push_back(obst);
    }
    return retnObstacles;
}






























