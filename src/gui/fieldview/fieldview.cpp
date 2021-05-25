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

#include "fieldview.h"
#define FIELD_COLOR 0.25,0.25,0.25,1.0
#define FIELD_LINES_COLOR 1.0,1.0,1.0,1.0
#pragma GCC diagnostic ignored "-Wunused-parameter"

FieldView::FieldView(QWidget *parent) : QOpenGLWidget(parent) {
    // 4x MSAA for antialiasing
    QSurfaceFormat fmt;
    fmt.setSamples(4);
    this->setFormat(fmt);

    // Setup glText
    QFont robotIDFont = this->font();
    robotIDFont.setWeight(QFont::Bold);
    robotIDFont.setPointSize(80);
    glText = GLText(robotIDFont);

    // CheckBox enablers starters
    _supporterShowEnabled = false;

    // Checkbox parameters starters
    _supporterDesiredPosition.setPosition(false, 0.0, 0.0);
}

void FieldView::setConstantsAndWorldMap(Constants *constants, WorldMap *worldMap) {
    // Taking pointers
    _constants = constants;
    _worldMap = worldMap;

    // Updating default geometry
    updateDefaultGeometry();
}

void FieldView::updateDefaultGeometry() {
    /// TODO: take these values using loc
    //Locations *loc = getWorldMap()->getLocations();

    // Load default data from locations
    const double kFieldLength = 1500.0;
    const double kFieldWidth = 1300.0;
    const double kGoalWidth = 400.0;
    const double kGoalDepth = 100.0;
    //const double kBoundaryWidth = 300.0;

    const double kCenterRadius = 200.0;
    const double kDefenseRadius = 150.0;
    const double kDefenseStretch = 700.0;
    const double kLineThickness = 3.0;
    const double kXMax = kFieldLength/2;
    const double kXMin = -kXMax;
    const double kYMax = kFieldWidth/2;
    const double kYMin = -kYMax;
    const double kFieldCorner = kFieldLength*5/150;
    const double kMarkDistanceX = kFieldLength/4;
    const double kMarkDistanceY = 4*kFieldLength/15;
    const double kMarkLength = kFieldLength*5/150;
    const double kMarkCircleDistance = 2*kFieldLength/15;
    const double kMarkCircleRadius = kMarkLength/10;

    // Clear old field objects
    _fieldLines.clear();
    _fieldTriangles.clear();
    _fieldCircularArcs.clear();

    // Fill field objects
    // Field lines
    _fieldLines.append(FieldLine("LeftGoalStretch", QVector2D(kXMin-kGoalDepth, -kGoalWidth/2), QVector2D(kXMin-kGoalDepth, kGoalWidth/2), kLineThickness));
    _fieldLines.append(FieldLine("LeftGoalLeftLine", QVector2D(kXMin, kGoalWidth/2), QVector2D(kXMin-kGoalDepth-kLineThickness/2, kGoalWidth/2), kLineThickness));
    _fieldLines.append(FieldLine("LeftGoalRightLine", QVector2D(kXMin, -kGoalWidth/2), QVector2D(kXMin-kGoalDepth-kLineThickness/2, -kGoalWidth/2), kLineThickness));
    _fieldLines.append(FieldLine("RightGoalStretch", QVector2D(kXMax+kGoalDepth, -kGoalWidth/2), QVector2D(kXMax+kGoalDepth, kGoalWidth/2), kLineThickness));
    _fieldLines.append(FieldLine("RightGoalLeftLine", QVector2D(kXMax, kGoalWidth/2), QVector2D(kXMax+kGoalDepth+kLineThickness/2, kGoalWidth/2), kLineThickness));
    _fieldLines.append(FieldLine("RightGoalRightLine", QVector2D(kXMax, -kGoalWidth/2), QVector2D(kXMax+kGoalDepth+kLineThickness/2, -kGoalWidth/2), kLineThickness));
    _fieldLines.append(FieldLine("LeftGoalLine", QVector2D(kXMin, kYMin), QVector2D(kXMin, kYMax), kLineThickness));
    _fieldLines.append(FieldLine("RightGoalLine", QVector2D(kXMax, kYMin), QVector2D(kXMax, kYMax), kLineThickness));
    _fieldLines.append(FieldLine("LeftFieldLeftDefenseLine", QVector2D(kXMin, kDefenseStretch/2), QVector2D(kXMin+kDefenseRadius+kLineThickness/2, kDefenseStretch/2), kLineThickness));
    _fieldLines.append(FieldLine("LeftFieldRightDefenseLine", QVector2D(kXMin, -kDefenseStretch/2), QVector2D(kXMin+kDefenseRadius+kLineThickness/2, -kDefenseStretch/2), kLineThickness));
    _fieldLines.append(FieldLine("RightFieldLeftDefenseLine", QVector2D(kXMax, kDefenseStretch/2), QVector2D(kXMax-kDefenseRadius-kLineThickness/2, kDefenseStretch/2), kLineThickness));
    _fieldLines.append(FieldLine("RightFieldRightDefenseLine", QVector2D(kXMax, -kDefenseStretch/2), QVector2D(kXMax-kDefenseRadius-kLineThickness/2, -kDefenseStretch/2), kLineThickness));
    _fieldLines.append(FieldLine("TopTouchLine", QVector2D(kXMin-kLineThickness/2, kYMax), QVector2D(kXMax+kLineThickness/2, kYMax), kLineThickness));
    _fieldLines.append(FieldLine("BottomTouchLine", QVector2D(kXMin-kLineThickness/2, kYMin), QVector2D(kXMax+kLineThickness/2, kYMin), kLineThickness));
    _fieldLines.append(FieldLine("HalfwayLine", QVector2D(0, kYMin), QVector2D(0, kYMax), kLineThickness));
    _fieldLines.append(FieldLine("LeftPenaltyStretch", QVector2D(kXMin+kDefenseRadius, -kDefenseStretch/2), QVector2D(kXMin+kDefenseRadius, kDefenseStretch/2), kLineThickness));
    _fieldLines.append(FieldLine("RightPenaltyStretch", QVector2D(kXMax-kDefenseRadius, -kDefenseStretch/2), QVector2D(kXMax-kDefenseRadius, kDefenseStretch/2), kLineThickness));
    _fieldLines.append(FieldLine("FBRightTopH", QVector2D(kMarkDistanceX-kMarkLength/2, kMarkDistanceY), QVector2D(kMarkDistanceX+kMarkLength/2, kMarkDistanceY), kLineThickness));
    _fieldLines.append(FieldLine("FBRightTopV", QVector2D(kMarkDistanceX, kMarkDistanceY-kMarkLength/2), QVector2D(kMarkDistanceX, kMarkDistanceY+kMarkLength/2), kLineThickness));
    _fieldLines.append(FieldLine("FBRightBottomH", QVector2D(kMarkDistanceX-kMarkLength/2, -kMarkDistanceY), QVector2D(kMarkDistanceX+kMarkLength/2, -kMarkDistanceY), kLineThickness));
    _fieldLines.append(FieldLine("FBRightBottomV", QVector2D(kMarkDistanceX, -kMarkDistanceY-kMarkLength/2), QVector2D(kMarkDistanceX, -kMarkDistanceY+kMarkLength/2), kLineThickness));
    _fieldLines.append(FieldLine("FBRightTopH", QVector2D(-kMarkDistanceX-kMarkLength/2, kMarkDistanceY), QVector2D(-kMarkDistanceX+kMarkLength/2, kMarkDistanceY), kLineThickness));
    _fieldLines.append(FieldLine("FBRightTopV", QVector2D(-kMarkDistanceX, kMarkDistanceY-kMarkLength/2), QVector2D(-kMarkDistanceX, kMarkDistanceY+kMarkLength/2), kLineThickness));
    _fieldLines.append(FieldLine("FBLeftBottomH", QVector2D(-kMarkDistanceX-kMarkLength/2, -kMarkDistanceY), QVector2D(-kMarkDistanceX+kMarkLength/2, -kMarkDistanceY), kLineThickness));
    _fieldLines.append(FieldLine("FBLeftBottomV", QVector2D(-kMarkDistanceX, -kMarkDistanceY-kMarkLength/2), QVector2D(-kMarkDistanceX, -kMarkDistanceY+kMarkLength/2), kLineThickness));
    _fieldLines.append(FieldLine("PKRightH", QVector2D(kMarkDistanceX-kMarkLength/2, 0), QVector2D(kMarkDistanceX+kMarkLength/2, 0), kLineThickness));
    _fieldLines.append(FieldLine("PKRightV", QVector2D(kMarkDistanceX, -kMarkLength/2), QVector2D(kMarkDistanceX, kMarkLength/2), kLineThickness));
    _fieldLines.append(FieldLine("PKLeftH", QVector2D(-kMarkDistanceX-kMarkLength/2, 0), QVector2D(-kMarkDistanceX+kMarkLength/2, 0), kLineThickness));
    _fieldLines.append(FieldLine("PKLeftV", QVector2D(-kMarkDistanceX, -kMarkLength/2), QVector2D(-kMarkDistanceX, kMarkLength/2), kLineThickness));

    // Center circle
    _fieldCircularArcs.append(FieldCircularArc("CenterCircle", QVector2D(0, 0), kCenterRadius-kLineThickness/2, 0, 2*M_PI, kLineThickness));
    _fieldCircularArcs.append(FieldCircularArc("FBRightTopLeft", QVector2D(kMarkDistanceX-kMarkCircleDistance, kMarkDistanceY), kMarkCircleRadius, 0, 2*M_PI, 2*kMarkCircleRadius));
    _fieldCircularArcs.append(FieldCircularArc("FBRightTopRight", QVector2D(kMarkDistanceX+kMarkCircleDistance, kMarkDistanceY), kMarkCircleRadius, 0, 2*M_PI, 2*kMarkCircleRadius));
    _fieldCircularArcs.append(FieldCircularArc("FBRightBottomLeft", QVector2D(kMarkDistanceX-kMarkCircleDistance, -kMarkDistanceY), kMarkCircleRadius, 0, 2*M_PI, 2*kMarkCircleRadius));
    _fieldCircularArcs.append(FieldCircularArc("FBRightBottomRight", QVector2D(kMarkDistanceX+kMarkCircleDistance, -kMarkDistanceY), kMarkCircleRadius, 0, 2*M_PI, 2*kMarkCircleRadius));
    _fieldCircularArcs.append(FieldCircularArc("FBLeftTopLeft", QVector2D(-kMarkDistanceX-kMarkCircleDistance, kMarkDistanceY), kMarkCircleRadius, 0, 2*M_PI, 2*kMarkCircleRadius));
    _fieldCircularArcs.append(FieldCircularArc("FBLeftTopRight", QVector2D(-kMarkDistanceX+kMarkCircleDistance, kMarkDistanceY), kMarkCircleRadius, 0, 2*M_PI, 2*kMarkCircleRadius));
    _fieldCircularArcs.append(FieldCircularArc("FBLeftBottomLeft", QVector2D(-kMarkDistanceX-kMarkCircleDistance, -kMarkDistanceY), kMarkCircleRadius, 0, 2*M_PI, 2*kMarkCircleRadius));
    _fieldCircularArcs.append(FieldCircularArc("FBLeftBottomRight", QVector2D(-kMarkDistanceX+kMarkCircleDistance, -kMarkDistanceY), kMarkCircleRadius, 0, 2*M_PI, 2*kMarkCircleRadius));
    _fieldCircularArcs.append(FieldCircularArc("GoalRightArc", QVector2D(kXMax-kDefenseRadius/2, 0), kDefenseRadius/2+50, M_PI-0.9272, M_PI+0.9272, kLineThickness));
    _fieldCircularArcs.append(FieldCircularArc("GoalLeftArc", QVector2D(kXMin+kDefenseRadius/2, 0), kDefenseRadius/2+50, -0.9272, +0.9272, kLineThickness));

    // Triangles
    _fieldTriangles.append(FieldTriangle("RightTop", QVector2D(kXMax-kFieldCorner, kYMax), QVector2D(kXMax, kYMax-kFieldCorner), QVector2D(kXMax, kYMax), kLineThickness));
    _fieldTriangles.append(FieldTriangle("RightBottom", QVector2D(kXMax-kFieldCorner, -kYMax), QVector2D(kXMax, -kYMax+kFieldCorner), QVector2D(kXMax, -kYMax), kLineThickness));
    _fieldTriangles.append(FieldTriangle("LeftTop", QVector2D(-kXMax+kFieldCorner, kYMax), QVector2D(-kXMax, kYMax-kFieldCorner), QVector2D(-kXMax, kYMax), kLineThickness));
    _fieldTriangles.append(FieldTriangle("LeftBottom", QVector2D(-kXMax+kFieldCorner, -kYMax), QVector2D(-kXMax, -kYMax+kFieldCorner), QVector2D(-kXMax, -kYMax), kLineThickness));
}

