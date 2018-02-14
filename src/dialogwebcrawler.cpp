/***************************************************************************
 SocNetV: Social Network Visualizer
 version: 2.4
 Written in Qt
 
                         dialogwebcrawler.cpp  -  description
                             -------------------
    copyright            : (C) 2005-2017 by Dimitris B. Kalamaras
    email                : dimitris.kalamaras@gmail.com
 ***************************************************************************/

/*******************************************************************************
*     This program is free software: you can redistribute it and/or modify     *
*     it under the terms of the GNU General Public License as published by     *
*     the Free Software Foundation, either version 3 of the License, or        *
*     (at your option) any later version.                                      *
*                                                                              *
*     This program is distributed in the hope that it will be useful,          *
*     but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
*     GNU General Public License for more details.                             *
*                                                                              *
*     You should have received a copy of the GNU General Public License        *
*     along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
********************************************************************************/



#include "dialogwebcrawler.h"
#include <QDebug>
#include <QTextEdit>
#include <QPushButton>
#include <QUrl>

DialogWebCrawler::DialogWebCrawler(QWidget *parent) : QDialog (parent)
{
    ui.setupUi(this);

    (ui.buttonBox) -> button (QDialogButtonBox::Ok) -> setDefault(true);

    (ui.seedUrlEdit)->setFocus();

    ui.patternsIncludedTextEdit->setText("*");
    ui.patternsIncludedTextEdit->setToolTip("Enter a pattern i.e. example.com/pattern/*");

    ui.patternsExcludedTextEdit->setText("");
    ui.patternsExcludedTextEdit->setToolTip("Enter a pattern i.e. example.com/pattern/*");

    connect (ui.patternsExcludedTextEdit, &QTextEdit::textChanged,
             this, &DialogWebCrawler::checkErrors);

    connect (ui.extLinksCheckBox, &QCheckBox::stateChanged,
             this, &DialogWebCrawler::checkErrors);

    connect (ui.intLinksCheckBox, &QCheckBox::stateChanged,
             this, &DialogWebCrawler::checkErrors);

    connect ( ui.buttonBox,SIGNAL(accepted()), this, SLOT(gatherData()) );

    if ( !ui.extLinksCheckBox->isChecked()  &&!ui.intLinksCheckBox->isChecked() )
    {
        (ui.buttonBox) -> button (QDialogButtonBox::Ok)->setDisabled(true);
    }

}

void DialogWebCrawler::checkErrors(){
    qDebug()<< "DialogWebCrawler::checkErrors...";
    if ( !ui.extLinksCheckBox->isChecked()  && !ui.intLinksCheckBox->isChecked() )
    {
        (ui.buttonBox) -> button (QDialogButtonBox::Ok)->setDisabled(true);
    }
    else
        (ui.buttonBox) -> button (QDialogButtonBox::Ok)->setEnabled(true);

    if (ui.patternsExcludedTextEdit->toPlainText() != "" ) {
        qDebug () <<ui.patternsExcludedTextEdit->toPlainText();
    }
}

void DialogWebCrawler::gatherData(){
    qDebug()<< "DialogWebCrawler::gatherData()...";

    bool extLinks=true, intLinks=false;

    QString seedUrl = (ui.seedUrlEdit)->text();

    qDebug()<< "DialogWebCrawler::gatherData() initial seed url "
            << seedUrl
            << " simplifying and lowering it";

    seedUrl = seedUrl.simplified().toLower() ;

    qDebug()<< "DialogWebCrawler::gatherData() adding / to seed url ";
    seedUrl = seedUrl + "/";

    QUrl newUrl(seedUrl);

    qDebug()<< "DialogWebCrawler::gatherData()  QUrl " << newUrl.toString()
            << " scheme " << newUrl.scheme()
            << " host " << newUrl.host()
            << " path " << newUrl.path();

    if ( newUrl.scheme() != "http"  && newUrl.scheme() != "https"  &&
         newUrl.scheme() != "ftp" && newUrl.scheme() != "ftps") {
        qDebug()<< "DialogWebCrawler::gatherData()  URL scheme missing "
                << newUrl.scheme()
                << " setting the default scheme http ";
        newUrl = QUrl ("http://" + seedUrl);
        qDebug() << newUrl;
    }

    if (! newUrl.isValid() || newUrl.host() == "") {
        emit webCrawlerDialogError(seedUrl);
        qDebug()<< "DialogWebCrawler::gatherData()  not valid URL";
        return;
    }

    seedUrl = newUrl.toString();

    qDebug()<< "DialogWebCrawler::gatherData()  final seed url " << newUrl
            << " scheme " << newUrl.scheme()
            << " host " << newUrl.host()
            << " path " << newUrl.path();

    int maxLinksPerPage = (ui.maxLinksPerPageSpinBox) -> value();
    int totalUrlsToCrawl = (ui.totalUrlsToCrawlSpinBox) -> value();

    if ( ui.extLinksCheckBox -> isChecked() ) {
        qDebug()<< "	External links will be crawled... " ;
        extLinks = true;
    }
    else {
        qDebug()<< "	No external links... ";
        extLinks = false;
    }
    if ( ui.intLinksCheckBox -> isChecked() ) {
        qDebug()<< "	Internal links will be crawled too. " ;
        intLinks = true;
    }
    else {
        qDebug()<< "	No internal links. ";
        intLinks = false;
        if (!intLinks && !extLinks)
            return;
    }

    qDebug()<< "	seedUrl: " << seedUrl;
    qDebug()<< "	maxLinksPerPage " << maxLinksPerPage
            << "  totalUrlsToCrawl " << totalUrlsToCrawl  ;
    emit userChoices( seedUrl, totalUrlsToCrawl, maxLinksPerPage, extLinks, intLinks );
}
