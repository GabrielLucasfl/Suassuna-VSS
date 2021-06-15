# Qt libs to import
QT += core    \
      gui     \
      widgets \
      network \
      opengl

# Project configs
TEMPLATE = app
DESTDIR  = ../bin
TARGET   = Suassuna-VSS
VERSION  = 1.0.0

CONFIG += c++14 console
CONFIG -= app_bundle

# Temporary dirs
OBJECTS_DIR = tmp/obj
MOC_DIR = tmp/moc
UI_DIR = tmp/moc
RCC_DIR = tmp/rc

# Project libs
LIBS *= -lprotobuf -lQt5Core -lGLU

# Compiling .proto files
system(echo "Compiling protobuf files" && cd include/proto && protoc --cpp_out=../ *.proto && cd ../..)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_NAME=\\\"$$TARGET\\\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += PROJECT_PATH=\\\"$${PWD}\\\"

# Project source files
SOURCES += \
    include/command.pb.cc \
    include/common.pb.cc \
    include/packet.pb.cc \
    include/replacement.pb.cc \
    include/vssref_command.pb.cc \
    include/vssref_common.pb.cc \
    include/vssref_placement.pb.cc \
    main.cpp \
    src/constants/constants.cpp \
    src/entities/actuator/actuator.cpp \
    src/entities/actuator/simulator/simactuator.cpp \
    src/entities/coach/behavior/barrier/behavior_barrier.cpp \
    src/entities/coach/behavior/behavior.cpp \
    src/entities/coach/behavior/goToBall/behavior_goToBall.cpp \
    src/entities/coach/behavior/intercept/behavior_intercept.cpp \
    src/entities/coach/behavior/moveto/behavior_moveto.cpp \
    src/entities/coach/coach.cpp \
    src/entities/coach/coordinator/coordinator.cpp \
    src/entities/coach/coordinator/ssl/coordinator_vss.cpp \
    src/entities/coach/playbook/default/playbook_default.cpp \
    src/entities/coach/playbook/playbook.cpp \
    src/entities/coach/player/navigation/navalgorithm.cpp \
    src/entities/coach/player/navigation/navigation.cpp \
    src/entities/coach/player/navigation/rrt/rrt.cpp \
    src/entities/coach/player/navigation/rrt/rrtnode.cpp \
    src/entities/coach/player/navigation/rrt/rrtobstacle.cpp \
    src/entities/coach/player/navigation/rrt/rrttree.cpp \
    src/entities/coach/player/navigation/rrt/rrtvertex.cpp \
    src/entities/coach/player/player.cpp \
    src/entities/coach/role/attacker/role_attacker.cpp \
    src/entities/coach/role/default/role_default.cpp \
    src/entities/coach/role/defender/role_defender.cpp \
    src/entities/coach/role/role.cpp \
    src/entities/coach/role/role_gk/role_goalkeeper.cpp \
    src/entities/coach/role/supporter/role_supporter.cpp \
    src/entities/coach/role/tmpDef/role_tmp_def.cpp \
    src/entities/coach/role/midfielder/role_midfielder.cpp \
    src/entities/coach/skill/goto/skill_goto.cpp \
    src/entities/coach/skill/rotateto/skill_rotateto.cpp \
    src/entities/coach/skill/skill.cpp \
    src/entities/coach/skill/spin/skill_spin.cpp \
    src/entities/entity.cpp \
    src/entities/referee/referee.cpp \
    src/entities/vision/filters/kalman/kalmanfilter.cpp \
    src/entities/vision/filters/kalman/matrix/matrix.cpp \
    src/entities/vision/filters/kalman/state/kalmanstate.cpp \
    src/entities/vision/filters/loss/lossfilter.cpp \
    src/entities/vision/filters/noise/noisefilter.cpp \
    src/entities/vision/vision.cpp \
    src/entities/world/locations/locations.cpp \
    src/entities/world/world.cpp \
    src/entities/world/worldmap.cpp \
    src/exithandler.cpp \
    src/gui/fieldview/fieldobjects/fieldobjects.cpp \
    src/gui/fieldview/fieldview.cpp \
    src/gui/fieldview/gltext/gltext.cpp \
    src/gui/gui.cpp \
    src/gui/robotframe/robotframe.cpp \
    src/suassuna.cpp \
    src/utils/freeangles/freeangles.cpp \
    src/utils/freeangles/obstacle.cpp \
    src/utils/graph/edge.cpp \
    src/utils/graph/graph.cpp \
    src/utils/graph/vertex.cpp \
    src/utils/text/text.cpp \
    src/utils/timer/timer.cpp \
    src/utils/types/angle/angle.cpp \
    src/utils/types/angularspeed/angularspeed.cpp \
    src/utils/types/field/field.cpp \
    src/utils/types/field/vss/field_vssb.cpp \
    src/utils/types/fieldside/fieldside.cpp \
    src/utils/types/object/object.cpp \
    src/utils/types/position/position.cpp \
    src/utils/types/velocity/velocity.cpp \
    src/utils/types/wall/wall.cpp \
    src/utils/utils.cpp


