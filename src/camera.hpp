/** camera.hpp **/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "global.hpp"

class camera_c {
	private:
		float posx, posy, posz, yaw, pitch;
		void set_defaults();

	public:
		camera_c();
		void move(const float mousex, const float mousey);
		void draw(const int d) const;
};

#endif
