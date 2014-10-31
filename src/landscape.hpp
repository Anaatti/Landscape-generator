/** landscape.hpp **/

#ifndef LANDSCAPE_HPP
#define LANDSCAPE_HPP

#include "global.hpp"

class landscape_c {
	private:
		struct coordinate {
			float x, y, z;
		} *coords;
		float *vertexes;
		float *colors;
		float *texcoords;
		unsigned int *index;
		const unsigned short size;

	public:
		landscape_c();
		~landscape_c();
		void draw() const;
};

#endif
