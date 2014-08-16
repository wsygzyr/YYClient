#pragma once
#define SAMPLE_22K 22050
#define SAMPLE_44K 44100
#define SAMPLE_48K 48000

typedef struct _ScoreResponse
{
	int   retCode;
	float pitchScore;       //音高打分
	float lyricScore;       //歌词打分
	float rhythmScore;      //节奏打分
	float sentenceScore;    //0.7*pitchScore+0.2*rhythmScore+0.1*lyricScore，非单句得分时为-1.00
	float elecWavScore;     //电音得分，缺省为-1.00
	float sexScore;         //男女声似然度，缺省为-1000.00
	int   sentenceIndex;
}ScoreResponse;

int SESInit(char* xmlData, int dataLen , char* tmpPath, int sampleRate , float timeOffset , int indexOffset , char* keyWord);

int SESInsertPcm(short* pcmData,unsigned int dataLen ,void* usrData);

void SESUnInit();

typedef void(*SESCallBack)(void* response,void* usrData);

void SetGetSTScoreErrorCallBack(SESCallBack);
void SetGet3DOnlineScoreCallBack(SESCallBack);