# Project header files
HEADERS += \
    include/command.pb.h \
    include/common.pb.h \
    include/packet.pb.h \
    include/replacement.pb.h \
    include/vssref_command.pb.h \
    include/vssref_common.pb.h \
    include/vssref_placement.pb.h \
    src/constants/constants.h \
    src/entities/actuator/actuator.h \
    src/entities/actuator/simulator/simactuator.h \
    src/entities/coach/basecoach.h \
    src/entities/coach/behavior/barrier/behavior_barrier.h \
    src/entities/coach/behavior/behavior.h \
    src/entities/coach/behavior/behaviors.h \
    src/entities/coach/behavior/goToBall/behavior_goToBall.h \
    src/entities/coach/behavior/intercept/behavior_intercept.h \
    src/entities/coach/behavior/moveto/behavior_moveto.h \
    src/entities/coach/coach.h \
    src/entities/coach/coordinator/coordinator.h \
    src/entities/coach/coordinator/ssl/coordinator_vss.h \
    src/entities/coach/playbook/default/playbook_default.h \
    src/entities/coach/playbook/playbook.h \
    src/entities/coach/playbook/playbooks.h \
    src/entities/coach/player/navigation/navalgorithm.h \
    src/entities/coach/player/navigation/navigation.h \
    src/entities/coach/player/navigation/rrt/rrt.h \
    src/entities/coach/player/navigation/rrt/rrtnode.h \
    src/entities/coach/player/navigation/rrt/rrtobstacle.h \
    src/entities/coach/player/navigation/rrt/rrttree.h \
    src/entities/coach/player/navigation/rrt/rrtvertex.h \
    src/entities/coach/player/player.h \
    src/entities/coach/role/attacker/role_attacker.h \
    src/entities/coach/role/default/role_default.h \
    src/entities/coach/role/defender/role_defender.h \
    src/entities/coach/role/role.h \
    src/entities/coach/role/role_gk/role_goalkeeper.h \
    src/entities/coach/role/roles.h \
    src/entities/coach/role/tmpDef/role_tmp_def.h \
    src/entities/coach/role/midfielder/role_midfielder.h \
    src/entities/coach/skill/goto/skill_goto.h \
    src/entities/coach/skill/rotateto/skill_rotateto.h \
    src/entities/coach/skill/skill.h \
    src/entities/coach/skill/skills.h \
    src/entities/coach/skill/spin/skill_spin.h \
    src/entities/entity.h \
    src/entities/referee/referee.h \
    src/entities/vision/filters/kalman/kalmanfilter.h \
    src/entities/vision/filters/kalman/matrix/matrix.h \
    src/entities/vision/filters/kalman/state/kalmanstate.h \
    src/entities/vision/filters/loss/lossfilter.h \
    src/entities/vision/filters/noise/noisefilter.h \
    src/entities/vision/filters/visionfilters.h \
    src/entities/vision/vision.h \
    src/entities/world/locations/locations.h \
    src/entities/world/world.h \
    src/entities/world/worldmap.h \
    src/exithandler.h \
    src/gui/fieldview/fieldobjects/fieldobjects.h \
    src/gui/fieldview/fieldview.h \
    src/gui/fieldview/gltext/gltext.h \
    src/gui/gui.h \
    src/gui/robotframe/robotframe.h \
    src/suassuna.h \
    src/utils/freeangles/freeangles.h \
    src/utils/freeangles/obstacle.h \
    src/utils/graph/edge.h \
    src/utils/graph/graph.h \
    src/utils/graph/graphbase.h \
    src/utils/graph/vertex.h \
    src/utils/text/text.h \
    src/utils/timer/timer.h \
    src/utils/types/angle/angle.h \
    src/utils/types/angularspeed/angularspeed.h \
    src/utils/types/color/color.h \
    src/utils/types/field/field.h \
    src/utils/types/field/vss/field_vssb.h \
    src/utils/types/fieldside/fieldside.h \
    src/utils/types/fieldside/side.h \
    src/utils/types/object/object.h \
    src/utils/types/position/position.h \
    src/utils/types/velocity/velocity.h \
    src/utils/types/wall/wall.h \
    src/utils/utils.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    src/gui/gui.ui

RESOURCES += \
    resources.qrc
