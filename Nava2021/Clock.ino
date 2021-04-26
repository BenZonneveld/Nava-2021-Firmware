//-------------------------------------------------
//                  NAVA v1.x
//                  BPM
//-------------------------------------------------

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

//      unsigned long micronow = micros();
//      unsigned long period = micronow;
      // Send MIDI notes for the playing instruments
      for(int inst=0 ; inst < NBR_INST ; inst++ )
      {
        if ( bitRead(pattern[ptrnBuffer].inst[inst], curStep) & bitRead(~muteInst,inst) )
        {
          if (inst >= 14 && bitRead(muteInst,5)) continue;
          if (instMidiNote[inst] != 0 )
          {
            unsigned int MIDIVelocity = map(pattern[ptrnBuffer].velocity[inst][curStep],instVelLow[inst],instVelHigh[inst],64,95);
            unsigned int MIDIAccent = map(((bitRead(pattern[ptrnBuffer].inst[TOTAL_ACC], curStep)) ? (pattern[ptrnBuffer].totalAcc * 4) : 0),0,52,0,32);
//            Serial.print("Accent: ");
//            Serial.println(MIDIAccent);
//            Serial.print("Velocity: ");
//            Serial.println(MIDIVelocity);
            MidiSendNoteOn(seq.TXchannel,instMidiNote[inst]-12,MIDIVelocity+MIDIAccent);
          }
        }
      }
      if (bitRead(pattern[ptrnBuffer].inst[TRIG_OUT], curStep)) TRIG_LOW;//Trigout

      //Trig external instrument-------------------------------------
      if (bitRead(pattern[ptrnBuffer].inst[EXT_INST], curStep))
      {
        InitMidiNoteOff();
        MidiSendNoteOn(seq.EXTchannel, pattern[ptrnBuffer].extNote[noteIndexCpt], HIGH_VEL);
        midiNoteOnActive = TRUE;
        noteIndexCpt++;//incremente external inst note index
      }
      if (noteIndexCpt > pattern[ptrnBuffer].extLength){
        noteIndexCpt = 0;
      }

      /* As this delay is in the interrupt routine it doesn't need to be replaced by a non blocking version */
//      while ( period < micronow + 2000)
//      {
//        period = micros();
//      }
      delayMicroseconds(2000);
      for(int inst=0 ; inst < NBR_INST ; inst++ )
      {
        if ( bitRead(pattern[ptrnBuffer].inst[inst], curStep)  & bitRead(~muteInst,inst) )
        {
          if (inst >= 14 && bitRead(muteInst,5)) continue;
          if (instMidiNote[inst] != 0 )
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
