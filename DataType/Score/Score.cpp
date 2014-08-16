#include "Score.h"

Score::Score()
{

}

Score::Score(float pitchScore,
            float lyricScore,
            float rhythmScore,
            float totalScore,
            int sentenceIndex)
{
    this->pitchScore        = pitchScore;
    this->lyricScore        = lyricScore;
    this->rhythmScore       = rhythmScore;
    this->totalScore        = totalScore;
    this->sentenceIndex     = sentenceIndex;
}

float Score::getPropScore() const
{
    return propScore;
}

void Score::setPropScore(float value)
{
    propScore = value;
}

int Score::getSentenceIndex() const
{
    return this->sentenceIndex;
}

void Score::setSentenceIndex(int sentenceIndex)
{
    this->sentenceIndex = sentenceIndex;
}

float Score::getPitchScore() const
{
    return this->pitchScore;
}

void Score::setPitchScore(float pitchScore)
{
    this->pitchScore = pitchScore;
}

float Score::getLyricScore() const
{
    return this->lyricScore;
}

void Score::setLyricScore(float lyricScore)
{
    this->lyricScore = lyricScore;
}

float Score::getRhythmScore() const
{
    return this->rhythmScore;
}

void Score::setRhythmScore(float rhythmScore)
{
    this->rhythmScore = rhythmScore;
}

float Score::getTotalScore() const
{
    return this->totalScore;
}

void Score::setTotalScore(float totalScore)
{
    this->totalScore = totalScore;
}


