/*--->START Define<---*/

#define LED1 5
#define LED2 4
#define LED3 6

#define Button1 2
#define Button2 3
#define Button3 8

#define ON 1
#define OFF 0

#define EventHazard 1
#define EventLaneChange 2
#define EventNormalLight 3
#define EventLock 4
#define EventDoubleLock 5
#define EventUnLock 6

/*--->END Define<---*/

/*--->START Variables Boolean<---*/

volatile bool bflag1 = OFF;
volatile bool bflag2 = OFF;

bool bPushButton1 = OFF;
bool bPushButton2 = OFF;
bool bStateHazard = OFF;
bool bStateButton1 = OFF;
bool bStateButton2 = OFF;
bool bCarIsLocked = OFF;

bool bStateConditionButton2 = OFF;

/*--->END Variables Boolean<---*/

/*--->START Variables INT<---*/

volatile unsigned int timer1 = 65534;

unsigned int iEvent = EventUnLock;

unsigned int iCounterLEDsONOFF = OFF;
unsigned int iCounterNMaxOfLights = OFF;
unsigned int iCounterReleaseLaneChange = OFF;
unsigned int iCounterButtonPushedHazard = OFF;
unsigned int iCounterNormalLight = OFF;
unsigned int iCounterLock = OFF;
unsigned int iCounterHazard = OFF;
unsigned int iCounterHazardoff = OFF;

unsigned int iTimeLEDsONLaneChange = 30;
unsigned int iTimeLEDsOFFLaneChange = 70;
unsigned int iTimeLEDsONNormalLight = 50;
unsigned int iTimeLEDsOFFNormalLight = 50;
unsigned int iTimeLEDsON = 50;
unsigned int iTimeLEDsOFF = 50;

unsigned
const int iNumberOfCyclesLaneChange = 3;
unsigned
const int iNumberOfCyclesLock = 1;
unsigned
const int iNumberOfCyclesDLock = 3;
unsigned
const int iNumberOfCyclesULock = 2;

unsigned int iFlagEvent = EventUnLock;
unsigned int iLastValueOfEvent = EventUnLock;

unsigned int counttest = 0;

/*--->END Variables INT<---*/

void setup() 
{

        pinMode(LED1, OUTPUT);
        pinMode(LED2, OUTPUT);
        pinMode(LED3, OUTPUT);

        pinMode(Button1, INPUT);
        pinMode(Button2, INPUT);
        pinMode(Button3, INPUT);

        cli(); //disable interrupt

        //timer1 = 64910; // preload timer 65535 - 16MHz/256/100Hz i.e 10mS

        TCCR1B = 0;
        TCNT1 = timer1; // preload timer
        TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 prescaler
        TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt

        EICRA |= (1 << ISC00) | (1 << ISC10); // enable any logical change on INT0 and INT1, pin 2 INT0 and pin 3
        EIMSK |= (1 << INT0) | (1 << INT1); // Enable INT0 AND INT1

        sei(); // enable interrupt

}

ISR(INT0_vect) 
{

        bPushButton1 = !bPushButton1;

}

ISR(INT1_vect) 
{

        bPushButton2 = !bPushButton2;

}

ISR(TIMER1_OVF_vect) 
{

        TCNT1 = timer1;
        bflag1 = true;

}

void loop()
{
  
                if (bflag1) 
                {

                        ReadButton();
                        ILCDL();
                        bflag1 = false;

                }
}

