#include "mainUI.h"
#include "ui_mainUI.h" //the designer *.ui file

MainUI::MainUI() : QMainWindow(), ui(new Ui::MainUI()){
  ui->setupUi(this); //load the designer file
  //Be careful about the QSettings setup, it must match the lumina-desktop setup
  QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, QDir::homePath()+"/.lumina");
  settings = new QSettings( QSettings::UserScope, "LuminaDE", "desktopsettings", this);
  qDebug() << "Settings File:" << settings->fileName();
  desktop = new QDesktopWidget();
  if(desktop->screenCount() == 1){
    ui->spin_screen->setValue(1);
    //Hide these since no other screens
    ui->group_screen->setVisible(false);
  }else{
    //Make sure this is only allows the current number of screens
    ui->spin_screen->setMaximum(desktop->screenCount());
  }

  //Setup the buttons signal/slot connections
  connect(ui->spin_screen, SIGNAL(valueChanged(int)), this, SLOT(loadCurrentSettings()) );
  connect(ui->tool_rmbackground, SIGNAL(clicked()), this, SLOT(removeBackground()) );
  connect(ui->tool_addbackground, SIGNAL(clicked()), this, SLOT(addBackground()) );
  connect(ui->push_save, SIGNAL(clicked()), this, SLOT(saveCurrentSettings()) );
  // - toolbar tab
  connect(ui->spin_tb_R, SIGNAL(valueChanged(int)), this, SLOT(colorChanged()) );
  connect(ui->spin_tb_G, SIGNAL(valueChanged(int)), this, SLOT(colorChanged()) );
  connect(ui->spin_tb_B, SIGNAL(valueChanged(int)), this, SLOT(colorChanged()) );
  //Now finish setting up the UI
  setupIcons();
  QTimer::singleShot(10, this, SLOT(loadCurrentSettings()) );
}

MainUI::~MainUI(){
	
}

void MainUI::slotSingleInstance(){
  //Make sure this window is visible
  this->showNormal();
  this->activateWindow();
  this->raise();
}

//================
//  PRIVATE FUNCTIONS
//================
void MainUI::setupIcons(){
  //Pull all the icons from the current theme using libLumina (LXDG)
  //  - Menu Items
  ui->actionSave_and_Quit->setIcon( LXDG::findIcon("document-save-all","") );
  ui->actionClose->setIcon( LXDG::findIcon("application-exit","") );
	
  //  - Background tab
  ui->tool_rmbackground->setIcon( LXDG::findIcon("list-remove","") );
  ui->tool_addbackground->setIcon( LXDG::findIcon("list-add","") );
  //  - Toolbar tab
  ui->tool_tb_addplugin->setIcon( LXDG::findIcon("list-add", "") );
  ui->tool_tb_rmplugin->setIcon( LXDG::findIcon("list-remove", "") );
  ui->tool_tb_rightplugin->setIcon( LXDG::findIcon("go-next-view", "") );
  ui->tool_tb_leftplugin->setIcon( LXDG::findIcon("go-previous-view", "") );
  ui->tool_tb_addpanel->setIcon( LXDG::findIcon("list-add", "") );
  ui->tool_tb_rmpanel->setIcon( LXDG::findIcon("list-remove", "") );
  //  - General UI buttons
  ui->push_save->setIcon( LXDG::findIcon("document-save","") );
}

void MainUI::checkForChanges(){
  //to see whether to enable the save button
  // -- TO DO (this should make the save button enabled only if changes are available)
	
}

int MainUI::currentDesktop(){
  return ui->spin_screen->value()-1; //backend starts at 0, not 1
}

int MainUI::currentPanel(){
  return 0; // - TO DO	
}

void MainUI::addNewBackgroundFile(QString filepath){
  QListWidgetItem *item = new QListWidgetItem(ui->list_backgrounds);
	item->setText(filepath.section("/",-1));
	item->setToolTip(filepath);
	item->setWhatsThis(filepath); //save the full path in this variable
	item->setIcon( QIcon(filepath) );
}

