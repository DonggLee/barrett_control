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

#ifndef __BARRETT_DIRECT_BH8_280_H
#define __BARRETT_DIRECT_BH8_280_H

#include <Eigen/Dense>

#include <barrett_direct/Puck.h>
#include <barrett_direct/Group.h>

//! A clas for a BH8_280 device
/**
  Whole Arm Manipulator (BH8_280) is a 4-7DOF robot manufactured by Barrett Tech. 
  From a device perspective the BH8_280 can be interfaced from either an ethernet
  device (wired or wireless) or a CAN device. On the BH8_280, the ethernet device
  is connected to a PC104. Thus, when using ethernet you can communicate with
  the onboard computer through a shell or client/server. The downside of this 
  is that the PC104 does all the (CAN) communication with the motors and 
  much of the computation as well. The PC104 can be bypassed and the motor can 
  be controlled by an external PC (see the documentation on how to configure 
  the switches). For this, the external PC must have a CAN device and the 
  communication with the hardware must be established and maintained. The 
  BH8_280 class implements a BH8_280 that can be controlled from an external PC.
  Thus it manages the CAN bus, the pucks present on the CAN bus and the BH8_280's
  safety module. Operations are pretty basic: set motors torques, set 
  motor positions and get motor positions.   
  */

namespace barrett_direct {
  class BH8_280{

  public:

    enum Errno{ ESUCCESS, EFAILURE };

  private:

    //! A vector of all the groups
    //Group broadcast;
    Group hand;
    Group handposition;

    //! Vector of pucks
    std::vector< Puck > pucks;

    Eigen::VectorXd qinit;

    //! Matrix used to convert motors positions to joints positions
    /**
      When given a vector of motors positions, this matrix us used to transform
      the motors positions to joints positions. For the BH8_280, this is essentially
      a block diagonal matrix.
      \sa MotorsPos2JointsPos
      */
    Eigen::MatrixXd mpos2jpos;

    //! Matrix used to convert joints positions to motors positions
    /**
      When given a vector of joints positions, this matrix us used to transform
      the joints positions to motors positions. For the BH8_280, this is essentially 
      a block diagonal matrix.
      \sa JointsPos2MotorsPos
      */
    Eigen::MatrixXd jpos2mpos;

    //! Matrix used to convert joints torques to motors torques
    /**
      When given a vector of joints torques, this matrix us used to transform
      the joints torques to motors torques. For the BH8_280, this is essentially  
      a block diagonal matrix.
      \sa JointsTrq2MotorsTrq
      */
    Eigen::MatrixXd jtrq2mtrq;

    //! Convert motor positions to joints positions
    /**
      Converts the motor angles received from the pucks to joint angles.
      This only perform a matrix/vector multiplication
      \param q A vector of motor angles
      \return A vector of joint angles
      \sa mpos2jpos
      */
    Eigen::VectorXd 
      MotorsPos2JointsPos( const Eigen::VectorXd& q );

    //! Convert joints positions to motors positions
    /**
      Converts joints angles to motors angles that can be sent to the pucks.
      This only perform a matrix/vector multiplication
      \param q A vector of joints angles
      \return A vector of motors angles
      \sa jpos2mpos
      */
    Eigen::VectorXd 
      JointsPos2MotorsPos( const Eigen::VectorXd& q );

    //! Convert joints torques to motors torques
    /**
      Converts joints torques to motors torques that can be sent to the pucks.
      This only perform a matrix/vector multiplication
      \param q A vector of motor angles
      \return A vector of joint angles
      \sa jtrq2mtrq
      */
    Eigen::VectorXd 
      JointsTrq2MotorsTrq( const Eigen::VectorXd& t );

  public:

    //! Default constructor
    /**
      Initiallize the BH8_280. This configures the pucks, groups and the safety 
      module. You can use BH8_280 with any CAN device, has long as it is derived
      from the devCAN base class.
      \param canbus The CAN device used by the BH8_280. This device must be derived
      from the devCAN classe.
      \sa leo_can::CANBus
      */
    BH8_280( leo_can::CANBus* canbus );

    //! Default destructor
    ~BH8_280();

    //! Initialize the arm
    /**
      Configure the groups and the pucks
      */
    BH8_280::Errno Initialize();

