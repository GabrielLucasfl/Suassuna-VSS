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

#ifndef FIELDVIEW_H
#define FIELDVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QTimer>
#include <QPainter>
#include <QMutex>
#include <QMouseEvent>
#include <GL/glu.h>
#include <GL/gl.h>

#include <src/constants/constants.h>
#include <src/entities/world/worldmap.h>
#include <src/gui/fieldview/gltext/gltext.h>
#include <src/gui/fieldview/fieldobjects/fieldobjects.h>

class FieldView : public QOpenGLWidget
{
    Q_OBJECT
public:
    FieldView(QWidget *parent = nullptr);
    void updateDefaultGeometry();
    void setConstantsAndWorldMap(Constants *constants, WorldMap *worldMap);

    // CheckBox enablers
    void enableSupporterShow(bool enabled) { _supporterShowEnabled = enabled; }
    void enableDefenderShow(bool enabled) { _defenderShowEnabled = enabled; }
    void enableGlkShow(bool enabled){ _glkEllipseEnabled = enabled; };

    // CheckBox setters
    void setSupporterDesiredPosition(Position supporterPosition) { _supporterDesiredPosition = supporterPosition; }
    void setDefenderDesiredPosition(Position defenderPosition) { _defenderDesiredPosition = defenderPosition; }
    void setGlkEllipseCenter(Position center) { _glkEllipseCenter = center; };
    void setGlkEllipseParameters(float a, float b) { _glkEllipseA = a; _glkEllipseB = b; };

protected:
    void paintGL();
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void resizeGL(int width, int height);

private:
    // Constants
    Constants *_constants;
    Constants* getConstants();

    // WorldMap
    WorldMap *_worldMap;
    WorldMap* getWorldMap();

    // Generalist draws
    void drawQuad(QVector2D v1, QVector2D v2, float z);
    void drawQuad(QVector2D v1, QVector2D v2, QVector2D v3, QVector2D v4, float z);
    void drawArc(QVector2D center, float r1, float r2, float theta1, float theta2, float z, float dTheta = -1);
    void drawTriangle(QVector2D v1, QVector2D v2, QVector2D v3, float z);
    void drawEllipse(QVector2D center, float a, float b, float thicknessPercentage, float theta1, float theta2, float z, float dTheta = -1);

    // Field view
    double viewScale; /// Ratio of world space to screen space coordinates
    double viewXOffset;
    double viewYOffset;
    void resetView();
    void recomputeProjection();

    // CheckBox enablers
    bool _supporterShowEnabled;
    bool _defenderShowEnabled;
    bool _glkEllipseEnabled;

    // CheckBox Parameters
    Position _supporterDesiredPosition;
    Position _defenderDesiredPosition;
    Position _glkEllipseCenter;
    float _glkEllipseA;
    float _glkEllipseB;

    // Mouse events
    bool leftButton;
    bool midButton;
    bool rightButton;
    int mouseStartX;
    int mouseStartY;

    // Text draw
    GLText glText;

    // Field Objects
    static constexpr float fieldZ = 1.0;
    static constexpr float robotZ = 2.0;
    static constexpr float ballZ = 3.0;
    static constexpr float minZValue = -10;
    static constexpr float maxZValue = 10;
    QList<FieldLine> _fieldLines;
    QList<FieldTriangle> _fieldTriangles;
    QList<FieldCircularArc> _fieldCircularArcs;
    QHash<Colors::Color, QList<quint8>> _players;
    Object _ball;
    void updateFieldObjects();
    void drawFieldLines();
    void drawBall();
    void drawRobots();

    // CheckBox functions
    void showSupporterPosition(Position supporterPosition);
    void showDefenderPosition(Position supporterPosition);
    void showGlkEllipse(Position center, float a, float b);

    // Mutex to control graphics
    QMutex _graphicsMutex;

public slots:
    void redraw();
};

#endif // FIELDVIEW_H
