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

#include "pid.h"

PID::PID(float P, float I, float D) {
    _firstRun = true;
    _errorSum = 0;
    setPID(P, I, D);
    setOutputLimits(0.0);
    setMaxIOutput(0.0);
    _timer.start();
}

PID::PID(std::tuple<float, float, float> pid) {
    _firstRun = true;
    _errorSum = 0;
    setPID(std::get<0>(pid), std::get<1>(pid), std::get<2>(pid));
    setOutputLimits(0.0);
    setMaxIOutput(0.0);
    _timer.start();
}

void PID::setP(float P) {
    _P = P;
}

void PID::setI(float I) {
    _I = I;
}

void PID::setD(float D) {
    _D = D;
}

void PID::setPID(float P, float I, float D) {
    _P = P;
    _I = I;
    _D = D;
}

void PID::setMaxIOutput(float maxIOutput) {
    _maxIOutput = maxIOutput;
}

void PID::setOutputLimits(float outputLimits) {
    _minOutput = -outputLimits;
    _maxOutput = outputLimits;
}

float PID::getOutput(float actual, float setPoint) {
    _timer.stop();

    float error = setPoint - actual;

    float Pout = _P * error;

    _errorSum += error * _timer.getSeconds();
    float Iout = _I * _errorSum;

    if(_firstRun) {
        _lastError = error;
        _lastOutput = Pout;
        _firstRun = false;
    }

    float derivative = (error - _lastError) / _timer.getSeconds();
    float Dout = _D * derivative;

    float output = Pout + Iout + Dout;

    if(_maxOutput != 0) {
        output = clamp(output, _minOutput, _maxOutput);
    }

    _lastError = error;

    _timer.start();

    return output;
}

void PID::reset() {
    _firstRun = true;
    _errorSum = 0;
    _timer.start();
}

float PID::clamp(float value, float min, float max) {
    if(value > max) {
        return max;
    }
    if(value < min) {
        return min;
    }

    return value;
}

bool PID::bounded(float value, float min, float max) {
    return ((min < value) && (value < max));
}