    //! Return the configuration of the BH8_280 (4/7DOF)
    //BH8_280::Configuration GetConfiguration() const { return configuration; }

    //! Get joints positions
    /**
      This broadcast a position query to the pucks and process all their replies.
      First, the replies (in encoder ticks) are converted to motors positions and
      then to joint angles.
      \param positions[out] The resulting motor positions in radians
      \return ESUCCESS if no error occurred. EFAILURE otherwise.
      */
    BH8_280::Errno GetPositions( Eigen::VectorXd& positions );

    //! Send joints positions
    /**
      The BH8_280 has relative encoders, which means that you must "zero" the 
      encoders each time the BH8_280 is powered. For this you manually move the BH8_280
      to a know configuration and then call SendPosition. First, the safety 
      module is turned off to avoid triggering a velocity fault. Then, the joints
      positions are converted to encoder counts and each position is overwritten 
      by "setting" the position on each puck. Finally, the safety module is 
      turned back on. Note that this does not "move" the robot, it only tells 
      each puck the absolute position of its encoder.
      \param positions[in] The motor positions in radians
      \return ESUCCESS if no error occurred. EFAILURE otherwise.
      */
    BH8_280::Errno SetPositions( const Eigen::VectorXd& positions );

    //! Set joints torques
    /**
      Set the torques of each joint. First this converts the joints torques to 
      motors torques. Then it packs the torques in two CAN frames. The first 
      frame is addressed the upper arm group and the second frame is addressed 
      to the forearm group (if present).
      \param torques[in] The motor torques
      \return ESUCCESS if no error occurred. EFAILURE otherwise
      */
    //BH8_280::Errno SetTorques( const Eigen::VectorXd& torques );

    //! Set velocity warning
    /**
      The safety module intercepts joints positions and compute joints velocities
      from them. After "zeroing" the joints (setting their initial values), the
      safety module compute the Cartesian velocity of the elbow and the Cartesian
      velocity of the 4DOF end-effector. If any of these velocities is greater 
      than the velocity warning limit, the safety module will display a warning 
      on the 7-segment of the display pendant ("E" for elbow and "A" for arm) and
      the "Warning Velocity" LED will be on. The warning won't affect anything 
      other than warn you to slow down before a velocity fault shuts down the 
      arm.
      */
    //BH8_280::Errno SetVelocityWarning( Barrett::Value velocitywarning );

    //! Set velocity fault
    /**
      The safety module intercepts joints positions and compute joints velocities
      from them. After "zeroing" the joints (setting their initial values), the
      safety module compute the Cartesian velocity of the elbos and the Cartesian
      velocity of the 4DOF end-effector. If any of these velocities is greater 
      than the velocity fault threshold, the safety module will shutdown the arm
      and display the fault on the 7-segment of the display pendant ("E" for 
      elbow and "A" for arm) and the "Velocity Fault" LED will be on. 
      */
    //BH8_280::Errno SetVelocityFault( Barrett::Value velocityfault );

    //! Set torque warning
    /**
      The safety module intercepts joints currents that are sent to the pucks.
      If any of these torques is greater than the torque warning limit, the 
      safety module will display a warning on the 7-segment of the display 
      pendant (the number of the joint with too much torque) and the "Warning 
      Torque" LED will be on. The warning won't affect anything other than warn 
      you to ease down the torque before a torque fault shuts down the arm. Keep
      in mind that this torque limit is "one size fits all". That is the torque
      warning limit of motor 1 will apply to motor 7 and vice versa.
      */
    //BH8_280::Errno SetTorqueWarning( Barrett::Value torquewarning );

    //! Set torque fault
    /**
      The safety module intercepts joints currents that are sent to the pucks.
      If any of these torques is greater than the torque fault limit, the 
      safety module will shutdown the arm and display a warning on the 7-segment 
      of the display pendant (the number of the joint with too much torque) and 
      the "Fault Torque" LED will be on. Keep in mind that this torque limit is
      "one size fits all". That is the torque warning limit of motor 1 will apply
      to motor 7 and vice versa.
      */
    //BH8_280::Errno SetTorqueFault( Barrett::Value torquefault );

    //! Set the modes of the pucks
    BH8_280::Errno SetMode( Barrett::Value mode );

    void Hi();

  };

}

#endif // ifndef __BARRETT_DIRECT_BH8_280_H
