#include "ttahelper.h"

extern "C" {
#include "stdio_meta.h"
}

#define MAX_BSIZE (MAX_BPS>>3)

TTAHelper::TTAHelper(const QString &url)
{
    m_path = url;
    m_info = (decode_info*)calloc(sizeof(decode_info), 1);
    m_info->buffer = (char*)malloc(sizeof(char) * PCM_BUFFER_LENGTH * MAX_BSIZE * MAX_NCH);
}

TTAHelper::~TTAHelper()
{
    deinit();
}

void TTAHelper::deinit()
{
    if(m_info)
    {
        free(m_info->buffer);
        player_stop(&m_info->input);
        close_tta_file(&m_info->input);
        free(m_info);
    }
}

bool TTAHelper::initialize()
{
    if(open_tta_file(m_path.toLocal8Bit().constData(), &m_info->input, 0) != 0)
    {
        qWarning("TTAHelper: open_tta_file failed");
        return false;
    }

    if(player_init(&m_info->input) != 0)
    {
        qWarning("TTAHelper: player_init invalid");
        return false;
    }

    m_info->end_sample = m_info->input.DATALENGTH - 1;

    return true;
}

int TTAHelper::totalTime() const
{
    return m_info->input.LENGTH * 1000;
}

void TTAHelper::seek(qint64 time)
{
    const int total = totalTime();
    if(total == 0 || m_info->end_sample == 0)
    {
        return;
    }

    const int sample = time * 1.0 / total * m_info->end_sample;
    m_info->samples_to_skip = set_position(&m_info->input, sample);
    if(m_info->samples_to_skip < 0)
    {
        return;
    }

    m_info->current_sample = sample;
    m_info->remaining = 0;
}

int TTAHelper::bitrate() const
{
    return m_info->input.BITRATE;
}

int TTAHelper::sampleRate() const
{
    return m_info->input.SAMPLERATE;
}

int TTAHelper::channels() const
{
    return m_info->input.NCH;
}

int TTAHelper::bitsPerSample() const
{
    return m_info->input.BPS;
}

int TTAHelper::read(unsigned char *buf, int size)
{
    int samplesize = channels() * bitsPerSample() / 8;
    if(m_info->current_sample + size / samplesize > m_info->end_sample)
    {
        size = (m_info->end_sample - m_info->current_sample + 1) * samplesize;
        if(size <= 0)
        {
            return 0;
        }
    }

    int initsize = size;
    while(size > 0)
    {
        if(m_info->samples_to_skip > 0 && m_info->remaining > 0)
        {
            int skip = MIN(m_info->remaining, m_info->samples_to_skip);
            if(skip < m_info->remaining)
            {
                memmove(m_info->buffer, m_info->buffer + skip * samplesize, (m_info->remaining - skip) * samplesize);
            }
            m_info->remaining -= skip;
            m_info->samples_to_skip -= skip;
        }

        if(m_info->remaining > 0)
        {
            int n = size / samplesize;
            n = MIN(n, m_info->remaining);
            int nn = n;
            char *p = m_info->buffer;

            memcpy(buf, p, n * samplesize);
            buf += n * samplesize;
            size -= n * samplesize;
            p += n * samplesize;

            if(m_info->remaining > nn)
            {
                memmove(m_info->buffer, p, (m_info->remaining - nn) * samplesize);
            }
            m_info->remaining -= nn;
        }

        if(size > 0 && !m_info->remaining)
        {
            m_info->remaining = get_samples(&m_info->input, m_info->buffer);
            if(m_info->remaining <= 0)
            {
                break;
            }
        }
    }

    m_info->current_sample += (initsize - size) / samplesize;
    return initsize - size;
}

QVariantMap TTAHelper::readTags(stdio_meta_type stdio_meta)
{
    FILE *file = fopen(m_path.toLocal8Bit().constData(), "rb");
    if(file == nullptr)
    {
        return QVariantMap();
    }

    metaInfo_t *item = (metaInfo_t*)calloc(sizeof(metaInfo_t), 1);
    item->key = ":MODULE";
    item->value = "TTA";

    switch(stdio_meta)
    {
        case meta_apev2: stdio_apev2_read(item, nullptr, file); break;
        case meta_id3v2: stdio_id3v2_read(item, nullptr, file); break;
        case meta_id3v1: stdio_id3v1_read(item, file); break;
        case meta_all:
        {
            stdio_apev2_read(item, nullptr, file);
            stdio_id3v2_read(item, nullptr, file);
            stdio_id3v1_read(item, file);
        }
    }

    fclose(file);

    QVariantMap data;
    while(item)
    {
        if(!item->key || !item->value)
        {
            item = item->next;
            continue;
        }

        data.insert(item->key, item->value);
        metaInfo_t *next = item->next;
        free(item);
        item = next;
    }

    return data;
}
