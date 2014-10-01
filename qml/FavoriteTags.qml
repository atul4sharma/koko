/*
 * Copyright (C) 2014 Vishesh Handa <vhanda@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

ColumnLayout {
    PlasmaExtras.Heading {
        text: "Favorite Tags"
        font.bold: true
        level: 4

        Layout.fillWidth: true
    }

    ListView {
        height: 500
        Layout.fillWidth: true

        delegate: Tag {
            text: model.name
            color: model.color
        }
        model: ListModel {
            ListElement {
                name: "Biology Presentation"
                color: "orange"
            }
            ListElement {
                name: "Beach"
                color: "green"
            }
            ListElement {
                name: "Wow"
                color: "cyan"
            }
        }
    }
}