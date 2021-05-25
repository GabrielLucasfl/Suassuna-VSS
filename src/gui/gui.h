#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QLabel>

#include <src/entities/world/worldmap.h>

namespace Ui {
class GUI;
}
class RobotFrame;

class GUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUI(Constants *constants, WorldMap *worldMap, Coach *coach, Referee *referee, QWidget *parent = nullptr);
    ~GUI();

private:
    // Internal
    Ui::GUI *ui;
    void setDarkTheme();

    // Tree management
    QHash<quint8, QTreeWidgetItem*> _players;
    QHash<QTreeWidgetItem*, QTreeWidgetItem*> _playerRole;
    Position _supPosition;
    QPixmap getRobotPixmap(quint8 playerId);
    QPixmap getRolePixmap(QString roleName);
    void treeInitialSetup();
    void updatePlaybookName(QString name);
    void updatePlayers(QList<quint8> &players);

    // Frames management
    QList<RobotFrame*> _robotFrames;
    void loadRobotFrames();

    // Constants
    Constants *_constants;
    Constants* getConstants();

    // Locations
    WorldMap *_worldMap;
    WorldMap* getWorldMap();

    // Coach
    Coach *_coach;
    Coach* getCoach();

    // Referee
    Referee *_referee;
    Referee* getReferee();

public slots:
    void updateFrames();
    void updateField();
    void updateTree();
};

#endif // GUI_H
