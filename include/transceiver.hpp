//============================================================================
// Project	   : Transceiver Implementation on USRP2
// CSCI        : \n
// CSC         : \n
//============================================================================
// File Name   : transceiver.hpp
// Description : Transceiver API header file
//
//
// Copyright   : Thales Communications S.A
//============================================================================
// Modifications
//============================================================================
// Author and date     : Mourad Haid, 2010 02 22
// Modification		   : Initial Creation
//============================================================================
// Author and date     : Nicolas Koropoulis, 2011 05 06
// Modification	       : Extension of previous work
//============================================================================
// Author and date     : Alejandro Sanchez, 2011 11 01
// Modification	       : First official Release preparation
//============================================================================
// Author and date     : Alejandro Sanchez, 2012 05 04
// Modification	       : I&Q samples changed to float (Linux machine 32 bits)
//============================================================================
// Author and date     : Alejandro Sanchez, 2012 07 03
// Modification	       : Clean up for release
//============================================================================


#ifndef TRANSCEIVER_HPP_INCLUDED
#define TRANSCEIVER_HPP_INCLUDED

//============================================================================
//			Includes List
//============================================================================
#include "time.h"
#include <iostream>
#include <complex>
#include <queue>
//============================================================================
// The namespace transceiver includes the Time_Management,
// CycleProfiles_Management and BasebandFIFO_Management used by the
// transceiver and the modem
// TIME MANAGEMENT: It is implemented by the class Time.
//    *The class Time has been created to decribe the different ways to give a TransmitStartTime and receiveStartTime in the createCycle function.\n
//    *There are 4 different types of Time:\n
//    *AbsoluteTime,EventBasedTime,Immediate(only for the startTime),undefined(only for the StopTime).\n
//    *AbsoluteTime gives the time when the samples must be sent.\n
//    *EventBasedTime gives a timeShift to add to the Time of the previous Event.\n
//    *Immediate means that the Transmit and the receive must start ASAP.\n
//    *Undefined means that the Receive has no stop time.\n
//    * CycleProfiles_Management.\n
//    * the CycleProfiles includes the parameters needed to set the device for a transmit or a receive, and the StartTime and StopTime.\n
//    * The classes I_TransmitControl and I_Receivecontrol include the functions needed to control and manage a cycleProfile.\n
//    *BaseBandFIFO_Management.\n
//    *the classes I_ReceiveDataPush and I_TransmitDataPush  define the way you can store the data in the basebandFIFO.\n
//    *I_Synchronisation.\n
//    *the classes I_ReceiveDataPush and I_TransmitDataPush  define the way you can store the data in the basebandFIFO.\n
//============================================================================
namespace Transceiver
{
typedef bool			Boolean;
typedef short			Short;
typedef long			Long;
typedef unsigned short	UShort;
typedef unsigned long	ULong;

typedef float	Frequency;
typedef Short 	BasebandPower;
typedef Short	AnaloguePower;
typedef	ULong	Latency;
typedef Short	Gain;
typedef	Short	GainSlope;
typedef	float	IQ;




typedef enum {transmitStartTime,transmitStopTime,receiveStartTime,receiveStopTime} EventSource;
typedef enum {absoluteDiscriminator,eventBasedDiscriminator,immediateDiscriminator,undefinedDiscriminator} TimeDiscriminator;

//============================================================================
// struct name   : EventBasedTime
// struct role   : EventBasedTime part of the class Time
// Variables     :
//	eventSourceId: Reference event
//	eventCountOrigin: Event reference origin
//	eventCount: Events counter
//	timeShift: Time to add to reference event
//============================================================================
typedef struct EventBasedTimeStruct
{
	EventSource eventSourceId;
	enum { Beginning, Previous, Next } eventCountOrigin;
	ULong eventCount;
	Latency timeShift;

	EventBasedTimeStruct(EventSource inEventSourceId=transmitStartTime,Latency inTimeShift=0)
	{
		eventSourceId=inEventSourceId;
		timeShift=inTimeShift;
		eventCountOrigin=Previous;
	}
	EventBasedTimeStruct(EventBasedTimeStruct const& inEventBasedTimeStruct)
	{
		eventSourceId=inEventBasedTimeStruct.eventSourceId;
		eventCountOrigin=inEventBasedTimeStruct.eventCountOrigin;
		eventCount=inEventBasedTimeStruct.eventCount;
		timeShift=inEventBasedTimeStruct.timeShift;
	}
	EventBasedTimeStruct& operator=(EventBasedTimeStruct const& inEventBasedTimeStruct)
	{
		eventSourceId=inEventBasedTimeStruct.eventSourceId;
		eventCountOrigin=inEventBasedTimeStruct.eventCountOrigin;
		eventCount=inEventBasedTimeStruct.eventCount;
		timeShift=inEventBasedTimeStruct.timeShift;
		return *this;
	}
	EventBasedTimeStruct& operator+=(Latency const& inTimeShift)
	{
		timeShift+=inTimeShift;
		return *this;
	}
	EventBasedTimeStruct operator+(Latency const& inTimeShift)
	{
		EventBasedTimeStruct temp;
		temp=*this;
		temp+=inTimeShift;
		return temp;
	}
} EventBasedTime;

//============================================================================
// struct name   : AbsoluteTime
// struct role   : AbsoluteTime part of the class Time
// Variables    :
//	secondCount: Target time, seconds field
//	nanosecondCount: Target time nanoseconds field
//	Many operators are created for this struct because it's the principal way
//to represent the time
//============================================================================
typedef struct AbsoluteTimeStruct
{
	ULong secondCount;
	ULong nanosecondCount;

	AbsoluteTimeStruct(ULong inSec=0, ULong inNsec=0)
	{
		secondCount = inSec;
		nanosecondCount = inNsec;
	}
	AbsoluteTimeStruct(AbsoluteTimeStruct const& inAbsoluteTimeStruct)
	{
		secondCount=inAbsoluteTimeStruct.secondCount;
		nanosecondCount=inAbsoluteTimeStruct.nanosecondCount;
	}
	AbsoluteTimeStruct& operator=(AbsoluteTimeStruct const& inAbsoluteTimeStruct)
	{
		secondCount=inAbsoluteTimeStruct.secondCount;
		nanosecondCount=inAbsoluteTimeStruct.nanosecondCount;
		return *this;
	}
	AbsoluteTimeStruct& operator+=(AbsoluteTimeStruct const& inAbsoluteTimeStruct)
	{
		secondCount+=inAbsoluteTimeStruct.secondCount;
		nanosecondCount+=inAbsoluteTimeStruct.nanosecondCount;
		secondCount+=nanosecondCount/1000000000;
		nanosecondCount=nanosecondCount%1000000000;
		return *this;
	}
	AbsoluteTimeStruct& operator-=(AbsoluteTimeStruct const& inAbsoluteTimeStruct)
	{
		secondCount-=inAbsoluteTimeStruct.secondCount;
		secondCount-=inAbsoluteTimeStruct.nanosecondCount/1000000000;
		if (inAbsoluteTimeStruct.nanosecondCount%1000000000>nanosecondCount)
		{
			secondCount-=1;
			nanosecondCount+= 1000000000;
		}
		nanosecondCount-=inAbsoluteTimeStruct.nanosecondCount;
		return *this;
	}
	AbsoluteTimeStruct& operator+=(Latency const& inTimeShift)
	{
		nanosecondCount+=inTimeShift;
		secondCount+=nanosecondCount/1000000000;
		nanosecondCount=nanosecondCount%1000000000;
		return *this;
	}
	AbsoluteTimeStruct operator-(AbsoluteTimeStruct const& inAbsoluteTimeStruct)
	{
		AbsoluteTimeStruct temporaryAbsoluteTimeStruct;
		temporaryAbsoluteTimeStruct=*this;
		temporaryAbsoluteTimeStruct-=inAbsoluteTimeStruct;
		return temporaryAbsoluteTimeStruct;
	}
	AbsoluteTimeStruct operator+(AbsoluteTimeStruct const& inAbsoluteTimeStruct)
	{
		AbsoluteTimeStruct temporaryAbsoluteTimeStruct;
		temporaryAbsoluteTimeStruct=*this;
		temporaryAbsoluteTimeStruct+=inAbsoluteTimeStruct;
		return temporaryAbsoluteTimeStruct;
	}
	AbsoluteTimeStruct operator+(Latency const& inTimeShift)
	{
		AbsoluteTimeStruct temporaryAbsoluteTimeStruct;
		temporaryAbsoluteTimeStruct=*this;
		temporaryAbsoluteTimeStruct+=inTimeShift;
		return temporaryAbsoluteTimeStruct;
	}
	ULong operator*(double const& inRate)
	{
		AbsoluteTimeStruct temporaryAbsoluteTimeStruct;
		temporaryAbsoluteTimeStruct=*this;
		return (temporaryAbsoluteTimeStruct.secondCount+temporaryAbsoluteTimeStruct.nanosecondCount*1000000000)*inRate;
	}
	Boolean operator<(AbsoluteTimeStruct const& inAbsoluteTimeStruct)
	{
		if (secondCount < inAbsoluteTimeStruct.secondCount)
			return true;
		else if (secondCount == inAbsoluteTimeStruct.secondCount)
			return nanosecondCount < inAbsoluteTimeStruct.nanosecondCount;
		else
			return false;
	}
	Boolean operator>(AbsoluteTimeStruct const& inAbsoluteTimeStruct)
	{
		if (secondCount > inAbsoluteTimeStruct.secondCount)
			return true;
		else if (secondCount == inAbsoluteTimeStruct.secondCount)
			return nanosecondCount > inAbsoluteTimeStruct.nanosecondCount;
		else
			return false;
	}
#ifdef DEBUG_ON
	void screen()
	{
		std::cout<<"AbsoluteTimesStruct  "<<secondCount<<" sec et "<<nanosecondCount<<" nanosecs"<<std::endl;
	}
#endif
	operator timespec()
	{
		timespec t;
		t.tv_sec = secondCount;
		t.tv_nsec = nanosecondCount;
		return t;
	}
} AbsoluteTime;

//============================================================================
// struct name   : LastTransmitTimes
// struct role   : This struct stores The times of the following Events :
//	TransmitStart and TransmitStop
// This struct is stored in the class DeviceImp and modified by the
// processChannel of the TransmitChannel
// Variables:
//	AbsoluteTime begin: Last transmit time start time
//	AbsoluteTime end: Last transmit time stop time
//============================================================================
typedef struct LastTransmitTimesStruct
{
	AbsoluteTime begin;
	AbsoluteTime end;
	LastTransmitTimesStruct(ULong inBeginSec=0, ULong inBeginNsec=0,ULong inEndSec=0, ULong inEndNsec=0)
	{
		begin.secondCount = inBeginSec;
		begin.nanosecondCount = inBeginNsec;
		end.secondCount = inEndSec;
		end.nanosecondCount = inEndNsec;
	}
#ifdef DEBUG_ON
	void screen()
	{
		std::cout<<"LastTransmitTimesStruct.begin   "<<begin.secondCount<<" sec et "<<begin.nanosecondCount<<" fracsecs"<<std::endl;
		std::cout<<"LastTransmitTimesStruct.end   "<<end.secondCount<<" sec et "<<end.nanosecondCount<<" fracsecs"<<std::endl;
	}
#endif
} LastTransmitTimes;

//============================================================================
// struct name   : LastReceiveTimes
// struct role   : This struct stores The times of the following Events :
//	ReceiveStart, ReceiveStop
// This struct is stored in the class DeviceImp and modified by the
// processChannel of the ReceiveChannel
// variables:
//	AbsoluteTime begin: Last receive time start time
//	AbsoluteTime end: Last receive time stop time
//============================================================================
typedef struct LastReceiveTimesStruct
{
	AbsoluteTime begin;
	AbsoluteTime end;
	LastReceiveTimesStruct(ULong inBeginSec=0, ULong inBeginNsec=0,ULong inEndSec=0, ULong inEndNsec=0)
	{
		begin.secondCount = inBeginSec;
		begin.nanosecondCount = inBeginNsec;
		end.secondCount = inEndSec;
		end.nanosecondCount = inEndNsec;
    }
#ifdef DEBUG_ON
	void screen()
	{
		std::cout<<"LastReceiveTimesStruct.begin   "<<begin.secondCount<<" sec et "<<begin.nanosecondCount<<" fracsecs"<<std::endl;
		std::cout<<"LastReceiveTimesStruct.end   "<<end.secondCount<<" sec et "<<end.nanosecondCount<<" fracsecs"<<std::endl;
	}
#endif
} LastReceiveTimes;

//============================================================================
// class   : Time
// class role   : Describe the way the Time is given to the cycleProfiles
// Variables    :
//	TimeDiscriminator discriminator: Type of time among AbsoluteTime,
//	EventBasedTime,ImmediateTime,UndefinedTime
//	AbsoluteTime absolute: AbsoluteTime struct data
//	EventBasedTime eventBased: EventBasedTime struct data
//============================================================================
class Time
{
	public:
	TimeDiscriminator discriminator;
	AbsoluteTime 	absolute;
	EventBasedTime	eventBased;
	Time(AbsoluteTime inTime):discriminator(absoluteDiscriminator),absolute(inTime) {}
	Time(EventBasedTime inTime):discriminator(eventBasedDiscriminator),eventBased(inTime) {}
	Time(TimeDiscriminator inTimeType):discriminator(inTimeType) {}
	Time() {}
	~Time() {}
	Time& operator=(Time const& inTime)
	{
		discriminator= inTime.discriminator;
		absolute = inTime.absolute;
		eventBased = inTime.eventBased;
		return *this;
	}
};

//============================================================================
// class   : ReceiveCycleProfile
// class role   : Describes when the receive must Start and Stop and with what
//	settings for the device
// Variables    :
//	ULong ReceiveCycle : ID of the ReceiveCycle
//	UShort TuningPreset : tuning preset selected by the modem
//	Frequency CarrierFrequency : carrierFrequency to set the device into
//	ULong PacketSize: size of Rx packets
//	Time ReceiveStartTime : Receive start time
//	Time ReceiveStopTime : Receive stop time
//============================================================================
class ReceiveCycleProfile
{
	public:
	ULong ReceiveCycle;
	UShort TuningPreset;
	Frequency CarrierFrequency;
	ULong PacketSize;
	Time ReceiveStartTime;
	Time ReceiveStopTime;
	ReceiveCycleProfile(): ReceiveCycle(0),TuningPreset(0),CarrierFrequency(0),PacketSize(0),
			ReceiveStartTime(immediateDiscriminator),ReceiveStopTime(undefinedDiscriminator){}
	ReceiveCycleProfile( ULong inReceiveCycle,UShort inTuningPreset, Frequency inCarrierFrequency,
						ULong inPacketSize,Time inReceiveStartTime,Time inReceiveStopTime):
			ReceiveCycle(inReceiveCycle), TuningPreset(inTuningPreset), CarrierFrequency(inCarrierFrequency),
			PacketSize(inPacketSize), ReceiveStartTime(inReceiveStartTime), ReceiveStopTime(inReceiveStopTime){}
	~ReceiveCycleProfile(){}
};

//============================================================================
// class   : TransmitCycleProfile
// class role   : Describes when the receive must Start and Stop and with what
//	settings for the device
// Variables    :
//	ULong TransmitCycle : ID of the TransmitCycle
//	UShort TuningPreset : tuning preset selected by the modem
//	Frequency CarrierFrequency : carrierFrequency to set the device into
//	AnaloguePower NominalPower : nominalPower RF power
//	Time TransmitStartTime: Transmit start time
//	Time TransmitStopTime: Transmit stop time
//============================================================================
class TransmitCycleProfile
{
	public:
	ULong TransmitCycle;
	UShort TuningPreset;
	Frequency CarrierFrequency;
	AnaloguePower NominalPower;
	Time TransmitStartTime;
	Time TransmitStopTime;
	TransmitCycleProfile():TransmitCycle(0),TuningPreset(0),CarrierFrequency(0),
			NominalPower(0),TransmitStartTime(immediateDiscriminator),TransmitStopTime(undefinedDiscriminator){}
	TransmitCycleProfile(ULong inTransmitCycle,UShort inTuningPreset,Frequency inCarrierFrequency,
						AnaloguePower inNominalPower,Time inTransmitStartTime,Time inTransmitStopTime):
			TransmitCycle(inTransmitCycle),TuningPreset(inTuningPreset),CarrierFrequency(inCarrierFrequency),
			NominalPower(inNominalPower),TransmitStartTime(inTransmitStartTime),TransmitStopTime(inTransmitStopTime){}
	~TransmitCycleProfile(){}
};

//============================================================================
// class   : I_ReceiveControl
// class role   : Interface class with operations for Receiving management
//============================================================================
class I_ReceiveControl
{
	public :
	//============================================================================
	// function   :  createReceiveCycleProfile
	// arguments   :
	//				Time		requestedReceiveStartTime,
	//				Time		requestedReceiveStopTime,
	//				ULong		requestedPacketSize,
	//				UShort		requestedPresetId,
	//				Frequency	requestedCarrierFrequency
	//============================================================================
	virtual ULong createReceiveCycleProfile(
			Time			requestedReceiveStartTime,
			Time			requestedReceiveStopTime,
			ULong			requestedPacketSize,
			UShort			requestedPresetId,
			Frequency		requestedCarrierFrequency) = 0;
	//============================================================================
	// function   :  configureReceiveCycle
	// arguments   :
	//				ULong		targetCycleId,
	//				Time		requestedReceiveStartTime,
	//				Time		requestedReceiveStopTime,
	//				ULong		requestedPacketSize,
	//				Frequency	requestedCarrierFrequency
	//============================================================================
	virtual void configureReceiveCycle(
			ULong			targetCycleId,
			Time			requestedReceiveStartTime,
			Time			requestedReceiveStopTime,
			ULong			requestedPacketSize,
			Frequency		requestedCarrierFrequency) = 0;
	//============================================================================
	// function   :  setReceiveStopTime
	// arguments  :
	//				ULong		targetCycleId,
	//				Time		requestedReceiveStopTime
	//============================================================================
	virtual void setReceiveStopTime(
			ULong			targetCycleId,
			Time			requestedReceiveStopTime) = 0;
};

//============================================================================
// class   : I_TransmitControl
// class role   : Interface class with operations for Transmitting management
//============================================================================
class I_TransmitControl
{
	public :
	//============================================================================
	// function   : createReceiveCycleProfile
	// arguments   :
	//				Time		requestedReceiveStartTime,
	//				Time		requestedReceiveStopTime,
	//				ULong		requestedPacketSize,
	//				UShort		requestedPresetId,
	//				Frequency	requestedCarrierFrequency
	//============================================================================
	virtual ULong createTransmitCycleProfile(
			Time 		requestedTransmitStartTime,
			Time			requestedTransmitStopTime,
			UShort			requestedPresetId,
			Frequency		requestedCarrierFrequency,
			AnaloguePower		requestedNominalRFPower) = 0;
	//============================================================================
	// function   : configureReceiveCycle
	// arguments   :
	//				ULong			targetCycleId,
	//				Time			requestedReceiveStartTime,
	//				Time			requestedReceiveStopTime,
	//				ULong			requestedPacketSize,
	//				Frequency		requestedCarrierFrequency
	//============================================================================
	virtual void configureTransmitCycle(
			ULong			targetCycleId,
			Time			requestedTransmitStartTime,
			Time			requestedTransmitStopTime,
			Frequency		requestedCarrierFrequency,
			AnaloguePower		requestedNominalRFPower) = 0;
	//============================================================================
	// function   : setTransmitStopTime
	// arguments   :
	//				ULong			targetCycleId,
	//				Time			requestedTransmitStopTime
	//============================================================================
	virtual void setTransmitStopTime(
			ULong			targetCycleId,
			Time			requestedTransmitStopTime) = 0;
};
//============================================================================
// struct name   : BBSample
// struct role   : Data type for BB samples storage
// Variables   :
//	typeIQ	valueI; in phase value
//	typeIQ	valueQ: in quadrature value
//============================================================================
typedef struct BBSampleStruct
{
	IQ	valueI;
	IQ	valueQ;
	BBSampleStruct(IQ inI=0,IQ inQ=0):valueI(inI),valueQ(inQ) {}
} BBSample;

//============================================================================
// class name   : BBPacket
// class role   : vector of BB samples
// Variables       :
//	ULong	SampleNumber: number of samples
//	BBSample*	packet: pointer to packet
//============================================================================
class BBPacket
{
	public:
	ULong	SampleNumber;
	BBSample*	packet;
	BBPacket(ULong inSamplesNumber, BBSample* inPacket):SampleNumber(inSamplesNumber),packet(inPacket) {}
};

//============================================================================
// class name   : I_ReceiveDataPush
// class role   : Interface class for data transfer. Pushes the samples in the
//basebandFIFO from the transceiver
// Variables       :
//					BBPacket * thePushedPacket: packet to be retrieved
//					Boolean endOfBurst: flag signaling the end of a burst
//============================================================================
class I_ReceiveDataPush
{
	public :
	virtual void pushBBSamplesRx(
			BBPacket * thePushedPacket,
			Boolean endOfBurst) = 0;
};

//============================================================================
// class name   : I_TransmitDataPush
// class role   : Interface class for data transfer. Pushes the samples from
//the basebandFIFO to the transceiver
// Variables       :
//					BBPacket * thePushedPacket: packet to be pushed
//					Boolean endOfBurst: flag signaling the end of a burst
//============================================================================
class I_TransmitDataPush
{
	public :
	virtual void pushBBSamplesTx(
			BBPacket * thePushedPacket,
			Boolean endOfBurst) = 0;
};



}
#endif // TRANSCEIVER_HPP_INCLUDED
