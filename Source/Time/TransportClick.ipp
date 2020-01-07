/*
  ==============================================================================

    TransportClick.h
    Created: 11 Oct 2019 2:35:35pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once


void TimeManager::processClick(float** outputChannelData,
                               int numOutputChannels,
                               int numSamples){
    if (click->boolValue() && timeState.isPlaying && timeState.time >= 0)
    {
        static sample_clk_t sinCount = 0;

        bool isFirstBeat = (getClosestBeat() % beatPerBar->intValue()) == 0;
        const int sinFreq = sampleRate / (isFirstBeat ? 1320 : 880);

        if (_desiredTimeState.isJumping)
        {
            clickFader->startFadeOut();
        }

        double cVol = float01ToGain (clickVolume->floatValue());

        for (int i = 0 ; i < numSamples; i++)
        {

            double x = (fmod (getBeatInNextSamples (i), 1.0) ) * beatTimeInSample * 1.0 / sampleRate ;
            double h = jmax (0.0, jmin (1.0, (0.02 - x) / 0.02));

            if ((timeState.time + i) % beatTimeInSample == 0)
            {
                sinCount = 0;
            }

            double carg = sinCount * 1.0 / sinFreq;



            clickFader->incrementFade();
            double cFade = clickFader->getCurrentFade();

            double env = cVol * cFade * jmax (0.0, h); //*exp(1.0-h));
            // jassert(env < 1.0);
            float res = ( env * (sin (2.0 * M_PI * carg ) + 0.1 * sin (2.0 * M_PI * 4.0 * carg )));

            for (int c = 0 ; c < numOutputChannels ; c++ ) {outputChannelData[c][i] = res;}

            sinCount = (sinCount + 1) % (sinFreq);
            //      DBG(clickFader->getCurrentFade());

        }
    }
    else
    {
        for (int i = 0; i < numOutputChannels; ++i)
            zeromem (outputChannelData[i], sizeof (float) * (size_t) numSamples);
    }

}
