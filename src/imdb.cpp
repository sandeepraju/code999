/***************************************************************************
 *   Copyright (C) 2012 by Sandeep Raju P <sandeep080@gmail.com>           *
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

//user defined includes
#include "imdb.h"
#include "qdownloader.h"
#include "nfo/image.h"
#include "nfo/webdataobject.h"
#include "nco/contact.h"

//Qt includes
#include <Qt>
#include <QDebug>
#include <QVariant>
#include <QDate>

//Nepomuk includes
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NMM>

//KDE includes
#include <KListWidget>


IMDB::IMDB(Nepomuk::Resource movieResource, KListWidget* list)
{
    this->movieName = new QString(movieResource.property(Nepomuk::Vocabulary::NFO::fileName()).toString().remove(".avi"));
    this->baseURL = new QString("http://www.imdbapi.com/?t=");
    this->mResource = movieResource;

    qDebug() << "fetching data for: " << *(this->movieName);
}

void IMDB::getData()
{
    //getting data
    QNetworkAccessManager* pageLoader = new QNetworkAccessManager();

    connect(pageLoader, SIGNAL(finished(QNetworkReply*)),this, SLOT(successResponse(QNetworkReply*)));

    QString* finalURL = new QString(this->baseURL->toLatin1().data());
    finalURL->append(this->movieName);

    pageLoader->get(QNetworkRequest(QUrl(*(finalURL))));
}

void IMDB::successResponse(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    this->responseJSON = new QString(data);

    parser = new QJson::Parser();
    bool ok;

    parsedResponse = parser->parse(this->responseJSON->toLatin1(), &ok).toMap();
    if (!ok) {
      qFatal("An error occured during parsing");
      exit (1);
    }

    //The response will be present in parsedResponse
    //download the poster
    //add parsed info into its particular fields
    //think about adding QList of actors

    qDebug() << "|" << parsedResponse["Response"].toString() << "|";
    if(parsedResponse["Response"].toString().contains("True") == true)
    {
        //response got successfully
        qDebug() << "response got successfully for: " << *(this->movieName);
        //example of response data if successful
        /*{"Title":"Avatar",
          "Year":"2009",
          "Rated":"PG-13",
          "Released":"18 Dec 2009",
          "Genre":"Action, Adventure, Fantasy, Sci-Fi",
          "Director":"James Cameron",
          "Writer":"James Cameron",
          "Actors":"Sam Worthington, Zoe Saldana, Sigourney Weaver, Michelle Rodriguez",
          "Plot":"A paraplegic marine dispatched to the moon Pandora on a unique mission becomes torn between following his orders and protecting the world he feels is his home.",
          "Poster":"http://ia.media-imdb.com/images/M/MV5BMTYwOTEwNjAzMl5BMl5BanBnXkFtZTcwODc5MTUwMw@@._V1_SX320.jpg",
          "Runtime":"2 hrs 42 mins",
          "Rating":"8.1",
          "Votes":"386930",
          "ID":"tt0499549",
          "Response":"True"}*/

        //adding Title
        this->mResource.setProperty(Nepomuk::Vocabulary::NIE::title(),parsedResponse["Title"].toString());

        //adding Date
        //if release date not available, set year.
        QString* tempDate = new QString(parsedResponse["Released"].toString());
        qDebug() << *tempDate;
        //qDebug() << (tempDate->mid(0,(tempDate->length() - 2)));
        QStringList dateList = tempDate->split(" ");
        qDebug() << dateList.length();
        QString tempDay = (QString)dateList[0];
        QString tempMonth = (QString)dateList[1];
        QString tempYear = (QString)dateList[2];
        qDebug() << tempDay << tempMonth << tempYear;
        bool ok;
        //QDate* rDate = new QDate(tempDay.toInt(&ok),monthNumber(tempMonth),tempYear.toInt(&ok));
        //QDate* rDate = new QDate(11,11,2011);
        //this->mResource.setProperty(Nepomuk::Vocabulary::NMM::releaseDate(),rDate);
        //this->mResource.setProperty(Nepomuk::Vocabulary::NMM::releaseDate(),new QDate(2009,5,8));
        //fix this^^^ release date issue

        //adding Genre
        QString* tempGenre = new QString(parsedResponse["Genre"].toString());
        QStringList genreStringList = tempGenre->split(",");
        this->mResource.setProperty(Nepomuk::Vocabulary::NMM::genre(),genreStringList);

        //adding Director
        QString* tempDirector = new QString(parsedResponse["Director"].toString());
        QStringList directorStringList = tempDirector->split(",");
        QList<Nepomuk::Resource> directorList;
        Q_FOREACH( const QString& director,directorStringList)
        {
            Nepomuk::Resource con;
            con.addType(Nepomuk::Vocabulary::NCO::Contact());
            con.setLabel(director);
            directorList.append(con);
        }
        this->mResource.setProperty(Nepomuk::Vocabulary::NMM::director(),directorList);

        //adding Writer
        QString* tempWriter = new QString(parsedResponse["Writer"].toString());
        QStringList writerStringList = tempWriter->split(",");
        QList<Nepomuk::Resource> writerList;
        Q_FOREACH( const QString& writer,writerStringList)
        {
            Nepomuk::Resource con;
            con.addType(Nepomuk::Vocabulary::NCO::Contact());
            con.setLabel(writer);
            writerList.append(con);
        }
        this->mResource.setProperty(Nepomuk::Vocabulary::NMM::writer(),writerList);

        //adding Actor
        QString* tempActor = new QString(parsedResponse["Actors"].toString());
        QStringList actorStringList = tempActor->split(",");
        QList<Nepomuk::Resource> actorList;
        Q_FOREACH( const QString& actor,actorStringList)
        {
            Nepomuk::Resource con;
            con.addType(Nepomuk::Vocabulary::NCO::Contact());
            con.setLabel(actor);
            actorList.append(con);
        }
        this->mResource.setProperty(Nepomuk::Vocabulary::NMM::actor(),actorList);

        //adding Plot
        //Temporarily storing in comment... later will change it with synopsis
        this->mResource.setProperty(Nepomuk::Vocabulary::NIE::description(),parsedResponse["Plot"].toString());

        //adding artwork(Poster)
