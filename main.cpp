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

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <src/constants/constants.h>
#include <src/utils/text/text.h>
#include <src/exithandler.h>
#include <src/suassuna.h>

QCoreApplication *createApplication(int &argc, char *argv[]) {
    // Try to found in args an '--gui'
    bool foundArg = false;
    for (int i = 0; i < argc; ++i) {
        if (!qstrcmp(argv[i], "--gui")) {
            foundArg = true;
            break;
        }
    }

    // if not found, call core application
    if(!foundArg) {
        return new QCoreApplication(argc, argv);
    }
    // otherwise, call gui application
    else {
        return new QApplication(argc, argv);
    }
}

int main(int argc, char *argv[])
{
    QScopedPointer<QCoreApplication> a(createApplication(argc, argv));
    a->setApplicationName(APP_NAME);
    a->setApplicationVersion(APP_VERSION);

    std::cout << Text::bold(Text::center(" ____                                               ")) + '\n';
    std::cout << Text::bold(Text::center("/ ___|  _   _   __ _  ___  ___  _   _  _ __    __ _ ")) + '\n';
    std::cout << Text::bold(Text::center("\\___ \\ | | | | / _` |/ __|/ __|| | | || '_ \\  / _` |")) + '\n';
    std::cout << Text::bold(Text::center(" ___) || |_| || (_| |\\__ \\\\__ \\| |_| || | | || (_| |")) + '\n';
    std::cout << Text::bold(Text::center("|____/  \\__,_| \\__,_||___/|___/ \\__,_||_| |_| \\__,_|")) + '\n';
    std::cout << Text::bold(Text::center("Made with 💛 by Maracatronics Robotics Team.")) + '\n';
    std::cout << Text::bold(Text::center(a->applicationName().toStdString() + " v" + a->applicationVersion().toStdString())) + '\n' + '\n';

    // Setup command line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    // Setup application options
    // Use gui
    QCommandLineOption useGuiOption("gui", "Enable GUI mode");
    parser.addOption(useGuiOption);

    // Vision network
    QCommandLineOption visionNetwork("vision",
                                     QCoreApplication::translate("main", "Set vision address:port"),
                                     QCoreApplication::translate("main", "address>:<port"));
    parser.addOption(visionNetwork);

    // Referee network
    QCommandLineOption refereeNetwork("referee",
                                     QCoreApplication::translate("main", "Set referee address:port"),
                                     QCoreApplication::translate("main", "address>:<port"));
    parser.addOption(refereeNetwork);

    // Team color
    QCommandLineOption teamColor("teamColor",
                                     QCoreApplication::translate("main", "Set team color"),
                                     QCoreApplication::translate("main", "color"));
    parser.addOption(teamColor);

    // Team side
    QCommandLineOption teamSide("teamSide",
                                     QCoreApplication::translate("main", "Set team side"),
                                     QCoreApplication::translate("main", "side"));
    parser.addOption(teamSide);

    // Process parser in application
    parser.process(*a);

    // Setup ExitHandler
    ExitHandler::setApplication(a.data());
    ExitHandler::setup();

    // Initiating constants
    QScopedPointer<Constants> constants(new Constants(QString(PROJECT_PATH) + "/src/constants/constants.json"));

    // Update constants with parsed data
    // Vision
    if(parser.isSet(visionNetwork)) {
        constants->setManualVisionNetwork(parser.value(visionNetwork));
    }

    // Referee
    if(parser.isSet(refereeNetwork)) {
        constants->setManualRefereeNetwork(parser.value(refereeNetwork));
    }

    // Team color
    if(parser.isSet(teamColor)) {
        constants->setManualTeamColor(parser.value(teamColor));
    }

    // Team side
    if(parser.isSet(teamSide)) {
        constants->setManualTeamSide(parser.value(teamSide));
    }

    // Start Suassuna
    QScopedPointer<Suassuna> suassuna(new Suassuna(constants.data()));
    suassuna->start(parser.isSet(useGuiOption));

    bool exec = a->exec();

    // Stop Suassuna
    suassuna->stop();

    return exec;
}
