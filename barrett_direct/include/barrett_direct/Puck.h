/*

  Author(s): Simon Leonard
  Created on: Dec 02 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef __BARRETT_DIRECT_PUCK_H
#define __BARRETT_DIRECT_PUCK_H

#include <leoCAN/CANBus.h>
#include <barrett_direct/Barrett.h>

//! Implements a Barrett puck
/**
   Pucks are the small gizmos that are connected to each motor. They essentially
   have all the hardware to control motors (micro-controller, amplifier,
   encoder and CAN device). Pucks are used to query encoders positions and set
   motors currents. A puck also has several "properties" (see the Barrett 
   documentation) to determine their behavior. Each puck is accesses on the CAN 
   bus by its node ID but several pucks can be addressed by group 
   (\sa devGroup).
*/

namespace barrett_direct {
  class Puck {

  public:

    EIGEN_MAKE_ALINGED_OPERATOR_NEW

    //! Define the ID of a puck
    /**
      The ID of a puck is at most 5 bits. Hence we use 8 bits of which only the 5
      LSB are used.
      */
    enum ID { PUCK_ID1=1,                // ID for a 4/7 DOF WAM
      PUCK_ID2=2, 
      PUCK_ID3=3,
      PUCK_ID4=4, 
      PUCK_ID5=5, 
      PUCK_ID6=6, 
      PUCK_ID7=7,
      SAFETY_MODULE_ID=10,      // ID for WAM safety module
      PUCK_IDF1=11,             // ID for BH 280
      PUCK_IDF2=12,
      PUCK_IDF3=13,
      PUCK_IDF4=14  };

    //! Define the modes of a puck
    /**
      Pucks defines the following mode in which a puck can be
      */
    static const Barrett::Value STATUS_RESET = 0;
    static const Barrett::Value STATUS_READY = 2;  

    //! Define the status of a puck
    /**
      Pucks defines the following status for a puck
      */
    static const Barrett::Value MODE_IDLE = 0;
    static const Barrett::Value MODE_TORQUE = 2;
    static const Barrett::Value MODE_POSITION = 3;
    static const Barrett::Value MODE_PID = 3;
    static const Barrett::Value MODE_VELOCITY = 4;
    static const Barrett::Value MODE_TRAPEZOIDAL = 5;

    //! Error values
    enum Errno{ ESUCCESS, EFAILURE };

  private:

    std::string LogPrefix();


    //! The CAN device connected to the puck.
    /**
      This is a bit counter-intuitive. This is not the CAN device "on" the puck
      (each puck has a CAN device). Rather, this is the can device used to 
      communicate "with" the puck.
      */
    leoCAN::CANBus*   canbus;

    //! The ID of the puck
    Puck::ID  id;

    //! Number of counts per revolution of the encoder
    Barrett::Value cntprev;

    //! Amps per Newton meter constant of the motor
    Barrett::Value ipnm;

    //! The index of the puck within its group
    Barrett::Value grpidx;

    //! The maximum current of the motor
    Barrett::Value imax;

    //! The encoder position
    Barrett::Value encpos;

    //! The motor current
    Barrett::Value mcurrent;

    Barrett::Value groupA, groupB, groupC;

    //! Convert a puck ID to a CAN id (assume origin from host 00000)
    /**
      Convert the ID of a puck to a CAN ID used in a CAN frame
      */
    static leoCAN::CANBusFrame::id_t CANID( Puck::ID id );

    //! Does the data contain a set property command
    /**
      Barretts have properties that can be read/write. To read/write a property, 
      you must send a CAN frame with the proper data format. To write a property,
      the CAN data must have a "write" bit set while a read command must have the
      "write" bit cleared. This method returns true if the "write" bit is set.
      \param canframe A CAN frame with a read/write command
      \return true if the command is a write. false if the command is a read
      */
    static bool IsSetFrame( const leoCAN::CANBusFrame& canframe );

    //! pack a CAN frame
    /**
      Build a CAN frame destined to the puck with the data formated.
      \param canframe[out] The resulting CAN frame
      \param propid The property ID to set or get
      \param propval The property value if the property must be set
      \param set True of the property must be set. False for a query
      \return false if no error occurred. true otherwise
      */
    Puck::Errno PackProperty( leoCAN::CANBusFrame& canframe,
        Barrett::Command command,
        Barrett::ID id,
        Barrett::Value val = 0 );

  public:

    //! Default constructor
    Puck();

    //! Create a puck with an ID and a CAN device
    /**
      Initialize the puck to the given ID and give the CAN device connected to 
      the puck.
      \param puckid The ID of the puck
      \param can The CAN device used to communicate with the puck
      */
    Puck( Puck::ID id, leoCAN::CANBus* can, bool createfilter=true );

    //! Return the puck ID
    Puck::ID GetID() const;

    //! Return the origin ID of the CAN id
    /**
      Each CAN ID consists of 11 bits. For communicating with pucks, this
      ID is composed of 5 bits representing the ID of the puck at the origin 
      of the CAN frame and 5 bits representing the ID of the destination puck.
      Call this method to obtain the origin puck ID in a CAN ID.
      \param canid The CAN id
      \return The origin puck ID of the CAN ID
      \sa Origin( leoCAN::CANBusFrame )
      */
    static Puck::ID OriginID( leoCAN::CANBusFrame::id_t id );

