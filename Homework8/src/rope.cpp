#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k,
               vector<int> pinned_nodes) {
        // DONE (Part 1): Create a rope starting at `start`, ending at `end`, and containing
        // `num_nodes` nodes.

        Vector2D node_distance{(end - start) / (num_nodes - 1)};

        for (int i{0}; i < num_nodes; ++i) {
            auto* node = new Mass(start + node_distance * i, node_mass, false);
            masses.emplace_back(node);
            if (i == 0) { continue; }
            auto* spring = new Spring(masses[i - 1], masses[i], k);
            springs.emplace_back(spring);
        }

        for (auto& i : pinned_nodes) { masses[i]->pinned = true; }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity) {
        for (auto& s : springs) {
            // DONE (Part 2): Use Hooke's law to calculate the force on a node
            Vector2D vec_ab   = s->m2->position - s->m1->position;
            double   distance = vec_ab.norm();
            Vector2D f_ab     = s->k * (vec_ab / distance) * (distance - s->rest_length);
            Vector2D f_ba     = -f_ab;

            s->m1->forces += f_ab;
            s->m2->forces += f_ba;
        }

        for (auto& m : masses) {
            if (!m->pinned) {
                // DONE (Part 2): Add the force due to gravity, then compute the new velocity and
                // position
                //  DONE (Part 2): Add global damping

                double damping = 5e-3;

                Vector2D a   = (m->forces - m->velocity * damping) / m->mass + gravity;
                m->velocity += a * delta_t;

                // 显式欧拉方法
                //  m->position += m->velocity *delta_t;

                // 半隐式欧拉方法
                Vector2D v_new  = m->velocity + a * delta_t;
                m->position    += v_new * delta_t;
            }

            // Reset all forces on each mass
            m->forces.x = 0;
            m->forces.y = 0;
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity) {
        for (auto& s : springs) {
            // DONE (Part 3): Simulate one timestep of the rope using explicit Verlet （solving
            // constraints)
            Vector2D vec_ab   = s->m2->position - s->m1->position;
            double   distance = vec_ab.norm();
            Vector2D dir_ab   = vec_ab / distance;

            if (s->m1->pinned && s->m2->pinned) { continue; }

            if (s->m1->pinned && !s->m2->pinned) {
                s->m2->position += -dir_ab * (distance - s->rest_length);
            } else if (!s->m1->pinned && s->m2->pinned) {
                s->m1->position += dir_ab * (distance - s->rest_length);
            } else {
                s->m1->position += dir_ab * (distance - s->rest_length) * 0.5;
                s->m2->position += -dir_ab * (distance - s->rest_length) * 0.5;
            }
        }

        for (auto& m : masses) {
            if (!m->pinned) {
                // Vector2D temp_position = m->position;
                // DONE (Part 3.1): Set the new position of the rope mass
                // DONE (Part 4): Add global Verlet damping

                Vector2D a       = m->forces / m->mass + gravity;
                Vector2D v_old   = m->position;
                double   damping = 0.00005;

                // 显式 Verlet add damping
                m->position +=
                    (1 - damping) * (m->position - m->last_position) + a * delta_t * delta_t;
                m->last_position = v_old;
            }
        }
    }
} // namespace CGL
