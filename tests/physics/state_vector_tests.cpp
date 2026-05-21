#include "pendulum/physics/chain_pendulum_system.hpp"
#include "pendulum/physics/rk4_integrator.hpp"
#include "pendulum/physics/single_pendulum_system.hpp"
#include "pendulum/physics/state_vector.hpp"

#include <cassert>
#include <cmath>
#include <vector>

int main()
{
    using namespace pendulum::physics;

    const StateVector a{1.0, 2.0};
    const StateVector b{3.0, 4.0};
    const StateVector c = a + b * 2.0;

    assert(c.size() == 2);
    assert(c[0] == 7.0);
    assert(c[1] == 10.0);

    SinglePendulumSystem pendulum{
        PendulumLink{1.0, 1.0},
        PhysicsParameters{9.81, 0.0},
        0.1,
        0.0
    };

    RK4Integrator integrator;
    integrator.step(pendulum, 0.0, 1.0 / 240.0);

    const StateVector next = pendulum.state();
    assert(next.size() == 2);
    assert(std::isfinite(next[0]));
    assert(std::isfinite(next[1]));

    assert(pendulum.setMassPositionWithoutVelocity(0, {0.0, -1.0}));
    const StateVector draggedToTop = pendulum.state();
    assert(std::abs(draggedToTop[0] - std::acos(-1.0)) < 1.0e-12);
    assert(draggedToTop[1] == 0.0);

    assert(pendulum.setMassPositionWithoutVelocity(0, {0.0, 1.0}));
    const StateVector draggedToBottom = pendulum.state();
    assert(std::abs(draggedToBottom[0]) < 1.0e-12);
    assert(draggedToBottom[1] == 0.0);

    ChainPendulumSystem chain{
        std::vector<PendulumLink>{{1.0, 1.0}},
        PhysicsParameters{9.81, 0.0},
        std::vector<double>{0.1},
        std::vector<double>{0.0}
    };
    const StateVector chainDerivative = chain.derivative(chain.state(), 0.0);
    assert(std::abs(chainDerivative[1] + 9.81 * std::sin(0.1)) < 1.0e-12);
    assert(chain.linkCount() == 1);
    assert(chain.addLinkAfterLast({0.9, 0.8}));
    assert(chain.linkCount() == 2);
    assert(chain.state().size() == 4);
    assert(chain.setMassPositionWithoutVelocity(1, {0.0, 2.0}));
    assert(chain.state()[2] == 0.0);
    assert(chain.state()[3] == 0.0);
    assert(chain.removeLastLink());
    assert(chain.linkCount() == 1);

    return 0;
}
