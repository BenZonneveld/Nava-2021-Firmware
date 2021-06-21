//-------------------------------------------------
//                  NAVA v1.x
//                  BPM
//-------------------------------------------------

byte instMidiNote[NBR_INST]={ 60, // TRIG_OUT
                              0, // HH_SLCT
                              50, // HT
                              37, // RM
                              39, // HC
                              0,  // HH
                              51, // RIDE
                              49, // CRASH
                              36, // BD
                              38, // SD
                              41, // LT
                              45, // MT
                              0,  // TOTAL_ACC
                              0,  // EXT_INST
                              42, // CH
                              46}; // OH
                              
/////////////////////Function//////////////////////
//Timer interrupt
ISR(TIMER1_COMPA_vect) { 
  CountPPQN(); 
}

//Tick proceed each pulse
void CountPPQN()
{
  blinkVeryFast =! blinkVeryFast;
  if (ppqn % (PPQN/2) == 0) blinkTempo = !blinkTempo;
  if (ppqn  % (pattern[ptrnBuffer].scale/2) == 0) blinkFast = LOW;
  if (ppqn % 4 == 0) MIDI.sendRealTime(Clock);//MidiSend(CLOCK_CMD);//as NAVA seq is 96ppqn we need to send clock each 4 internal ppqn

  if (seq.sync == MASTER){
    if (ppqn % 4 == 0){
      DIN_CLK_HIGH;
      dinClkState = HIGH;
    }
    else if (ppqn % 4 == 2) {
      DIN_CLK_LOW;
      dinClkState = LOW;
    }    
  }

  if (isRunning)
  {
    if (ppqn % pattern[ptrnBuffer].scale == (pattern[ptrnBuffer].scale/2)) tapStepCount++;

    if (tapStepCount > pattern[ptrnBuffer].length) tapStepCount = 0;
    
    if (ppqn % pattern[ptrnBuffer].scale == 0) stepChanged = TRUE;//Step changed ?

    if (((ppqn + shuffle[(pattern[ptrnBuffer].shuffle)-1][shufPolarity]) % pattern[ptrnBuffer].scale == 0) && stepChanged)
    {//Each Step
      stepChanged = FALSE;//flag that we already trig this step
      shufPolarity = !shufPolarity;
      blinkFast = HIGH;

      //sequencer direction-----------
      switch (seq.dir){
      case FORWARD:
        curStep = stepCount;
        break;
      case BACKWARD:
        curStep = pattern[ptrnBuffer].length - stepCount;
        break;
      case PING_PONG:
        if (curStep == pattern[ptrnBuffer].length && changeDir == 1) changeDir = 0;
        else if (curStep == 0 && changeDir == 0) changeDir = 1;
        if (changeDir) curStep = stepCount;
        else curStep = pattern[ptrnBuffer].length - stepCount;
        break;
      case RANDOM:
        curStep = random(0, 16);
        break;
      }

      SetMux();
      SetDoutTrig(((pattern[ptrnBuffer].step[curStep]) | (bitRead(metronome,curStep)<<RM)) & (~muteInst));//patternA[curStep]<<8 |  patternB[curStep]);

      // Send MIDI notes for the playing instruments
      for(int inst=0 ; inst < NBR_INST ; inst++ )
      {
        if ( bitRead(pattern[ptrnBuffer].inst[inst], curStep) & bitRead(~muteInst,inst) )
        {
          if (inst >= 14 && bitRead(muteInst,5)) continue;
          if (instMidiNote[inst] != 0 && pattern[ptrnBuffer].velocity[inst][curStep] > 0 )
          {
            unsigned int MIDIVelocity = (MIDI_LOW_VELOCITY * (pattern[ptrnBuffer].velocity[inst][curStep] == instVelLow[inst])) + 
                                        (MIDI_HIGH_VELOCITY * (pattern[ptrnBuffer].velocity[inst][curStep] == instVelHigh[inst]));
                                        
            if (bitRead(pattern[ptrnBuffer].inst[TOTAL_ACC], curStep)) MIDIVelocity = MIDI_ACCENT_VELOCITY;
          
            if (MIDIVelocity == 0 ) continue;
            MidiSendNoteOn(seq.TXchannel,instMidiNote[inst]-12,MIDIVelocity);
          }
        }
      }

      if (bitRead(pattern[ptrnBuffer].inst[TRIG_OUT], curStep)) TRIG_LOW;//Trigout

      InitMidiNoteOff();
      //Trig external instrument-------------------------------------
      if (bitRead(pattern[ptrnBuffer].inst[EXT_INST], curStep))
      {
        MidiSendNoteOn(seq.EXTchannel, pattern[ptrnBuffer].extNote[noteIndexCpt], MAX_VEL);
        midiNoteOnActive = TRUE;
        noteIndexCpt++;//incremente external inst note index
      }
      if (noteIndexCpt > pattern[ptrnBuffer].extLength){
        noteIndexCpt = 0;
      }

      /* As this delay is in the interrupt routine it doesn't need to be replaced by a non blocking version */
      delayMicroseconds(10000);
      for(int inst=0 ; inst < NBR_INST ; inst++ )
      {
        if ( bitRead(pattern[ptrnBuffer].inst[inst], curStep)  & bitRead(~muteInst,inst) )
        {
          if (inst >= 14 && bitRead(muteInst,5)) continue;
          if (instMidiNote[inst] != 0 && pattern[ptrnBuffer].velocity[inst][curStep] > 0)
          {
            MidiSendNoteOff(seq.TXchannel,instMidiNote[inst]-12);
          }
        }
      }
      if (bitRead(pattern[ptrnBuffer].inst[CH], curStep)) tempDoutTrig = B10;//CH trig
      if (bitRead(pattern[ptrnBuffer].inst[OH], curStep)) tempDoutTrig = 0;// OH trig
      SetDoutTrig(tempDoutTrig);
      TRIG_HIGH;
      //ResetDoutTrig();
      stepCount++;

    }
    if (stepCount > pattern[ptrnBuffer].length){// && (ppqn % 24 == pattern[ptrnBuffer].scale - 1))
      endMeasure = TRUE;
      trackPosNeedIncremante = TRUE;
      stepCount = 0;
      //In pattern play mode this peace of code execute in the PPQNCount function
      if(nextPatternReady && curSeqMode == PTRN_PLAY 
          && (( endMeasure && seq.patternSync ) 
          || !seq.patternSync  
          || !isRunning )
      ){
        nextPatternReady = FALSE;
        keybOct = DEFAULT_OCT;
        noteIndex = 0;
        InitMidiNoteOff();
        ptrnBuffer = !ptrnBuffer;
        InitPattern();//SHOULD BE REMOVED WHEN EEPROM WILL BE INITIALIZED
        SetHHPattern();
        InstToStepWord();
      }
    }
    if (ppqn % pattern[ptrnBuffer].scale == 4 && stepCount == 0){ 
      endMeasure = FALSE;
    }
  }

  if (ppqn++ >= PPQN ) ppqn = 1;
}

//
void Metronome(boolean state)
{
  if (state){
    metronome = 0x1111;//trig RM every time
    pattern[ptrnBuffer].velocity[RM][0] = 127;
    pattern[ptrnBuffer].velocity[RM][4] = 30;
    pattern[ptrnBuffer].velocity[RM][8] = 30;
    pattern[ptrnBuffer].velocity[RM][12] = 30;
  }
  else{
    metronome = 0;
    pattern[ptrnBuffer].velocity[RM][0] = HIGH_VEL;
    pattern[ptrnBuffer].velocity[RM][4] = HIGH_VEL;
    pattern[ptrnBuffer].velocity[RM][8] = HIGH_VEL;
    pattern[ptrnBuffer].velocity[RM][12] = HIGH_VEL;
  }
}
