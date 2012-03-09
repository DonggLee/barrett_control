#ifndef __BARD_COMPONENTS_CONTROLLERS_TRIVIAL_H
#define __BARD_COMPONENTS_CONTROLLERS_TRIVIAL_H

#include <kdl/jntarray.hpp>

#include <rtt/RTT.hpp>
#include <rtt/Port.hpp>

namespace bard_components {
  namespace controllers {
    class Trivial : public RTT::TaskContext
    {
      // RTT Ports
      RTT::OutputPort<KDL::JntArray> torques_out_port_;

    public:
      Trivial(string const& name);
      bool configureHook();
      bool startHook();
      void updateHook();
      void stopHook();
      void cleanupHook();

    private:
      // Configuration properties
      int n_wam_dof_;
      // Working variables
      KDL::JntArray torques_;
    };
  }
}


#endif // ifndef __BARD_COMPONENTS_CONTROLLERS_TRIVIAL_H
