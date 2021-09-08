#include "gui.h"
#include "ui_gui.h"

#include <src/entities/coach/coach.h>
#include <src/entities/coach/coordinator/coordinator.h>
#include <src/entities/coach/player/player.h>

GUI::GUI(Constants *constants, WorldMap *worldMap, Coach *coach, Referee *referee, QWidget *parent) : QMainWindow(parent), ui(new Ui::GUI) {
    // Setting initial vars
    _supPosition.setPosition(false, 0.0, 0.0);
    _coach = coach;
    _referee = referee;
    _constants = constants;
    _worldMap = worldMap;

    // Setup GUI
    ui->setupUi(this);
    loadRobotFrames();
    setDarkTheme();

    // Setup fieldview
    ui->fieldView->setConstantsAndWorldMap(getConstants(), getWorldMap());

    // Setup tree
    treeInitialSetup();
}

GUI::~GUI() {
    delete ui;
}

void GUI::setDarkTheme() {
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window,QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
    darkPalette.setColor(QPalette::Base,QColor(42,42,42));
    darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
    darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
    darkPalette.setColor(QPalette::ToolTipText,Qt::white);
    darkPalette.setColor(QPalette::Text,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
    darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
    darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
    darkPalette.setColor(QPalette::Button,QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
    darkPalette.setColor(QPalette::BrightText,Qt::red);
    darkPalette.setColor(QPalette::Link,QColor(42,130,218));
    darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
    darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
    darkPalette.setColor(QPalette::HighlightedText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

    this->setPalette(darkPalette);
}

QPixmap GUI::getRobotPixmap(quint8 playerId) {
    QString path = QString(":/resources/robots/%1/%2%3.png")
            .arg(getConstants()->teamColorName())
            .arg((getConstants()->teamColor() == Colors::Color::BLUE) ? "b" : "y")
            .arg(playerId);

    QPixmap pixmap = QPixmap(path);

    if(pixmap.isNull()) {
        return QPixmap(":/resources/gui/none.png");
    }

    return QPixmap(path);
}

QPixmap GUI::getRolePixmap(QString roleName) {
    QString path = QString(":/resources/gui/roles/%1.png").arg(roleName.toLower());
    QPixmap pixmap = QPixmap(path);

    if(pixmap.isNull()) {
        return QPixmap(":/resources/gui/none.png");
    }

    return QPixmap(path);
}

void GUI::treeInitialSetup() {
    // Root
    ui->treeWidget->headerItem()->setIcon(0, QPixmap(":/resources/gui/strategy.png"));
    ui->treeWidget->headerItem()->setText(0, "NULL");
    ui->treeWidget->headerItem()->setText(1, "");
}

void GUI::updatePlaybookName(QString name) {
    /// TODO: (check this later) change pb icon
    ui->treeWidget->headerItem()->setText(0, name);
}

void GUI::updatePlayers(QList<quint8> &players) {
    // Delete players that don't appeared anymore
    QList<quint8> oldPlayers = _players.keys();
    for(int i = 0; i < oldPlayers.size(); i++) {
        // If actual players don't contain an old player
        if(!players.contains(oldPlayers.at(i))) {
            // Take player item
            QTreeWidgetItem *player = _players.take(oldPlayers.at(i));

            // Take role item
            QTreeWidgetItem *role = _playerRole.take(player);

            // Delete both
            delete role;
            delete player;
        }
    }

    // Insert/update players that appeared now =)
    QList<quint8> actualPlayers = _players.keys();
    for(int i = 0; i < players.size(); i++) {
        // If an player isn't contained in actual players list
        if(!actualPlayers.contains(players.at(i))) {
            // Create an tree widget item for player
            QTreeWidgetItem *player = new QTreeWidgetItem(ui->treeWidget);
            player->setText(0, QString("Robot %1").arg(players.at(i)));
            player->setIcon(0, getRobotPixmap(players.at(i)));

            // Add new player item to hash
            _players.insert(players.at(i), player);

            /// TODO: change role icon
            // Create an tree widget item for player role
            QTreeWidgetItem *role = new QTreeWidgetItem(player);

            // Add role item to hash
            _playerRole.insert(player, role);

            // Set expanded
            player->setExpanded(true);
        }

        // Update role
        QTreeWidgetItem *role = _playerRole.value(_players.value(players.at(i)));
        Player *playerPointer = getWorldMap()->getPlayerPointer(players.at(i));
        if(playerPointer == nullptr) {
            role->setText(0, "No Role");
            _robotFrames.at(playerPointer->playerId())->setRobotRole("No Role");
        }
        else {
            // Setting role name and icon
            QString roleName = playerPointer->roleName();
            role->setText(0, roleName);
            role->setIcon(0, getRolePixmap(roleName));
            _robotFrames.at(playerPointer->playerId())->setRobotRole(roleName);

            // Setting behavior
            role->setText(1, playerPointer->behaviorName());
        }
    }
}

void GUI::loadRobotFrames() {
    // Load frames
    _robotFrames.append(ui->robotframe_0);
    _robotFrames.append(ui->robotframe_1);
    _robotFrames.append(ui->robotframe_2);
    _robotFrames.append(ui->robotframe_3);
    _robotFrames.append(ui->robotframe_4);
    _robotFrames.append(ui->robotframe_5);

    // Set pattern
    for(int i = 0; i < _robotFrames.size(); i++) {
        _robotFrames.at(i)->setRobotData(getConstants()->teamColor(), i);
        _robotFrames.at(i)->setDisabled(true);
    }
}

void GUI::updateFrames() {
    for(int i = 0; i < getConstants()->qtPlayers(); i++) {
        // Get player object
        Object playerObject = getWorldMap()->getPlayer(getConstants()->teamColor(), i);

        // If player position is invalid
        if(playerObject.getPosition().isInvalid()) {
            // Disable and reset frame
            _robotFrames.at(i)->setDisabled(true);
            _robotFrames.at(i)->setRobotRadioStatus(false);
        }
        else {
            // Enable player frame
            _robotFrames.at(i)->setDisabled(false);

            // Take player pointer
            //Player *player = getWorldMap()->getPlayerPointer(i);

            // Enable radio status
            /// TODO: check how to do this with real world radio
            _robotFrames.at(i)->setRobotRadioStatus(true);
        }
    }
}

void GUI::updateField() {
    if (ui->supporterBox->isChecked()) {
        ui->fieldView->enableSupporterShow(true);
        Position desiredPosition(false, 0.0, 0.0);
        for (quint8 i = 0; i < _players.keys().size(); i++) {
            Player *playerPointer = getWorldMap()->getPlayerPointer(i);
            if (playerPointer->roleName() == "Role_Supporter") {
                desiredPosition = playerPointer->getPlayerDesiredPosition();
            }
        }
        ui->fieldView->setSupporterDesiredPosition(desiredPosition);
    } else {
        ui->fieldView->enableSupporterShow(false);
    }

    if (ui->defenderBox->isChecked()) {
        ui->fieldView->enableDefenderShow(true);
        Position desiredPosition(false, 0.0, 0.0);
        for (quint8 i = 0; i < _players.keys().size(); i++) {
            Player *playerPointer = getWorldMap()->getPlayerPointer(i);
            if (playerPointer->roleName() == "Role_Defender") {
                desiredPosition = playerPointer->getPlayerDesiredPosition();
                ui->fieldView->setDefenderDesiredPosition(desiredPosition);
            }
        }
    } else {
        ui->fieldView->enableDefenderShow(false);
    }

    if(ui->ballPredBox->isChecked()){
        ui->fieldView->enableBallPredShow(true);
        Position BallPredPos = getWorldMap()->getBall().getPredPosition(20);
        ui->fieldView->setBallPredParameters(BallPredPos);
    }
    else{
        ui->fieldView->enableBallPredShow(false);
    }

    if(ui->goalkeeperBox->isChecked()){
        ui->fieldView->enableGlkShow(true);
        if(getWorldMap()->getLocations()->ourGoal().x() > 0){
            //setDefenderEllipse(Position(true, 0.72f, 0.0f), 0.1f, 0.25f);
            Position ellipseCenter = Position(true, 0.72f, 0.0f);
            ui->fieldView->setGlkEllipseCenter(ellipseCenter);
            ui->fieldView->setGlkEllipseParameters(0.1f, 0.25f);
        }
        else{
            //setDefenderEllipse(Position(true, -0.72f, 0.0f), 0.1f, 0.25f);
            Position ellipseCenter = Position(true, -0.72f, 0.0f);
            ui->fieldView->setGlkEllipseCenter(ellipseCenter);
            ui->fieldView->setGlkEllipseParameters(0.1f, 0.25f);
        }
    }
    else{
        ui->fieldView->enableGlkShow(false);
    }



    ui->fieldView->redraw();
}

void GUI::updateTree() {
    // Take coordinator
    Coordinator *coordinator = getCoach()->getCoordinator();

    // Take actual playbook
    Playbook *actualPlaybook = coordinator->actualPlaybook();

    // Taking player list
    QList<quint8> pbList = actualPlaybook->getPlayers();

    // Update
    updatePlaybookName(actualPlaybook->name());
    updatePlayers(pbList);
}

Constants* GUI::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Constants with nullptr value at GUI") + '\n';
    }
    else {
        return _constants;
    }

    return nullptr;
}

WorldMap* GUI::getWorldMap() {
    if(_worldMap == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("WorldMap with nullptr value at GUI") + '\n';
    }
    else {
        return _worldMap;
    }

    return nullptr;
}

Coach* GUI::getCoach() {
    if(_coach == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Coach with nullptr value at GUI") + '\n';
    }
    else {
        return _coach;
    }

    return nullptr;
}

Referee* GUI::getReferee() {
    if(_referee == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Referee with nullptr value at GUI") + '\n';
    }
    else {
        return _referee;
    }

    return nullptr;
}
