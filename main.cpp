/*
 ***************************************************************************
 *   Copyright (C) 2011 by Phaneendra Hegde <phaneendra.hegde@gmail.com>   *
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
 ***************************************************************************
*/

//Qt Includes
#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QStringList>
#include <QHash>
#include <QHashIterator>
#include <QListIterator>

//Nepomuk Includes
#include <Nepomuk/File>
#include <Nepomuk/Resource>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Variant>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QFile fileSW("/home/pnh/kde/src/hastopic/stopwords.txt");

    if (!fileSW.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open";
    }
    QStringList list,stopwords;
    QHash<QString,int>hash;

    QTextStream sw(&fileSW);
    while(!sw.atEnd()) {
        QString line = sw.readLine();
        stopwords += line.split(" ");
    }
//    QString filePath(argv[1]);
//    QFile file(filePath);
//    QTextStream in(&file);

//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        qDebug()<<"Can't open";
//    }

    Nepomuk::Query::Query query;
    query.setLimit(10);
    Nepomuk::Query::Term term;
    term = Nepomuk::Query::ResourceTypeTerm(Nepomuk::Vocabulary::NFO::PaginatedTextDocument()) ||
                                                          Nepomuk::Query::ComparisonTerm(Nepomuk::Vocabulary::NIE::mimeType(),
                                                                       Nepomuk::Query::LiteralTerm(QLatin1String("application/pdf")));
    query.setTerm(term);
    QList<Nepomuk::Query::Result>results = Nepomuk::Query::QueryServiceClient::syncQuery( query );

    Q_FOREACH( const Nepomuk::Query::Result& result, results ) {
        if(result.resource().topics().length() <=3 ) {
            Nepomuk::Variant v = result.resource().property(Nepomuk::Vocabulary::NIE::plainTextContent());
            QString line = v.toString();
            list = line.split(" ");
            foreach(QString str,list) {
                if(!stopwords.contains(str)) {
                    if(hash.value(str))
                        hash[str] = hash[str]+1;
                    else
                        hash[str] = 1;
                }
            }

            QHashIterator<QString, int> l(hash);
            while (l.hasNext()) {
                l.next();
            }

            QList<int> vals = hash.values();
            QSet<int>set = vals.toSet();
            vals= set.toList();
            qSort(vals);
            QStringList string;
            while(string.length()<5) {
                string = string+hash.keys(vals.last());
                vals.removeLast();
            }
            qDebug()<<string << "File name:->>>>" << result.resource().genericLabel();

            for(int i=0;i<3;i++) {
              Nepomuk::Resource topic(string[i]);
              topic.addType(Nepomuk::Vocabulary::PIMO::Topic());
              result.resource().addTopic(topic);
            }
        }
    }



    return a.exec();
}
