#pragma once

#include "galsim.h"
#include <stdio.h>

// Båda formlerna för det här finns i assignmentinstruktionerna "https://uppsala.instructure.com/courses/95091/files/7929374?wrap=1"

double *calculate_forces_over_mass(Particle *particles, int N)
{
    // TODO: returnera en array som har alla krafter som verkar på alla partiklar delat med deras massa
    return NULL;
}

void update_particles(Particle *particles, double *forces_over_mass, int N, double delta_t)
{
    // TODO: uppdatera partiklarnas position och hastighet
}