/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#ifndef PID_H
#define PID_H

#include <src/utils/timer/timer.h>
#include <tuple>

class PID
{
public:
    PID(float P, float I, float D);
    PID(std::tuple<float, float, float> pid);

    // Constant setters
    void setP(float P);
    void setI(float I);
    void setD(float D);
    void setPID(float P, float I, float D);

    // Output setters
    void setMaxIOutput(float maxIOutput);
    void setOutputLimits(float outputLimits);

    // Getters
    float getOutput(float actual, float setPoint);

    // Reset
    void reset();

private:
    // Utils functions
    float clamp(float value, float min, float max);
    bool bounded(float value, float min, float max);

    // Constants
    float _P;
    float _I;
    float _D;

    // Output constants
    float _maxIOutput;
    float _minOutput;
    float _maxOutput;

    // Internal control
    bool _firstRun;
    float _errorSum;
    float _lastError;
    float _lastOutput;
    Timer _timer;
};

#endif // PID_H