//================
//    PRIVATE SLOTS
//================
//General Utility Functions
void MainUI::loadCurrentSettings(){
  settings->sync();
  QString DPrefix = "desktop-"+QString::number(currentDesktop())+"/";
  QString PPrefix = "panel"+QString::number(currentDesktop())+"."+QString::number(currentPanel())+"/";
  //The Background tab
    //Setup the bg preview image size (correct aspect ratio for the current screen)
      int icoWidth = ui->list_backgrounds->width()/3;
      double ratio = desktop->screenGeometry(currentDesktop()).height()/ ( (double) desktop->screenGeometry(currentDesktop()).width() );
      ui->list_backgrounds->setIconSize( QSize(icoWidth, icoWidth*ratio) );
    //Load the background files
    QStringList bgs = settings->value(DPrefix+"background/filelist", QStringList()<<"default").toStringList();
    qDebug() << "Backgrounds:" << DPrefix << bgs;
    ui->list_backgrounds->clear();
    for(int i=0; i<bgs.length(); i++){
      if(bgs[i]=="default"){ bgs[i]=DEFAULTBG; }
      if(QFile::exists(bgs[i])){
        addNewBackgroundFile(bgs[i]);
      }
    }
    if(bgs.length() <= 1){ ui->radio_singleBG->setChecked(true); }
    else{ ui->radio_rotateBG->setChecked(true); }
    ui->spin_bgRotateMin->setValue( settings->value(DPrefix+"background/minutesToChange", 5).toInt() );
    
}

void MainUI::saveCurrentSettings(){
  QString DPrefix = "desktop-"+QString::number(currentDesktop())+"/";
  QString PPrefix = "panel"+QString::number(currentDesktop())+"."+QString::number(currentPanel())+"/";
  //Now save the current settings from the UI
    // Background Tab
    QStringList bgs; //get the list of backgrounds to use
    if(ui->radio_rotateBG->isChecked()){
      for(int i=0; i<ui->list_backgrounds->count(); i++){
	bgs << ui->list_backgrounds->item(i)->whatsThis();
      }
    }else{
	QListWidgetItem *it = ui->list_backgrounds->currentItem();
	if(it != 0){ bgs << it->whatsThis(); }
	else if(ui->list_backgrounds->count() > 0){ bgs << ui->list_backgrounds->item(0)->whatsThis(); }
    }
    if(bgs.isEmpty()){ bgs << "default"; }
    settings->setValue(DPrefix+"background/filelist", bgs);
    settings->setValue(DPrefix+"background/minutesToChange", ui->spin_bgRotateMin->value());

    //Panels tab
    
    //All done - make sure the changes get saved to file right now
    settings->sync();
}

//Background Tab Functions
void MainUI::addBackground(){
  //Prompt the user to find an image file to use for a background
  QString dir = "/usr/local/share/wallpapers";
  if( !QFile::exists(dir) ){ dir = QDir::homePath(); }
  QStringList bgs = QFileDialog::getOpenFileNames(this, tr("Find Background Image(s)"), dir, "Images (*.png *.xpm *.jpg)");
  for(int i=0; i<bgs.length(); i++){
    addNewBackgroundFile(bgs[i]);
  }
  
}

void MainUI::removeBackground(){
  //Remove the currently selected background from the list
  int item = ui->list_backgrounds->currentRow();
  if(item >= 0){
    delete ui->list_backgrounds->takeItem(item);
  }
}

//ToolBar Tab Functions
void MainUI::colorChanged(){
  //Get the RGB color
  QString color = "rgb(%1,%2,%3)";
  color = color.arg(QString::number(ui->spin_tb_R->value()), QString::number(ui->spin_tb_G->value()), QString::number(ui->spin_tb_B->value()) );
  //Now change the color of the label for example
  ui->label_tb_color->setStyleSheet("background: "+color+";");
}

