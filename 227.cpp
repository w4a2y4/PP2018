#include <iostream>
#include <stdlib.h>

typedef struct {
	double mass;
	double pos_x;
	double pos_y;
	double pos_z;
	double acc_x;
	double acc_y;
	double acc_z;
} particle;

double getrand() {
	return ( rand() / RAND_MAX ) ;
}

int main( void ) {

	int G = 1;
	int T = 20;
	int n = 10000;

	particle p[n];

	// initialize
	for (int i = 0; i < n; i++) {
		p[i].mass = getrand();
		p[i].pos_x = getrand();
		p[i].pos_y = getrand();
		p[i].pos_z = getrand();
		p[i].acc_x = 0;
		p[i].acc_y = 0;
		p[i].acc_z = 0;
	}

	// compute acceleration
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if ( i != j ) {
				double v_x = p[j].pos_x - p[i].pos_x;
				double v_y = p[j].pos_y - p[i].pos_y;
				double v_z = p[j].pos_z - p[i].pos_z;
				double r_square = v_x * v_x + v_y * v_y + v_z * v_z;
				p[i].acc_x += v_x * ( G * p[j].mass / r_square );
				p[i].acc_y += v_y * ( G * p[j].mass / r_square );
				p[i].acc_z += v_z * ( G * p[j].mass / r_square );
			}

	// compute new position
	for (int i = 0; i < n; i++) {
		// d = d0 + 0.5*a*t^2
		p[i].pos_x += 0.5 * p[i].acc_x * T * T;
		p[i].pos_y += 0.5 * p[i].acc_y * T * T;
		p[i].pos_z += 0.5 * p[i].acc_z * T * T;
	}

	return 0;
}