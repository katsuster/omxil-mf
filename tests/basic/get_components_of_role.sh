#!/bin/sh

set -xe

TEST_NAME=get_components_of_role

#./${TEST_NAME} video_decoder.avc
#./${TEST_NAME} video_decoder.mpeg4
#./${TEST_NAME} video_decoder.h263
#./${TEST_NAME} audio_decoder.aac
#./${TEST_NAME} audio_decoder.mp3
#./${TEST_NAME} audio_decoder.vorbis
./${TEST_NAME} audio_reader.zero
./${TEST_NAME} audio_renderer.null
./${TEST_NAME} audio_filter.copy
