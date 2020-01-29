/*
 *   Copyright 2016 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.4 as Kirigami
import qmlelixir 1.0 as Elixir

Kirigami.ApplicationWindow {
    id: root

    globalDrawer: Kirigami.GlobalDrawer {
        title: "Hello App"
        titleIcon: "applications-graphics"

        actions: [
            Kirigami.Action {
                text: "View"
                iconName: "view-list-icons"
                Kirigami.Action {
                    text: "action 1"
                }
                Kirigami.Action {
                    text: "action 2"
                }
                Kirigami.Action {
                    text: "action 3"
                }
            },
            Kirigami.Action {
                text: "action 3"
                onTriggered: hack.send("QML SAYS HELLO")
            },
            Kirigami.Action {
                text: "action 4"
            }
        ]
    }
    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    pageStack.initialPage: mainPageComponent

    Component {
        id: mainPageComponent
        Kirigami.ScrollablePage {
            title: "Hello"
            Elixir.ElixirChannel {
                id: elixirChannel
                typeId: "elixirTestChannel"
                property alias test: textField.text
                property var variantProp: []
                onVariantPropChanged: {
                    print("variantProp property changed to type: " + (typeof variantProp))
                    if (typeof variantProp === "object") {
                        for (var i in variantProp) {
                            print(i + ": " + variantProp[i])
                        }
                    } else {
                        print(variantProp);
                    }
                }
                signal testSignal(string param1, int param2)
                signal missingSignal(string param1, string param2)
            }
            header: Controls.ToolBar {
                Kirigami.Theme.inherit: false
                Kirigami.Theme.colorSet: Kirigami.Theme.Window
                contentItem: RowLayout {
                    Controls.Label {
                        text: hack.test
                    }
                    Controls.ToolButton {
                        text: "Send Signal"
                        onClicked: elixirChannel.testSignal("First param", 123)
                    }
                    Controls.ToolButton {
                        text: "Send Unmanaged Signal"
                        onClicked: elixirChannel.missingSignal("First param", "second param")
                    }
                    Controls.TextField {
                        id: textField
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                }
            }
            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: Elixir.ElixirModelChannel {
                    typeId: "testModel"
                    roles: ["title", "description"]
                }
                delegate: Kirigami.AbstractListItem {
                    contentItem: ColumnLayout {
                        Controls.Label {
                            text: model.title
                        }
                        Controls.Label {
                            text: model.description
                        }
                    }
                }
            }
        }
    }


}