void FieldView::paintGL() {
    _graphicsMutex.lock();

    glClearColor(FIELD_COLOR);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();

    drawFieldLines();
    drawBall();
    drawRobots();

    if (_supporterShowEnabled) {
        showSupporterPosition(_supporterDesiredPosition);
    }

    glPopMatrix();
    _graphicsMutex.unlock();
}

void FieldView::mousePressEvent(QMouseEvent* event) {
    leftButton = event->buttons().testFlag(Qt::LeftButton);
    midButton = event->buttons().testFlag(Qt::MidButton);
    rightButton = event->buttons().testFlag(Qt::RightButton);

    if(leftButton) {
        setCursor(Qt::ClosedHandCursor);
    }
    if(midButton) {
        setCursor(Qt::SizeVerCursor);
    }
    if(leftButton || midButton) {
        mouseStartX = event->x();
        mouseStartY = event->y();
    }
}

void FieldView::mouseReleaseEvent(QMouseEvent* event) {
    setCursor(Qt::ArrowCursor);
}

void FieldView::mouseMoveEvent(QMouseEvent* event) {
    bool leftButton = event->buttons().testFlag(Qt::LeftButton);
    bool midButton = event->buttons().testFlag(Qt::MidButton);

    if(leftButton) {
        viewXOffset -= viewScale*double(event->x() - mouseStartX);
        viewYOffset += viewScale*double(event->y() - mouseStartY);
        mouseStartX = event->x();
        mouseStartY = event->y();
        recomputeProjection();
    }
    else if(midButton) {
        float zoomRatio = static_cast<float>(event->y() - mouseStartY)/500.0;
        viewScale = viewScale*(1.0+zoomRatio);
        recomputeProjection();
        mouseStartX = event->x();
        mouseStartY = event->y();
    }
}

