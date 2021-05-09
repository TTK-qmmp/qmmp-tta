#include "ttahelper.h"
#include "decoder_tta.h"

DecoderTTA::DecoderTTA(const QString &path)
    : Decoder()
{
    m_tta = new TTAHelper(path);
}

DecoderTTA::~DecoderTTA()
{
    delete m_tta;
}

bool DecoderTTA::initialize()
{
    if(!m_tta->initialize())
    {
        qWarning("DecoderTTA: initialize failed");
        return false;
    }

    const int rate = m_tta->sampleRate();
    const int channels = m_tta->channels();
    if(rate == 0 || channels == 0)
    {
        qWarning("DecoderTTA: rate or channel invalid");
        return false;
    }

    configure(rate, channels, Qmmp::PCM_S16LE);
    qDebug("DecoderTTA: initialize succes");
    return true;
}

qint64 DecoderTTA::totalTime() const
{
    return m_tta->totalTime();
}

int DecoderTTA::bitrate() const
{
    return m_tta->bitrate();
}

qint64 DecoderTTA::read(unsigned char *data, qint64 size)
{
    return m_tta->read(data, size);
}

void DecoderTTA::seek(qint64 pos)
{
    m_tta->seek(pos);
}
