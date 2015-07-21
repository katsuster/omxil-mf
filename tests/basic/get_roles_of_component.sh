#!/bin/sh

set -xe

TEST_NAME=get_roles_of_component

#./${TEST_NAME} OMX.st.video_decoder.avc
#./${TEST_NAME} OMX.st.video_decoder.mpeg4
#./${TEST_NAME} OMX.st.video_decoder.h263
#./${TEST_NAME} OMX.st.audio_decoder.aac
#./${TEST_NAME} OMX.st.audio_decoder.mp3
#./${TEST_NAME} OMX.st.audio_decoder.vorbis
#./${TEST_NAME} OMX.MF.reader.zero
#./${TEST_NAME} OMX.MF.renderer.null
./${TEST_NAME} OMX.MF.filter.copy
