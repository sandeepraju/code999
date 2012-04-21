/***************************************************************************
 *   Copyright (C) 2012 by Sandeep Raju P <sandeep080@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

//Qt includes
#include <QtGui/QDropEvent>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>
#include <QMessageBox>
#include <QListView>
#include <QDockWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QButtonGroup>
#include <QToolButton>
#include <QMenu>
#include <QLabel>
#include <QSize>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QHash>
#include <QtDebug>
#include <QGraphicsScene>
#include <QScrollArea>
#include <QWebView>
#include <QListView>
#include <QToolButton>
#include <QComboBox>
#include <QDesktopServices>
#include <QUrl>
#include <QHash>
#include <QListWidget>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QModelIndexList>

//KDE includes
#include <KConfigDialog>
#include <KStatusBar>
#include <KRatingWidget>
#include <KAction>
#include <KActionCollection>
#include <KStandardAction>
#include <KLocale>
#include <KListWidget>

//user defined includes
#include "moviemanager.h"
#include "nmm/movie.h"
#include "nfo/video.h"

//Nepomuk Includes
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NMM>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Query/QueryParser>
#include <Nepomuk/Variant>
#include <Nepomuk/Tag>
#include <Nepomuk/Utils/FacetWidget>
#include <Nepomuk/File>
#include <Nepomuk/Utils/SimpleResourceModel>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/Query>

//Soprano includes
#include <Soprano/QueryResultIterator>
#include <Soprano/Model>
#include <Soprano/Vocabulary/NAO>

moviemanager::moviemanager()
    : KXmlGuiWindow()
{
    //this shows the UI
    setupGUI();

    //calling all the user defined functions
    //initializing the application
    setupUserInterface();
}

moviemanager::~moviemanager()
{
}

void moviemanager::setupUserInterface()
{
    mainWindow = new QWidget(this);
    mainHLayout = new QHBoxLayout(mainWindow);

    firstVLayout = new QVBoxLayout(mainWindow);
    secondVLayout = new QVBoxLayout(mainWindow);
    thirdVLayout = new QVBoxLayout(mainWindow);
    mainHLayout->addItem(firstVLayout);
    mainHLayout->addItem(secondVLayout);
    mainHLayout->addItem(thirdVLayout);
    //add layouting information here

    //adding widgets into the firstVLayout
    //adding searchbar
    QHBoxLayout* searchHLayout = new QHBoxLayout(mainWindow);
    firstVLayout->addItem(searchHLayout);
    searchHLayout->addWidget(new QLabel("Search:", mainWindow));
    searchBar = new KLineEdit("", mainWindow);
    searchHLayout->addWidget(searchBar);
    searchHLayout->setAlignment(Qt::AlignTop); //searchbar alignment
    //connecting searchbar return pressed to a slot
    if(connect(searchBar,
               SIGNAL(returnPressed(QString)),
               this,
               SLOT(slotSearchBarReturnPressed(QString))) == false)
    {
        qDebug() << "searchBar connect is not successful";
    }

    //adding klistwidget for displaying movies
    mainMovieList = new KListWidget(mainWindow);
    mainMovieList->setSortingEnabled(true);
    fetchAllMovieList(mainMovieList);
    firstVLayout->addWidget(mainMovieList);
    //connecting the mainMovieList signal to a slot
    if(connect(mainMovieList,
               SIGNAL(activated(QModelIndex)),
               this,
               SLOT(slotMovieClicked(QModelIndex))) == false)
    {
        qDebug() << "mainMovieList connect is not successful";
    }

    //adding widgets into secondVLayout


    //adding widgets into thirdVLayout
    thirdVLayout->addWidget(new QLabel("<span style='font-size: 20px;'>Recommended Movies</span>", mainWindow));
    recoMovieList = new KListWidget(mainWindow);
    recoMovieList->setSortingEnabled(true);
    fetchRecoMovieList(recoMovieList);
    thirdVLayout->addWidget(recoMovieList);
    //connecting the mainMovieList signal to a slot
    if(connect(recoMovieList,
               SIGNAL(activated(QModelIndex)),
               this,
               SLOT(slotMovieClicked(QModelIndex))) == false)
    {
        qDebug() << "recoMovieList connect is not successful";
    }



    setCentralWidget(mainWindow);
}

void moviemanager::fetchAllMovieList(KListWidget* mainMovieList)
{
    qDebug() << "fetch All Movie List";
    Nepomuk::Query::Term term =  Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Video());
    Nepomuk::Query::Query currentQuery;
    currentQuery.setTerm(term);
    QList<Nepomuk::Query::Result> results = Nepomuk::Query::QueryServiceClient::syncQuery( currentQuery );

    Q_FOREACH( const Nepomuk::Query::Result& result,results)
    {
        //An if condition should be added here...
        //If the synopsis propoery is not set, then create a qdownloader object
        //and send the data to fetch from imdb
        //and then let that function add the movie to the list
        if(result.resource().property(Nepomuk::Vocabulary::NFO::fileName()).toString().contains(".avi") == true)   //allowing only .avi files
        {
            qDebug() << "yes, .avi file!";
            if(result.resource().property(Nepomuk::Vocabulary::NMM::synopsis()).toString().contains("tt") == true)     //checking for synopsis
            {
                //only .avi and
                //data already present, so adding to the list
                qDebug() << "yes, contains synopsis";

                //very rarely if title does not exists
                if(result.resource().property(Nepomuk::Vocabulary::NIE::title()).toString().isEmpty() == true)
                {
                    //Title does not exists
                    qDebug() << "title does not exist, so showing file names";
                    mainMovieList->addItem(new QListWidgetItem(result.resource().property(Nepomuk::Vocabulary::NFO::fileName()).toString().remove(".avi")));
                }
                else
                {
                    //Title exists, so displaying the title
                    qDebug() << "Title exists, showing title of the movie: " << result.resource().property(Nepomuk::Vocabulary::NIE::title()).toString();
                    mainMovieList->addItem(new QListWidgetItem(result.resource().property(Nepomuk::Vocabulary::NIE::title()).toString()));
                }
            }
            else
            {
                //.avi but no data and tt synopsis
                qDebug() << "synopsis: " << result.resource().property(Nepomuk::Vocabulary::NMM::synopsis()).toString();
                //no data, so data needs to be fetched
                //item will be added to the list later
                IMDB* imdbFetcher = new IMDB(result.resource(), mainMovieList);
                imdbFetcher->getData();
            }
        }
    }
}

void moviemanager::fetchRecoMovieList(KListWidget * recoMovieList)
{
    //fetch recommended movie list
    //discuss with vijesh
    qDebug() << "fetch Reco Movie List";
    //fetchAllMovieList(recoMovieList);   //temporarily until vijesh gives the module
}

//All slot definitions
void moviemanager::slotSearchBarReturnPressed(QString userInput)
{
    qDebug() << "search bar return pressed";
    qDebug() << "user entered: " << userInput;
}

void moviemanager::slotMovieClicked(QModelIndex currentIndex)
{
    qDebug() << "movie item clicked";
    QString movieFileName = QString(currentIndex.model()->data(currentIndex).toString());    //extracting the name of the movie
    qDebug() << movieFileName << " selected";
    //query nepomuk and get the details of the movie and
    //display it in the secondVLayout
}

#include "moviemanager.moc"
