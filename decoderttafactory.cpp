#include "ttahelper.h"
#include "decoder_tta.h"
#include "decoderttafactory.h"
#include "ttametadatamodel.h"

#include <QMessageBox>

bool DecoderTTAFactory::canDecode(QIODevice *) const
{
    return false;
}

DecoderProperties DecoderTTAFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("TTA Plugin");
    properties.filters << "*.tta";
    properties.description = tr("TTA True Audio Files");
    properties.shortName = "tta";
    properties.noInput = true;
    properties.protocols << "tta";
    return properties;
}

Decoder *DecoderTTAFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderTTA(path);
}

QList<TrackInfo*> DecoderTTAFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    TrackInfo *info = new TrackInfo(path);

    if(parts == TrackInfo::Parts())
    {
        return QList<TrackInfo*>() << info;
    }

    TTAHelper helper(path);
    if(!helper.initialize())
    {
        return QList<TrackInfo*>();
    }

    if(parts & TrackInfo::MetaData)
    {
        const QVariantMap &meta = helper.readTags(meta_all);
        info->setValue(Qmmp::ALBUM, meta.value("album", QString()));
        info->setValue(Qmmp::ARTIST, meta.value("artist", QString()));
        info->setValue(Qmmp::COMMENT, meta.value("comment", QString()));
        info->setValue(Qmmp::GENRE, meta.value("genre", QString()));
        info->setValue(Qmmp::TITLE, meta.value("title", QString()));
        info->setValue(Qmmp::YEAR, meta.value("year", QString()));
        info->setValue(Qmmp::TRACK, meta.value("track", QString()));
    }

    if(parts & TrackInfo::Properties)
    {
        info->setValue(Qmmp::BITRATE, helper.bitrate());
        info->setValue(Qmmp::SAMPLERATE, helper.sampleRate());
        info->setValue(Qmmp::CHANNELS, helper.channels());
        info->setValue(Qmmp::BITS_PER_SAMPLE, helper.bitsPerSample());
        info->setValue(Qmmp::FORMAT_NAME, "tta");
        info->setDuration(helper.totalTime());
    }

    return QList<TrackInfo*>() << info;
}

MetaDataModel* DecoderTTAFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    Q_UNUSED(readOnly);
    return new TTAMetaDataModel(path);
}

void DecoderTTAFactory::showSettings(QWidget *parent)
{
    Q_UNUSED(parent);
}

void DecoderTTAFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About TTA Reader Plugin"),
                        tr("Qmmp TTA Reader Plugin")+"\n"+
                        tr("Written by: Greedysky <greedysky@163.com>"));
}

QString DecoderTTAFactory::translation() const
{
    return QString();
}
