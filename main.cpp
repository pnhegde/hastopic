#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QStringList>
#include <QHash>
#include <QHashIterator>
#include <QListIterator>
#include <Nepomuk/File>
#include <Nepomuk/Resource>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QFile fileSW("/home/pnh/kde/src/qthash/stopwords.txt");

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
    QString filePath(argv[1]);
    QFile file(filePath);
    QTextStream in(&file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open";
    }

    while (!in.atEnd()) {
        QString line = in.readLine();
        list = line.split(" ");
        foreach(QString str,list) {
            if(!stopwords.contains(str)) {
                if(hash.value(str))
                    hash[str] = hash[str]+1;
                else
                    hash[str] = 1;
            }
        }
    }

    QHashIterator<QString, int> l(hash);
    while (l.hasNext()) {
        l.next();
        qDebug()<< l.key() << ": " << l.value() << endl;
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
    qDebug()<<string;

    Nepomuk::File f((KUrl)filePath);
    Nepomuk::Resource topic(string[0]);
    topic.addType(Nepomuk::Vocabulary::PIMO::Topic());
    f.addTopic(topic);
    return a.exec();
}