    //! Return the origin ID of the CAN frame
    /**
      Each CAN frame contains an 11 bits ID. For communicating with pucks, this
      ID is composed of 5 bits representing the ID of the puck at the origin 
      of the CAN frame and 5 bits representing the ID of the destination puck.
      Call this method to obtain the origin puck ID in a CAN frame.
      \param canid The CAN frame
      \return The origin puck ID of the CAN frame
      \sa Origin( leoCAN::CANBusID )
      */
    static Puck::ID OriginID( const leoCAN::CANBusFrame& canframe );

    //! Return the destination ID of the CAN id
    /**
      Each CAN ID consists of 11 bits. For communicating with pucks, this
      ID is composed of 5 bits representing the ID of the puck at the origin 
      of the CAN frame and 5 bits representing the ID of the destination puck.
      Call this method to obtain the destination puck ID in a CAN ID.
      \param canid The CAN id
      \return The origin puck ID of the CAN ID
      \sa Origin( leoCAN::CANBusFrame )
      */
    static Puck::ID DestinationID( leoCAN::CANBusFrame::id_t id );

    //! Return the destination ID of the CAN frame
    /**
      Each CAN frame contains an 11 bits ID. For communicating with pucks, this
      ID is composed of 5 bits representing the ID of the puck at the origin 
      of the CAN frame and 5 bits representing the ID of the destination puck.
      Call this method to obtain the destination puck ID in a CAN frame.
      \param canid The CAN frame
      \return The origin puck ID of the CAN frame
      \sa Destination( leoCAN::CANBusID )
      */
    static Puck::ID DestinationID( const leoCAN::CANBusFrame& canframe );

    //! Return the index of the puck within its group
    /**
      Each puck on the WAM belongs to a group. The upper arm group contains 
      represents 4 pucks and the forearm group represents 3 pucks. Since these
      groups are ordered, the index (0, 1, 2, 3) of a puck within its group 
      matters. Call this method if you want to find the (zero) index of the pucks
      within its group.
      \return The zero index of the puck within its group
      */
    Barrett::Value GroupIndex() const ;

    //! Return the motor Amp/Nm constant
    Barrett::Value IpNm() const ;

    //! Return the encoder counts/revolution constant
    Barrett::Value CountsPerRevolution() const ;

    //! Perform the initial configuration
    /**
      This sets the puck in the "ready" status and in "idle" mode. It also 
      queries the puck for its group index, I/Nm constant and counts/rev 
      constant.
      \param false if no error occurred. true otherwise
      */
    Puck::Errno InitializeMotor();


    //! Perform the initial configuration
    /**
      This sets the safety module
      \param false if no error occurred. true otherwise
      */
    Puck::Errno InitializeSM();


    Puck::Errno SetMode( Barrett::Value mode );
    Puck::Errno GetMode( Barrett::Value& mode );

    Puck::Errno GetGroupA();
    Puck::Errno GetGroupB();
    Puck::Errno GetGroupC();

    Puck::Errno SetGroupA( Barrett::Value A );
    Puck::Errno SetGroupB( Barrett::Value B );
    Puck::Errno SetGroupC( Barrett::Value C );

    Puck::Errno GetStatus( Barrett::Value& status );

    Puck::Errno GetCountsPerRev();
    Puck::Errno GetIpNm();
    Puck::Errno GetGroupIndex();
    Puck::Errno GetMembership();


    Puck::Errno IgnoreFault( Barrett::Value fault );

    Puck::Errno SetVelocityWarning( Barrett::Value vw );
    Puck::Errno SetVelocityFault( Barrett::Value vf );

    Puck::Errno SetTorqueWarning( Barrett::Value tw );
    Puck::Errno SetTorqueFault( Barrett::Value tf );

    //! Query the puck for a property ID 
    /**
      This method queries the puck for the value of a property.
      \param propid The ID of the property to query
      \return The value of the property
      */
    Puck::Errno GetProperty( Barrett::ID id, Barrett::Value& value );

    //! Set the puck property ID to a value
    /**
      This method sets the value of the puck's property.
      \param propid The ID of the property to set
      \param propval The value of the property
      \param verify Double check by querying the property of the puck
      \return false is no error occurred. true otherwise
      */
    Puck::Errno SetProperty( Barrett::ID propid, 
        Barrett::Value propval, 
        bool verify );

    //! Reset the firmware
    Puck::Errno Reset();

    //! Set the status to ready
    Puck::Errno Ready();

    Puck::Errno SetPosition( double q );
    Puck::Errno GetPosition( Barrett::Value& position );

    //! unpack a CAN frame
    /**
      Extract the property ID and a property value of a CAN frame.
      \param canframe[in] The CAN frame to process
      \param propid[out] The ID of the property in the data
      \param propval[out] The value of the property in the data
      */
    Puck::Errno UnpackCANFrame( const leoCAN::CANBusFrame& canframe, 
        Barrett::ID& id, 
        Barrett::Value& value );

  }; 

  // Increment operator for pucks id
  Puck::ID operator++( Puck::ID& pid, int i );


  //! Define the safety module
  /**
    The safety module is the hardware that monitors the activity on the CAN bus.
    It contains a puck that processes the CAN frames and is able to shut down the
    WAM if a fault occurs.
    */
  typedef Puck SafetyModule;
}

#endif // ifndef __BARRETT_DIRECT_PUCK_H
