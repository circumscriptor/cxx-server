/**
 *
 * SpadesServer
 *
 */

#include "Compress.hxx"

#include <iostream>
#include <zlib.h>

Spades::Compressor::Compressor(int level)
{
    stream         = new z_stream;
    stream->zalloc = Z_NULL;
    stream->zfree  = Z_NULL;
    stream->opaque = Z_NULL;

    if (deflateInit(stream, level) < 0) {
        std::cerr << "failed to initialize compressor\n";
        mIsValid = false;
    }
}

Spades::Compressor::~Compressor()
{
    if (deflateEnd(stream) != 0) {
        std::cerr << "failed to close compressor\n";
    }
    delete stream;
}

Spades::DataChunk* Spades::Compressor::Compress(void* data, uint32 length, uint32 chunkSize)
{
    if (!mIsValid) {
        return nullptr;
    }

    DataChunk* first;
    DataChunk* chunk;

    stream->next_in  = (uint8*) data;
    stream->avail_in = length;

    do {
        if (first == NULL) {
            first = new DataChunk(chunkSize);
            chunk = first;
        } else {
            chunk = chunk->Push(chunkSize);
        }

        stream->avail_out = chunkSize;
        stream->next_out  = chunk->mChunk;
        if (deflate(stream, Z_FINISH) < 0) {
            std::cerr << "failed to compress chunk\n";
            while (first) {
                first = first->Pop();
            }
            return nullptr;
        }
        chunk->mLength = chunkSize - stream->avail_out;
    } while (stream->avail_out == 0);

    return first;
}