void FieldView::wheelEvent(QWheelEvent* event) {
    float zoomRatio = -static_cast<float>(event->delta())/1000.0;
    viewScale = viewScale*(1.0+zoomRatio);
    recomputeProjection();
}

void FieldView::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
    resetView();
}

void FieldView::drawQuad(QVector2D v1, QVector2D v2, float z) {
    glBegin(GL_QUADS);
    glVertex3d(v1.x(), v1.y(), z);
    glVertex3d(v2.x(), v1.y(), z);
    glVertex3d(v2.x(), v2.y(), z);
    glVertex3d(v1.x(), v2.y(), z);
    glEnd();
}

void FieldView::drawQuad(QVector2D v1, QVector2D v2, QVector2D v3, QVector2D v4, float z) {
    glBegin(GL_QUADS);
    glVertex3d(v1.x(), v1.y(), z);
    glVertex3d(v2.x(), v2.y(), z);
    glVertex3d(v3.x(), v3.y(), z);
    glVertex3d(v4.x(), v4.y(), z);
    glEnd();
}

void FieldView::drawArc(QVector2D center, float r1, float r2, float theta1, float theta2, float z, float dTheta) {
    static const float tesselation = 1.0;

    if(dTheta < 0) {
        dTheta = tesselation/r2;
    }

    glBegin(GL_QUAD_STRIP);
    for(float theta = theta1; theta < theta2; theta += dTheta) {
        float c1 = cos(theta);
        float s1 = sin(theta);
        glVertex3d(r2*c1 + center.x(), r2*s1 + center.y(), z);
        glVertex3d(r1*c1 + center.x(), r1*s1 + center.y(), z);
    }

    float c1 = cos(theta2);
    float s1 = sin(theta2);
    glVertex3d(r2*c1 + center.x(), r2*s1 + center.y(), z);
    glVertex3d(r1*c1 + center.x(), r1*s1 + center.y(), z);
    glEnd();
}

