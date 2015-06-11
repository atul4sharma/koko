import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
//import QtQuick.Window 2.1

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

MobileMainWindow {
    id: mainWindow
    width: 700
    height: 1100

    SystemPalette { id: sysPal; }

    mainItem: Rectangle {
        color: sysPal.dark
        width: mainWindow.width
        height: 1100
    }

    sidebar: Rectangle {
        color: sysPal.window
        width: mainWindow.width * 0.7
        height: 1100
    }
}