void ReadButton() 
{
        /*Start function ReadButton*/

        /*<Condition of Button 1>*/

        if (digitalRead(Button3) == ON) 
        {
                bStateHazard = !bStateHazard;
                digitalWrite(LED1, LOW);
                digitalWrite(LED3, LOW);
        }

        /*>End condition of Button1<*/

        /*<Logic for Hazard in ReadButton>*/

        if (bStateHazard == ON) 
        {
                if (iCounterButtonPushedHazard < 20) 
                {
                        iCounterButtonPushedHazard++;
                }
        } 
        else
        {
                if (iCounterButtonPushedHazard == 20) 
                {
                        if (iEvent == EventHazard) 
                        {

                                iEvent = iLastValueOfEvent;
                                iLastValueOfEvent = EventHazard;

                        } 
                        else 
                        {

                                iLastValueOfEvent = iEvent;
                                iEvent = EventHazard;

                        }
                }
                if (iEvent == EventHazard) 
                {
                        iCounterNMaxOfLights = OFF;
                }
                iCounterButtonPushedHazard = OFF;
        }
        /*>End of Logic for Hazard in ReadButton<*/

        /*>Condition of Button2<*/

        (bPushButton2) ? bStateButton2 = ON: bStateButton2 = OFF;

        /*>End codition of Button2<*/

        /*<Logic for LaneChange and Normal Light in ReadButton>*/

        if ((bStateButton2 == ON) && ((iFlagEvent == EventUnLock) && (iEvent != EventHazard)))
        {
                bStateConditionButton2 = ON;
                iCounterReleaseLaneChange = OFF;
                if (iCounterNormalLight < 50)
                {
                        iCounterNormalLight++;
                }
                else 
                {
                        iLastValueOfEvent = iEvent;
                        iEvent = EventNormalLight;
                        iCounterHazard = OFF;
                }
        } 
        else 
        {
                if ((iCounterNormalLight < 50) && (bStateConditionButton2 == ON)) 
                {
                        iLastValueOfEvent = iEvent;
                        iEvent = EventLaneChange;
                        iCounterNormalLight = OFF;
                        bStateConditionButton2 = OFF;
                        iCounterNMaxOfLights = OFF;
                        iCounterHazard = OFF;
                }
                if ((iCounterNormalLight >= 50) && (bStateConditionButton2 == ON)) 
                {
                        if (iCounterReleaseLaneChange < 10)
                        {
                                iCounterReleaseLaneChange++;
                        }
                        else
                        {
                                iEvent = OFF;
                                iCounterNormalLight = OFF;
                                bStateConditionButton2 = OFF;
                        }
                }
        }

        /*>End of Logic for Lane Change and Normal Light in ReadButton<*/

        /*<Condition of Button3>*/

        (bPushButton1) ? bStateButton1 = ON: bStateButton1 = OFF;

        /*>End condition of Button3<*/

        /*<Logic for Lock, Double Lock and Unlock in ReadButton>*/

        if ((bStateButton1 == ON) && ((iEvent != EventLaneChange) && (iEvent != EventNormalLight) && (iEvent != EventHazard))) 
        {
                if (iCounterLock < 40) 
                {
                        iCounterLock++;
                } 
                else 
                {
                        if (((iEvent == EventLock) || (iEvent == EventDoubleLock)) && (iEvent != EventUnLock))
                        {
                                iCounterLock = OFF;
                                iLastValueOfEvent = iEvent;
                                iEvent = EventUnLock;
                                iFlagEvent = EventUnLock;
                                iCounterNMaxOfLights = OFF;
                                iCounterHazard = OFF;
                        }
                }
        } 
        else 
        {
                if ((iCounterLock < 40) && (iCounterLock > 0) && (iEvent == EventLock)) 
                {
                        iCounterLock = OFF;
                        iLastValueOfEvent = iEvent;
                        iEvent = EventDoubleLock;
                        iCounterNMaxOfLights = OFF;
                        iCounterHazard = OFF;
                }
                if ((iCounterLock >= 40) && (iFlagEvent == EventUnLock))
                {
                        iFlagEvent = OFF;
                        iLastValueOfEvent = iEvent;
                        iEvent = EventLock;
                        iCounterNMaxOfLights = OFF;
                        iCounterHazard = OFF;
                }
                iCounterLock = OFF;
        }
        /*>End of Logic for Lock, Double Locl and Unlock in ReadButton<*/

} /*End function ReadButton*/

void ILCDL() {
        switch (iEvent)
        {
        case 1: // Hazard

                LEDs(iTimeLEDsON, iTimeLEDsOFF, ON);

                break;

        case 2: //Lane change

                LEDs(iTimeLEDsONLaneChange, iTimeLEDsOFFLaneChange, iNumberOfCyclesLaneChange);

                break;

        case 3: //Normal signaling

                iCounterNMaxOfLights = OFF;
                LEDs(iTimeLEDsONNormalLight, iTimeLEDsOFFNormalLight, ON);

                break;

        case 4: // Lock

                LEDs(iTimeLEDsON, iTimeLEDsOFF, iNumberOfCyclesLock);

                break;

        case 5: //DoubleLock

                LEDs(iTimeLEDsON, iTimeLEDsOFF, iNumberOfCyclesDLock);

                break;

        case 6: //Unlock

                LEDs(iTimeLEDsON, iTimeLEDsOFF, iNumberOfCyclesULock);

                break;

        default:

                digitalWrite(LED1, LOW);
                digitalWrite(LED3, LOW);

                iCounterLEDsONOFF = OFF;

                break;
        }
}

void LEDs(int iLEDsON, int iLEDsOFF, int iNMaxOfLights) 
{

        if ((iCounterNMaxOfLights < iNMaxOfLights) && (iLastValueOfEvent != EventHazard))
        {

                iCounterLEDsONOFF++;
                if (iCounterLEDsONOFF < iLEDsON) 
                {
                        digitalWrite(LED1, HIGH);
                        digitalWrite(LED3, HIGH);
                }
                if ((iCounterLEDsONOFF >= iLEDsON) && (iCounterLEDsONOFF < iLEDsON + iLEDsOFF)) 
                {
                        digitalWrite(LED1, LOW);
                        digitalWrite(LED3, LOW);
                }
                if (iCounterLEDsONOFF == iLEDsON + iLEDsOFF) 
                {
                        iCounterLEDsONOFF = OFF;
                        iCounterNMaxOfLights++;
                }
        } else {
                if (iEvent == EventLaneChange)
                {
                        iEvent = OFF;
                        iCounterNMaxOfLights = OFF;
                }
        }
}