void FieldView::drawTriangle(QVector2D v1, QVector2D v2, QVector2D v3, float z) {
    glBegin(GL_TRIANGLES);
    glVertex3d(v1.x(), v1.y(), z);
    glVertex3d(v2.x(), v2.y(), z);
    glVertex3d(v3.x(), v3.y(), z);
    glEnd();
}

void FieldView::resetView() {
    viewScale = (getWorldMap()->getLocations()->fieldLength()*1000.0 + 500.0) / width();
    viewScale = std::max(viewScale, (getWorldMap()->getLocations()->fieldWidth()*1000.0 + 500.0) / height());

    viewXOffset = viewYOffset = 0.0;
    recomputeProjection();
}

void FieldView::recomputeProjection() {
    makeCurrent();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5*viewScale*width()+viewXOffset, 0.5*viewScale*width()+viewXOffset, -0.5*viewScale*height()+viewYOffset, 0.5*viewScale*height()+viewYOffset, minZValue, maxZValue);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void FieldView::updateFieldObjects() {
    // Update teams
    for(int i = Colors::Color::BLUE; i <= Colors::Color::YELLOW; i++) {
        // Taking old list (default destructor will handle that)
        QList<quint8> oldObjects = _players.take(Colors::Color(i));

        // Pushing new list
        QList<quint8> objects = getWorldMap()->getAvailablePlayers(Colors::Color(i));
        _players.insert(Colors::Color(i), objects);
    }

    // Update ball
    _ball = getWorldMap()->getBall();

    // Cast updateDefaultGeometry
    updateDefaultGeometry();
}

