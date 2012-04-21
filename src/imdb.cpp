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
    qDebug() << "response received successfully for: " << *(this->movieName);
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

}



IMDB::~IMDB()
{
}
