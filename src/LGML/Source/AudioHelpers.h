/*
 ==============================================================================

 AudioHelpers.h
 Created: 8 Mar 2016 12:33:13pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef AUDIOHELPERS_H_INCLUDED
#define AUDIOHELPERS_H_INCLUDED


#define DB0_FOR_01 0.8f
// create a gain value for a float  between 0 and 1
// DB0_FOR_01   -> 0dB
// 1            -> +6dB
inline float float01ToGain(float f){
    if(f==0)return 0;
    float minus6Pos = (1-2*(1-DB0_FOR_01));
    if(f>minus6Pos) return Decibels::decibelsToGain(jmap<float>(f,DB0_FOR_01,1.0f,0.0f,6.0f));
    return Decibels::decibelsToGain(jmap<float>(f,0,minus6Pos,-70.0f,-6.0f));
};

inline float rmsToGain01(float rms){
        return jmap<float>(20.0f*log10(rms/0.74f),0.0f,6.0f,DB0_FOR_01,1.0f);
}






#endif  // AUDIOHELPERS_H_INCLUDED
