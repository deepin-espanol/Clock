#include "src/utils/configoperator.h"
#include "src/widgets/mainwindow.h"
#include "src/external/qtsingleapplication/dsingleapplication.h"

#include <DAboutDialog>

#ifndef VERSION
#define VERSION "1.0"
#endif

static const char *group_path = ":/icons/deepines.svg";

int main(int argc, char *argv[])
{
	DSingleApplication a(argc, argv);
	if (a.isRunning()) {
		a.sendMessage("show");
		exit(0);
	}

	//Setup the app
	DAboutDialog dialog;
	QTranslator ts;

	//[TODO] Check about how to properly put these in /usr/share/$appName/translations works
	//In future, use the built in DApplication way to intall the translations.
	ts.load("./.qm/tienda-deepines_" + QLocale::system().name() + ".qm");
	a.installTranslator(&ts);

	a.setAttribute(Qt::AA_UseHighDpiPixmaps);
	a.setOrganizationName("deepin-espanol");
	a.setApplicationName("clock");
	a.setApplicationDisplayName(a.tr("Clock"));
	a.setProductName(a.tr("Clock"));
	a.setApplicationVersion(DApplication::buildVersion(VERSION));
	a.setApplicationDescription(QObject::tr("Full featured clock application, with different timezones, alarms, a stopwatch and a timer."));
	a.setAboutDialog(&dialog);
	a.setWindowIcon(QIcon::fromTheme("clock"));
	a.setQuitOnLastWindowClosed(false);

	//Informations dialog
	dialog.setWindowTitle(a.tr("Clock"));
	//dialog.setProductName("<span>" + a.tr("Clock") + "</span>");
	dialog.setProductIcon(QIcon::fromTheme("clock"));
	dialog.setWindowIcon(QIcon::fromTheme("clock"));
	dialog.setCompanyLogo(QIcon(group_path).pixmap(64, 64));
	dialog.setDescription(
		 "<span>" + QObject::tr("Full featured clock application, with different timezones, alarms, a stopwatch and a timer.") + "</span>\n"
		 "<span style='font-size:8pt;font-weight:600;'>Deepin en Español</span>"
		 "<span style='font-size:8pt;font-weight:600;'>Deepin Latin Code - developers</span>");
	dialog.setVersion(a.tr("Version: ") + DApplication::buildVersion(VERSION));
	dialog.setLicense("GNU GPL v3");
	dialog.setWebsiteName("deepinenespañol.org");
	dialog.setWebsiteLink("https://deepinenespañol.org");


	//Create instances
	ConfigOperator *cop = new ConfigOperator(&a);
	MainWindow w;

	a.connect(&a, &DSingleApplication::messageReceived, [&w](const QString &msg) {
		if (msg == "show") {
			w.setWindowState(w.windowState() & ~Qt::WindowMinimized);
			w.raise();
			w.activateWindow();
			w.show();
		}
	});
	a.connect(&w, &MainWindow::closed, cop, &ConfigOperator::writeData);
	cop->loadData();
	w.show();

	return a.exec();
}