//        QDownloader* downloader = new QDownloader("/home/sandeep/.moviemanager/", parsedResponse["ID"].toString() + ".jpg");
//        downloader->setFile(parsedResponse["Poster"].toString());
//        Nepomuk::Resource tempImage = Nepomuk::Resource("/home/sandeep/.moviemanager/"+parsedResponse["ID"].toString() +".jpg");
//        tempImage.addType(Nepomuk::Vocabulary::NFO::Image());
//        this->mResource.setProperty(Nepomuk::Vocabulary::NMM::artwork(),tempImage);

        //adding rating (G, U/A, PG-13, 18, AV)
        this->mResource.setProperty(Nepomuk::Vocabulary::NMM::audienceRating(),parsedResponse["Rated"].toString());

        //adding rating
        //adding temporarily...
        //semantically wrong...
        //this will be reimplemented later
        this->mResource.setProperty(Nepomuk::Vocabulary::NIE::comment(),parsedResponse["Rating"].toString());

        //adding ID
        this->mResource.setProperty(Nepomuk::Vocabulary::NMM::synopsis(),parsedResponse["ID"].toString());
    }
    else
    {
        //negative response
        //movie not found in the IMDB server database
    }

}



IMDB::~IMDB()
{
}

int IMDB::monthNumber(QString month)
{
    qDebug() << "month is : " << month;
    if(month.contains("Jan") ==true)
    {
        return 1;
    }
    if(month.contains("Feb") ==true)
    {
        return 2;
    }
    if(month.contains("Mar") ==true)
    {
        return 3;
    }
    if(month.contains("Apr") ==true)
    {
        return 4;
    }
    if(month.contains("May") ==true)
    {
        return 5;
    }
    if(month.contains("Jun") ==true)
    {
        return 6;
    }
    if(month.contains("Jul") ==true)
    {
        return 7;
    }
    if(month.contains("Aug") ==true)
    {
        return 8;
    }
    if(month.contains("Sep") ==true)
    {
        return 9;
    }
    if(month.contains("Oct") ==true)
    {
        return 10;
    }
    if(month.contains("Nov") ==true)
    {
        return 11;
    }
    if(month.contains("Dec") ==true)
    {
        return 12;
    }

    qDebug() << "something is wrong with the month string revieved!";
    return 0;
}
