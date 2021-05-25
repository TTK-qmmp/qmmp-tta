#include "ttametadatamodel.h"

TTAMetaDataModel::TTAMetaDataModel(const QString &path) :
    MetaDataModel(true)
{
    m_helper = new TTAHelper(path);
    m_tags << new TTAFileTagModel(m_helper, meta_id3v1);
    m_tags << new TTAFileTagModel(m_helper, meta_id3v2);
    m_tags << new TTAFileTagModel(m_helper, meta_apev2);
}

TTAMetaDataModel::~TTAMetaDataModel()
{
    while(!m_tags.isEmpty())
        delete m_tags.takeFirst();
    delete m_helper;
}

QList<TagModel* > TTAMetaDataModel::tags() const
{
    return m_tags;
}


TTAFileTagModel::TTAFileTagModel(TTAHelper* tta, stdio_meta_type stdio_meta)
    : TagModel()
{
    m_helper = tta;
    m_stdio_meta = stdio_meta;
}

TTAFileTagModel::~TTAFileTagModel()
{}

QString TTAFileTagModel::name() const
{
    if(m_stdio_meta == meta_id3v1)
        return "ID3v1";
    else if(m_stdio_meta == meta_id3v2)
        return "ID3v2";
    return "APE";
}

QList<Qmmp::MetaData> TTAFileTagModel::keys() const
{
    QList<Qmmp::MetaData> list = TagModel::keys();
    if(m_stdio_meta == meta_id3v2)
        return list;
    else if(m_stdio_meta == meta_apev2)
    {
        list.removeAll(Qmmp::DISCNUMBER);
        return list;
    }
    list.removeAll(Qmmp::COMPOSER);
    list.removeAll(Qmmp::ALBUMARTIST);
    list.removeAll(Qmmp::DISCNUMBER);
    return list;
}

QString TTAFileTagModel::value(Qmmp::MetaData key) const
{
    if(m_helper)
    {
        const QVariantMap &meta = m_helper->readTags(m_stdio_meta);
        switch((int) key)
        {
        case Qmmp::TITLE: return meta.value("title", QString()).toString();
        case Qmmp::ARTIST: return meta.value("artist", QString()).toString();
        case Qmmp::ALBUMARTIST: return meta.value("albumartist", QString()).toString();
        case Qmmp::ALBUM: return meta.value("album", QString()).toString();
        case Qmmp::COMMENT: return meta.value("comment", QString()).toString();
        case Qmmp::GENRE: return meta.value("genre", QString()).toString();
        case Qmmp::COMPOSER: return meta.value("COMPOSER", QString()).toString();
        case Qmmp::YEAR: return meta.value("year", QString()).toString();
        case Qmmp::TRACK: return meta.value("track", QString()).toString();
        case Qmmp::DISCNUMBER: return meta.value("TPOS", QString()).toString();
        }
    }
    return QString();
}

void TTAFileTagModel::setValue(Qmmp::MetaData , const QString &)
{
}
