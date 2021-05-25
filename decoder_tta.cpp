#include "ttahelper.h"
#include "decoder_tta.h"

DecoderTTA::DecoderTTA(const QString &path)
    : Decoder()
{
    m_helper = new TTAHelper(path);
}

DecoderTTA::~DecoderTTA()
{
    delete m_helper;
}

bool DecoderTTA::initialize()
{
    if(!m_helper->initialize())
    {
        qWarning("DecoderTTA: initialize failed");
        return false;
    }

    const int rate = m_helper->sampleRate();
    const int channels = m_helper->channels();
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
    return m_helper->totalTime();
}

int DecoderTTA::bitrate() const
{
    return m_helper->bitrate();
}

qint64 DecoderTTA::read(unsigned char *data, qint64 size)
{
    return m_helper->read(data, size);
}

void DecoderTTA::seek(qint64 pos)
{
    m_helper->seek(pos);
}