void FieldView::drawFieldLines() {
    glColor4f(FIELD_LINES_COLOR);

    for (int i = 0; i < _fieldLines.size(); ++i) {
        const FieldLine& line = _fieldLines[i];
        const float half_thickness = 0.5 * line.lineThickness();
        const QVector2D p1 = line.p1();
        const QVector2D p2 = line.p2();

        QVector2D perp = (p2 - p1);
        float square = sqrt(pow(perp.x(), 2) + pow(perp.y(), 2));
        perp = QVector2D(perp.x() / square, perp.y() / square);
        perp = QVector2D(-perp.y(), perp.x());

        const QVector2D v1 = p1 - half_thickness * perp;
        const QVector2D v2 = p1 + half_thickness * perp;
        const QVector2D v3 = p2 + half_thickness * perp;
        const QVector2D v4 = p2 - half_thickness * perp;
        drawQuad(v1, v2, v3, v4, fieldZ);
    }

    for (int i = 0; i < _fieldCircularArcs.size(); ++i) {
        const FieldCircularArc& arc = _fieldCircularArcs[i];
        const float half_thickness = 0.5 * arc.lineThickness();
        const float radius = arc.radius();
        const QVector2D center = arc.center();
        const float a1 = arc.a1();
        const float a2 = arc.a2();
        drawArc(center, radius - half_thickness, radius + half_thickness, a1, a2, fieldZ);
    }

    for(int i = 0; i < _fieldTriangles.size(); i++) {
        const FieldTriangle& triangle = _fieldTriangles[i];
        const QVector2D v1 = triangle.p1();
        const QVector2D v2 = triangle.p2();
        const QVector2D v3 = triangle.p3();
        drawTriangle(v1, v2, v3, fieldZ);
    }
}

