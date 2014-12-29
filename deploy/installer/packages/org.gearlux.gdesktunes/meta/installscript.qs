/**************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
**************************************************************************/

var Dir = new function () {
    this.toNativeSparator = function (path) {
        if (installer.value("os") == "win")
            return path.replace(/\//g, '\\');
        return path;
    }
};

// Constructor
function Component()
{
    component.loaded.connect(this, my_componentLoaded);
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, false);
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, true);
    // installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
    // installer.setDefaultPageVisible(QInstaller.PerformInstallation, false);
    // installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
}

// Called as soon as the component was loaded
my_componentLoaded = function()
{
	// don't show when updating / de-installing
	if (installer.isInstaller()) {
        if (installer.addWizardPage(component, "TargetWidget", QInstaller.TargetDirectory)) {
            var widget = gui.pageWidgetByObjectName("DynamicTargetWidget");
            if (widget != null) {
                widget.targetDirectory.textChanged.connect(this, Component.prototype.targetChanged);
                widget.targetChooser.clicked.connect(this, Component.prototype.chooseTarget);

                widget.windowTitle = "Installation Folder";
                // Hack for QtIFW 1.4.0
                // 1.4.0 is needed because two shortcuts crash from 1.5.0
                // var userProfile = installer.environmentVariable("ProgramFiles");
                // installer.setValue("ProgramFiles", userProfile + "\\GDeskTunes");
                widget.targetDirectory.text = Dir.toNativeSparator(installer.value("TargetDir"));

                widget.labelWarning.visible = false;
            }
        }
	}
}
Component.prototype.isDefault = function()
{
    // select the component by default
    return true;
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();

		if (installer.value("os") === "win") {
			component.addOperation("CreateShortcut", "@TargetDir@/GDeskTunes.exe", "@StartMenuDir@/GDeskTunes.lnk",
				"workingDirectory=@TargetDir@");
            if (component.userInterface( "TargetWidget" ).desktopShortcut.checked)
            {
                // IFW crashses on the second shortcut
                // We will have to do it this way
                component.addOperation("CreateShortcut", "@TargetDir@/GDeskTunes.exe", "@DesktopDir@/GDeskTunes.lnk", "workingDirectory=@TargetDir@");
                // component.addOperation("Execute", "cmd", "/c" , "shortcut.bat", "\"@TargetDir@\\GDeskTunes.exe\"", "\"@DesktopDir@\\GDeskTunes.lnk\"", "workingDirectory=@TargetDir@");
            }
        }

	} catch (e) {
        print(e);
    }
}

// Callback when one is clicking on the button to select where to install your application

Component.prototype.chooseTarget = function () {
    var widget = gui.pageWidgetByObjectName("DynamicTargetWidget");
    if (widget != null) {
        var newTarget = QFileDialog.getExistingDirectory("Choose your target directory.", widget.targetDirectory.text);
        if (newTarget != "") {
            widget.targetDirectory.text = Dir.toNativeSparator(newTarget);
        }
    }
}

Component.prototype.targetChanged = function (text) {
    var widget = gui.pageWidgetByObjectName("DynamicTargetWidget");
    if (widget != null) {
        if (text != "") {
            widget.complete = true;
            installer.setValue("TargetDir", text);
            if (installer.fileExists(text + "/components.xml")) {
                var warning = "<font color='red'>" + qsTr("A previous installation exists in this folder. If you wish to continue, everything will be overwritten.") + "</font>";
                widget.labelOverwrite.text = warning;
            } else {
                widget.labelOverwrite.text = "";
            }
            return;
        }
        widget.complete = false;
    }
}

