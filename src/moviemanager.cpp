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
#include <QScrollBar>

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
#include <Soprano/QueryResultIterator>600
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
    secondVLayout->addWidget(new QLabel("<span style='font-size: 20px;'>Movie Details</span>", mainWindow));
    QScrollArea * scrollArea = new QScrollArea(mainWindow);
    //scrollArea->setLayout();
    //scrollArea->setAlignment(Qt::AlignBottom);
    secondVLayout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(false);
    //scrollArea->setVerticalScrollBar(new QScrollBar());
    middleInnerWidget = new QWidget(mainWindow);
    //middleInnerWidget->setMaximumHeight(50);
    scrollArea->setWidget(middleInnerWidget);
    //secondVLayout->addWidget(middleInnerWidget);
    middleInnerVLayout = new QVBoxLayout(middleInnerWidget);
    scrollArea->setLayout(middleInnerVLayout);
    QLabel* tempTitle = new QLabel("<span style='font-size: 20px;'>Title</span>", middleInnerWidget);
    tempTitle->setAlignment(Qt::AlignTop);
    middleInnerVLayout->addWidget(tempTitle);

    artwork = new QWebView(middleInnerWidget);
    artwork->load(QUrl("file:///home/sandeep/.moviemanager/tt0000000.jpg"));
    artwork->setMinimumSize(320,474);
    artwork->setMaximumSize(320,474);
    middleInnerVLayout->addWidget(artwork);


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

void moviemanager::removeAllItems(KListWidget *list)
{
    while(list->count() != 0)
    {
        qDebug() << "Removing: " << list->itemAt(0,0)->text();
        delete list->itemAt(0,0);
    }
}

void moviemanager::queryGenre(QString gen)
{
    //building the term
     term = term && Nepomuk::Query::ComparisonTerm(Nepomuk::Vocabulary::NMM::genre(), Nepomuk::Query::LiteralTerm(QLatin1String(gen.toLatin1().data())));
}

void moviemanager::queryActor(QString act)
{
    //building the term
     term = term && Nepomuk::Query::ComparisonTerm(Nepomuk::Vocabulary::NMM::actor(), Nepomuk::Query::LiteralTerm(QLatin1String(act.toLatin1().data())));
}

void moviemanager::queryTag(QString userTag)
{
    //This module is not working
    //get help from pnh for debugging
    //pnh code
    Nepomuk::Tag myTag;
    myTag.setLabel(userTag);
    QString query
       = QString("select distinct ?r where { ?r %1 %2 . ?r a %3 }")
         .arg( Soprano::Node::resourceToN3(Soprano::Vocabulary::NAO::hasTag()) )
         .arg( Soprano::Node::resourceToN3(myTag.resourceUri()) )
         .arg( Soprano::Node::resourceToN3(Nepomuk::Vocabulary::NFO::Video()) );

    QList<Nepomuk::Resource> myResourceList;
    Soprano::Model *model = Nepomuk::ResourceManager::instance()->mainModel();

    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );
    while( it.next() ) {
        qDebug() << "looping";
       myResourceList << Nepomuk::Resource( it.binding( "r" ).uri() );
    }

    Q_FOREACH (const Nepomuk::Resource& r, myResourceList)
    {
        mainMovieList->addItem(r.property(Nepomuk::Vocabulary::NFO::fileName()).toString());
        //if(r.tags().contains(new Nepomuk:Tag("video"))) newList.append(r)
    }
    //pnh code
}

//All slot definitions
void moviemanager::slotSearchBarReturnPressed(QString userInput)
{
    qDebug() << "search bar return pressed";
    qDebug() << "user entered: " << userInput;

    //remove all items in the list
    removeAllItems(mainMovieList);

    term =  Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Video());
    queryActor(userInput);
    executeQuery();

}

void moviemanager::executeQuery()
{
    Nepomuk::Query::Query currentQuery;
    currentQuery.setTerm(term);
    QList<Nepomuk::Query::Result> results = Nepomuk::Query::QueryServiceClient::syncQuery( currentQuery );

    //This is not an efficient way, check pnh's code and optimize this code
    Q_FOREACH( const Nepomuk::Query::Result& result,results)
    {
        if(result.resource().property(Nepomuk::Vocabulary::NIE::title()).toString().isEmpty() == true)
        {
            //showing filename
            mainMovieList->addItem(result.resource().property(Nepomuk::Vocabulary::NFO::fileName()).toString().remove(".avi"));
        }
        else
        {
            //showing movie title
            mainMovieList->addItem(result.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());
        }
    }
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