void FieldView::drawBall() {
    // Draw ball if its position is valid
    if(!_ball.getPosition().isInvalid()) {
        glColor3d(1.0,0.5059,0.0);
        drawArc(QVector2D(_ball.getPosition().x() * 1000.0, _ball.getPosition().y()*1000.0),
                -18.5, 18.5, -M_PI, M_PI, ballZ);
        glColor3d(0.8706,0.3490,0.0);
        drawArc(QVector2D(_ball.getPosition().x() * 1000.0, _ball.getPosition().y()*1000.0),
                18.5, 21.5, -M_PI, M_PI, ballZ);
    }
}

void FieldView::drawRobots() {
    for(int i = Colors::Color::BLUE; i <= Colors::Color::YELLOW; i++) {
        // Take objects list
        QList<quint8> teamRobots = _players.value(Colors::Color(i));

        for(int j = 0; j < teamRobots.size(); j++) {
            Object playerObject = getWorldMap()->getPlayer(Colors::Color(i), teamRobots.at(j));
            Position playerPosition = Position(true, playerObject.getPosition().x() * 1000.0, playerObject.getPosition().y() * 1000.0);

            glPushMatrix();
            glLoadIdentity();
            glTranslated(playerPosition.x(), playerPosition.y(), 0);
            glRotated(Angle::toDegrees(playerObject.getOrientation()), 0, 0, 1.0);

            // Color for triangle (robot front)
            if(i == Colors::Color::YELLOW) {
                glColor3d(1.0, 0.9529, 0.2431);
            }
            else if(i == Colors::Color::BLUE) {
                glColor3d(0.2549, 0.4941, 1.0);
            }

            // Drawing robot border
            drawQuad(QVector2D(-35, 35), QVector2D(35, -35), robotZ);

            // Color for robot body
            if(i == Colors::Color::YELLOW) {
                glColor3d(0.8, 0.6157, 0.0);
            }
            else if(i == Colors::Color::BLUE) {
                glColor3d(0.0706, 0.2314, 0.6275);
            }

            // Draw robot body
            drawQuad(QVector2D(-40, 40), QVector2D(40, 35), robotZ+0.01);
            drawQuad(QVector2D(-40, -40), QVector2D(40, -35), robotZ+0.01);
            drawQuad(QVector2D(40, -40), QVector2D(35, 40), robotZ+0.01);
            drawQuad(QVector2D(-40, -40), QVector2D(-35, 40), robotZ+0.01);

            glColor3d(0.5, 0.5, 0.5);

            drawQuad(QVector2D(-20, 47), QVector2D(20, 40), robotZ+0.02);
            drawQuad(QVector2D(-20,-47), QVector2D(20, -40), robotZ+0.02);

            // Draw robot id
            glColor3d(0.0, 0.0, 0.0);
            QString playerId = QString("%1").arg(teamRobots.at(j));
            glText.drawString(QVector2D(playerPosition.x(), playerPosition.y()), 0.0, 50, playerId.toStdString().c_str(), GLText::CenterAligned, GLText::MiddleAligned);

            glPopMatrix();
        }
    }
}

void FieldView::showSupporterPosition(Position supporterPosition) {
    if (!supporterPosition.isInvalid()) {
        glColor3d(0, 255, 255);
        drawArc(QVector2D(supporterPosition.x() * 1000.0f, supporterPosition.y() * 1000.0f), -15.0f, 15.0f,
                static_cast<float>(-M_PI), static_cast<float>(M_PI), 2.5f);
    }
}

Constants* FieldView::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Constants with nullptr value at FieldView") + '\n';
    }
    else {
        return _constants;
    }

    return nullptr;
}

WorldMap* FieldView::getWorldMap() {
    if(_worldMap == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("WorldMap with nullptr value at FieldView") + '\n';
    }
    else {
        return _worldMap;
    }

    return nullptr;
}

void FieldView::redraw() {
    _graphicsMutex.lock();
    updateFieldObjects();
    update();
    _graphicsMutex.unlock();
}
