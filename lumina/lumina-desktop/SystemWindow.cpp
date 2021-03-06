#include "SystemWindow.h"
#include "ui_SystemWindow.h"

SystemWindow::SystemWindow() : QDialog(), ui(new Ui::SystemWindow){
  ui->setupUi(this); //load the designer file
  //Setup the window flags
  this->setWindowFlags( Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  //Setup the icons based on the current theme
  ui->tool_logout->setIcon( LXDG::findIcon("system-log-out","") );
  ui->tool_restart->setIcon( LXDG::findIcon("system-reboot","") );
  ui->tool_shutdown->setIcon( LXDG::findIcon("system-shutdown","") );
  ui->push_cancel->setIcon( LXDG::findIcon("dialog-cancel","") );
  //Connect the signals/slots
  connect(ui->tool_logout, SIGNAL(clicked()), this, SLOT(sysLogout()) );
  connect(ui->tool_restart, SIGNAL(clicked()), this, SLOT(sysRestart()) );
  connect(ui->tool_shutdown, SIGNAL(clicked()), this, SLOT(sysShutdown()) );
  connect(ui->push_cancel, SIGNAL(clicked()), this, SLOT(sysCancel()) );
  //Center this window on the screen
  QDesktopWidget desktop;
  this->move(desktop.screenGeometry().width()/2 - this->width()/2, desktop.screenGeometry().height()/2 - this->height()/2);
  this->show();
}

SystemWindow::~SystemWindow(){
	
}