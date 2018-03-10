/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: Tiffany Huang
 */

#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h> 
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

#include "particle_filter.h"

using namespace std;
default_random_engine gen;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
	// TODO: Set the number of particles. Initialize all particles to first position (based on estimates of 
	//   x, y, theta and their uncertainties from GPS) and all weights to 1. 
	// Add random Gaussian noise to each particle.
	// NOTE: Consult particle_filter.h for more information about this method (and others in this file).
	
	//1. Init number of particles
	num_particles = 100;
		 
	// This line creates a normal (Gaussian) distribution for x,y and theta
	normal_distribution<double> dist_x(x, std[0]);
	normal_distribution<double> dist_y(y, std[1]);
	normal_distribution<double> dist_theta(theta, std[2]);
	
	//3. Init all particles
	for( unsigned int i = 0; i < num_particles; i++ )
	{
		//Create a temporary particle
		Particle temp;
		
		//Init it from a random distribution
		temp.id = i;
		temp.x = dist_x(gen);
		temp.y = dist_y(gen);
		temp.theta = dist_theta(gen);
		temp.weight = 1.0;
		
		//Add particle to vector
		particles.push_back(temp);
	}
	
	is_initialized = true;
}

void ParticleFilter::prediction(double delta_t, double std_pos[], double velocity, double yaw_rate) {
	// TODO: Add measurements to each particle and add random Gaussian noise.
	// NOTE: When adding noise you may find std::normal_distribution and std::default_random_engine useful.
	//  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
	//  http://www.cplusplus.com/reference/random/default_random_engine/
	
	//Init normal distribution for sensor - around 0 mean
	normal_distribution<double> dist_x(0, std_pos[0]);
	normal_distribution<double> dist_y(0, std_pos[1]);
	normal_distribution<double> dist_theta(0, std_pos[2]);

	//Update particles
	for(unsigned int i = 0; i < num_particles; i++)
	{
		//yaw_rate in dr will cause div by zero.
		if(fabs(yaw_rate) < 0.001)
		{
			particles[i].x += velocity * delta_t * cos(particles[i].theta);
			particles[i].y += velocity * delta_t * sin(particles[i].theta);
		}
		else
		{
			particles[i].x += velocity/yaw_rate * (sin(particles[i].theta + yaw_rate*delta_t) - sin(particles[i].theta));
			particles[i].y += velocity/yaw_rate * (cos(particles[i].theta) - cos(particles[i].theta + yaw_rate*delta_t));
			particles[i].theta += yaw_rate*delta_t;
		}
		
		//Add gaussian noise
		particles[i].x += dist_x(gen);
		particles[i].y += dist_y(gen);
		particles[i].theta += dist_theta(gen);
	}
}

void ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted, std::vector<LandmarkObs>& observations) {
	// TODO: Find the predicted measurement that is closest to each observed measurement and assign the 
	//   observed measurement to this particular landmark.
	// NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
	//   implement this method and use it as a helper during the updateWeights phase.

	for(unsigned int i = 0; i < observations.size(); i++ )
	{
		//We have to map each observation to a prediction.
	    //Assume dist is very large from any prediction
		// Initialize min distance as a really big number.
        double minDist = numeric_limits<double>::max();
		int iLandmark = -1; //The final prediction ID which is considered as a map to the observation
		
		//Run the current observation against all predictions
		for(unsigned int j = 0;  j < predicted.size(); j++ )
		{
			double xdiff = observations[i].x - predicted[i].x;
			double ydiff = observations[i].y - predicted[i].y;
			
			//Euclidean distance - Use helper function later
			double dist = xdiff*xdiff + ydiff*ydiff;
			
			//If current dist < previous minimum distance, this must be landmark
			if( dist < minDist )
			{
				minDist = dist;
				iLandmark = predicted[j].id;
			}
		}
		//Map current observation to landmark with least distance
		observations[i].id = iLandmark;
	}
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
		const std::vector<LandmarkObs> &observations, const Map &map_landmarks) {
	// TODO: Update the weights of each particle using a mult-variate Gaussian distribution. You can read
	//   more about this distribution here: https://en.wikipedia.org/wiki/Multivariate_normal_distribution
	// NOTE: The observations are given in the VEHICLE'S coordinate system. Your particles are located
	//   according to the MAP'S coordinate system. You will need to transform between the two systems.
	//   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
	//   The following is a good resource for the theory:
	//   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
	//   and the following is a good resource for the actual equation to implement (look at equation 
	//   3.33
	//   http://planning.cs.uiuc.edu/node99.html
	
	double sig_x = std_landmark[0];
	double sig_y = std_landmark[1];
	
	for(unsigned int i = 0; i < num_particles; i++)
	{
		/*Using distance between particles and landmark build a list of landmarks that are
		 * within sensor range of any particle.
		 */
		double currPartX = particles[i].x;
		double currPartY = particles[i].y;
		double currPartTheta = particles[i].theta;
		
		vector<LandmarkObs> LandMarks;
		for(unsigned int j = 0; j < map_landmarks.landmark_list.size(); j++)
		{
			double lm_x = map_landmarks.landmark_list[j].x_f;
			double lm_y = map_landmarks.landmark_list[j].y_f;
			int    lm_id = map_landmarks.landmark_list[j].id_i;
			
			if((fabs(lm_x-currPartX) <= sensor_range) && (fabs(lm_y-currPartY) <= sensor_range) )
			{
				LandMarks.push_back(LandmarkObs{lm_id,lm_x,lm_y});
			}
		}
		
		//Transform observations made from vehicle to map co-ordinates
		vector<LandmarkObs>TransformObs;
		for(unsigned int j = 0; j < observations.size(); j++)
		{
			//Matrix multiplication from class
			double x = cos(currPartTheta)*observations[j].x - sin(currPartTheta)*observations[j].y + currPartX;
            double y = sin(currPartTheta)*observations[j].x + cos(currPartTheta)*observations[j].y + currPartY;
            TransformObs.push_back(LandmarkObs{ observations[j].id, x, y });
		}
		
		//Map landmarks to transformed observations
		dataAssociation(LandMarks,TransformObs);
		
		//Update weight step. First reset it
		particles[i].weight = 1.0;
		
		//Calculate weight for particle associated with an transformed observation
		for( unsigned int j = 0; j < TransformObs.size(); j++ )
		{
			double obsX = TransformObs[j].x;
			double obsY = TransformObs[j].y;
			
			int id = TransformObs[j].id;
			
			double LandMarkX, LandMarkY;
			unsigned int k = 0;
			
			bool bFound = false; //Not all landmarks might have been observed
			while ( k < LandMarks.size() )
			{
				if ( true == bFound ) break;
				LandMarkX = LandMarks[k].x;
				LandMarkY = LandMarks[k].y;
				k++;
			}
		
		    double dx = obsX - LandMarkX;
		    double dy = obsY - LandMarkY;
		    double gauss_norm = (1/(2*M_PI*sig_x*sig_y));
		    double exponent = (dx*dx)/(2*sig_x*sig_x) + (dy*dy)/(2*sig_y*sig_y);
		    double weight = gauss_norm *exp(-1.0*exponent);
		    if(weight <0.001 ) weight = 0.001;
		        particles[i].weight *= weight;
		}
	} 
}

void ParticleFilter::resample() {
	// TODO: Resample particles with replacement with probability proportional to their weight. 
	// NOTE: You may find std::discrete_distribution helpful here.
	//   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
	
	// 1. we have to find beta from 0 to 2*weightmax
	vector<double> weights;
	double maxwt = numeric_limits<double>::min();
	
	for( unsigned int i =0; i < num_particles; i++ )
	{
		weights::push_back(particles[i].weight);
		if( particles[i].weight > maxwt )
		{
			maxwt = particles[i].weight();
		}
	}
	
	//We found max weight. Now find beta from 0 to 2*maxwt from a distribution
	uniform_real_distribution<double> WtDist(0.0, maxwt);
	uniform_int_distribution<int> partDist(0, num_particles - 1);

	//Get a random starting point
	int index = partDist(gen);
	
	double beta = 0.0;
	
	vector<Particle> SampledParticles;
	for( unsigned int j =0; j < num_particles; j++ )
	{
		beta += WtDist(gen)*2.0;
		//Wheel logic
		while(beta > weights[index])
		{
			beta -= weights[index];
			index = (index+1)%num_particles;
		}
		//We got enough particles. 
		SampledParticles.push_back(particles[index]);
	}
	
	//Now use the new set as particles set
	particles = SampledParticles;
}

Particle ParticleFilter::SetAssociations(Particle& particle, const std::vector<int>& associations, 
                                     const std::vector<double>& sense_x, const std::vector<double>& sense_y)
{
    //particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
    // associations: The landmark id that goes along with each listed association
    // sense_x: the associations x mapping already converted to world coordinates
    // sense_y: the associations y mapping already converted to world coordinates

    particle.associations= associations;
    particle.sense_x = sense_x;
    particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best)
{
	vector<int> v = best.associations;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseX(Particle best)
{
	vector<double> v = best.sense_x;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseY(Particle best)
{
	vector<double> v = best.sense_y;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
