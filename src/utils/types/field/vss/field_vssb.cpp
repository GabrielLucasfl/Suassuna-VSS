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

#include "field_vssb.h"

Field_VSSB::Field_VSSB() {

}

QString Field_VSSB::name() const {
    return "Field_VSSB";
}

float Field_VSSB::length() const {
    return 1.50f;
}

float Field_VSSB::width() const {
    return 1.30f;
}

float Field_VSSB::goalWidth() const {
    return 0.40f;
}

float Field_VSSB::goalDepth() const {
    return 0.10f;
}

float Field_VSSB::centerRadius() const {
    return 0.20f;
}

float Field_VSSB::defenseRadius() const {
    return 0.15f;
}

float Field_VSSB::defenseStretch() const {
    return 0.7f;
}
