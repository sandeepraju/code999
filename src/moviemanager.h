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

#ifndef moviemanager_H
#define moviemanager_H

//user defined includes
#include "imdb.h"

//Qt includes
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListView>
#include <QUrl>
#include<QScrollArea>
#include <QListView>
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
#include <QToolButton>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QModelIndexList>

//KDE includes
#include <KXmlGuiWindow>
#include <KDE/KApplication>
#include <KLineEdit>
#include <KPushButton>
#include <KListWidget>

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
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/ResourceTerm>
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

class QPrinter;
class KToggleAction;
class KUrl;

class moviemanager : public KXmlGuiWindow
{
    Q_OBJECT
//public methods
public:
    moviemanager();
    virtual ~moviemanager();

//private methods
private:
    void setupUserInterface();
    void fetchAllMovieList(KListWidget*);
    void fetchRecoMovieList(KListWidget*);

//private members
private:
    QWidget* mainWindow;
    QHBoxLayout* mainHLayout;
    QVBoxLayout* firstVLayout;
    QVBoxLayout* secondVLayout;
    QVBoxLayout* thirdVLayout;
    KLineEdit* searchBar;
    KListWidget* mainMovieList;
    //secondVLayout widgets here

    KListWidget* recoMovieList;


//private slots
private slots:
    void slotSearchBarReturnPressed(QString);
    void slotMovieClicked(QModelIndex);

};

#endif // _moviemanager_H_
