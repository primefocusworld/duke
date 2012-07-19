#include "UIFileDialog.h"
#include <QGridLayout>
#include <iostream>

UIFileDialog::UIFileDialog(QWidget * parent, Qt::WindowFlags flags) :
    QFileDialog(parent, flags) {
    setupCustomUI();
}

UIFileDialog::UIFileDialog(QWidget * parent, const QString & caption, const QString & directory, const QString & filter) :
    QFileDialog(parent, caption, directory, filter) {
    setupCustomUI();
}

bool UIFileDialog::browseDirectory() const {
    return mBrowseCheckbox->isChecked();
}

// private slot
void UIFileDialog::browseCheckbox_stateChanged(int state){
    if(state)
        setFileMode(QFileDialog::ExistingFile);
    else
        setFileMode(QFileDialog::ExistingFiles);
}

// private
void UIFileDialog::setupCustomUI() {
    // global filedialog settings
    setViewMode(QFileDialog::Detail);
    setFileMode(QFileDialog::ExistingFile);
    setNameFilter(tr("All (*.pic *.pbm *.dpx *.tpic *.png *.pgm *.jpg *.pnm *.bmp *.exr *.sgi *.tiff *.tif *.dds *.tga *.jp2 *.rgb *.j2k *.jpeg *.ico *.hdr *.ppm *.dk *.ppl);;Images (*.pic *.pbm *.dpx *.tpic *.png *.pgm *.jpg *.pnm *.bmp *.exr *.sgi *.tiff *.tif *.dds *.tga *.jp2 *.rgb *.j2k *.jpeg *.ico *.hdr *.ppm);;Playlists (*.dk *.ppl)"));
    // custom checkbox
    mBrowseCheckbox = new QCheckBox(this);
    mBrowseCheckbox->setText("Browse directory");
    mBrowseCheckbox->setChecked(false);
    connect(mBrowseCheckbox, SIGNAL(stateChanged(int)), this, SLOT(browseCheckbox_stateChanged(int)));
    QGridLayout *layout = (QGridLayout*)this->layout();
    layout->addWidget(mBrowseCheckbox, 4, 0);
}
