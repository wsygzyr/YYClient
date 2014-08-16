#ifndef SCORE_H
#define SCORE_H

class Score
{
public:
    Score();
    Score(float pitchScore,
          float lyricScore,
          float rhythmScore,
          float totalScore,
          int sentenceIndex);

private:
    int   sentenceIndex;
    float pitchScore;
    float lyricScore;
    float rhythmScore;
    float totalScore;    //0.7*pitchScore+0.2*rhythmScore+0.1*lyricScore
    float propScore;

public:
    int getSentenceIndex() const;
    void setSentenceIndex(int);

    float getPitchScore() const;
    void setPitchScore(float);

    float getLyricScore() const;
    void setLyricScore(float);

    float getRhythmScore() const;
    void setRhythmScore(float);

    float getTotalScore() const;
    void setTotalScore(float);

    float getPropScore() const;
    void setPropScore(float value);
};

#endif // SCORE_H
