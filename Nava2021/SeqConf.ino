//-------------------------------------------------
//                  NAVA v1.x
//                 SEQ configuration
//-------------------------------------------------

/////////////////////Function//////////////////////
void SeqConfiguration()
{
  if(seq.syncChanged){                               
    SetSeqSync();
    seq.syncChanged = FALSE;
  }
 
  if (seq.setupNeedSaved && enterBtn.justPressed && seq.configPage != 3 ){
    SaveSeqSetup();
    seq.setupNeedSaved = FALSE;
    LcdPrintSaved();
  }

#if MIDI_HAS_SYSEX
  // Transmit Midi System Exclusive
  if ( seq.configMode && seq.configPage == 3 && enterBtn.justPressed )
  {
    MidiSendSysex(sysExDump, sysExParam);
  }
#endif

  if (!seq.configMode) seq.setupNeedSaved = FALSE;

#if MIDI_HAS_SYSEX
  if ( seq.configPage == 3)
  { 
    if ( seq.SysExMode == false )
    {
      free (patternGroup);
      EnableSysexMode();
    }
  } else {
    if ( seq.SysExMode == true )
    {
      seq.SysExMode = false;
      patternGroup = (Pattern *)malloc(NBR_PATTERN * sizeof(Pattern));
      if ( patternGroup == NULL )
      {
        lcd.setCursor(0,1);
        lcd.print("Mem Alloc Error");
        while(1);
      }

      SetSeqSync();
    }
  }
#endif  
}

void SetSeqSync() 
{
  //Sync configuration
  switch (seq.sync){                             // [zabox] [1.028] added expander mode
  case MASTER: 
    initTrigTimer();                          
    DisconnectMidiHandleRealTime();
    DisconnectMidiHandleNote();
#if MIDI_HAS_SYSEX    
    DisconnectMidiSysex();
#endif    
    TimerStart();//cf timer
    break;
  case SLAVE:
    TimerStop();
    initTrigTimer();
#if MIDI_BANK_PATTERN_CHANGE    
    ConnectMidiHandleNote(); // Connects Notes but ignores drum notes.                       
#else    
    DisconnectMidiHandleNote();
#endif    
    ConnectMidiHandleRealTime();
#if MIDI_HAS_SYSEX    
    DisconnectMidiSysex();
#endif    
    break;
  case EXPANDER:
    TimerStop();
    initExpTimer();                 
    DisconnectMidiHandleRealTime();
    ConnectMidiHandleNote();
#if MIDI_HAS_SYSEX    
    DisconnectMidiSysex();
#endif    
    stepLeds = 0;
    configLed = 0;
    menuLed = 0;
    break;
  }
}
