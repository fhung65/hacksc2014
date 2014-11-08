// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <SDL.h>
// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>
// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.
class DataCollector : public myo::DeviceListener {
public:
	DataCollector()
		: onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
	{
	}
	// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
	void onUnpair(myo::Myo* myo, uint64_t timestamp)
	{
		// We've lost a Myo.
		// Let's clean up some leftover state.
		roll_w = 0;
		pitch_w = 0;
		yaw_w = 0;
		onArm = false;
	}
	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;
		// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
		float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
			1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
		// Convert the floating point angles in radians to a scale from 0 to 18.
		m_roll = ((roll + (float)M_PI) / (M_PI * 2.0f) * 18);
		roll_w = static_cast<int>(m_roll);
		m_pitch = ((pitch + (float)M_PI / 2.0f) / M_PI * 18);
		pitch_w = static_cast<int>(m_pitch);
		m_yaw = ((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);
		yaw_w = static_cast<int>(m_yaw);
	}
	// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
	// making a fist, or not making a fist anymore.
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
	{
		currentPose = pose;
		// Vibrate the Myo whenever we've detected that the user has made a fist.
		if (pose == myo::Pose::fist) {
			myo->vibrate(myo::Myo::vibrationMedium);
		}
	}
	// onArmRecognized() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
	// arm. This lets Myo know which arm it's on and which way it's facing.
	void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
	{
		onArm = true;
		whichArm = arm;
	}
	// onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
	// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
	// when Myo is moved around on the arm.
	void onArmLost(myo::Myo* myo, uint64_t timestamp)
	{
		onArm = false;
	}
	// There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
	// For this example, the functions overridden above are sufficient.
	// We define this function to print the current values that were updated by the on...() functions above.
	void print()
	{
		// Clear the current line
		std::cout << '\r';
		// Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
		std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
			<< '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
			<< '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';
		if (onArm) {
			// Print out the currently recognized pose and which arm Myo is being worn on.
			// Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
			// output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
			// that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
			std::string poseString = currentPose.toString();
			std::cout << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
				<< '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
		}
		else {
			// Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
			std::cout << "[?]" << '[' << std::string(14, ' ') << ']';
		}
		std::cout << std::flush;
	}
	// These values are set by onArmRecognized() and onArmLost() above.
	bool onArm;
	myo::Arm whichArm;
	// These values are set by onOrientationData() and onPose() above.
	int roll_w, pitch_w, yaw_w;
	double m_roll, m_pitch, m_yaw;
	myo::Pose currentPose;
};

void put_pixel(SDL_Surface* screen, int screen_w, int screen_h, int x, int y, Uint32 val)
{ // TODO: add error checks for invalid values and screen
	if (x > screen_w || y > screen_h)
	{
		std::cout << "error, invalid pixel: out of screen bounds\n";
		exit(-1);
	}

	int index = (screen_w * y) + x;
	((Uint32*)screen->pixels)[index] = val;
}

int main(int argc, char** argv)
{
	// We catch any exceptions that might occur below -- see the catch statement for more details.
	try {
		// First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
		// publishing your application. The Hub provides access to one or more Myos.
		myo::Hub hub("com.example.hello-myo");
		std::cout << "Attempting to find a Myo..." << std::endl;
		// Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
		// immediately.
		// waitForAnyMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
		// if that fails, the function will return a null pointer.
		myo::Myo* myo = hub.waitForMyo(10000);
		// If waitForAnyMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}
		// We've found a Myo.
		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
		// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
		DataCollector collector;
		// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
		// Hub::run() to send events to all registered device listeners.
		hub.addListener(&collector);

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			return 1;
		}
		const int wind_width = 640;
		const int wind_height = 480;

		SDL_Window* wind = SDL_CreateWindow("Fuzzy",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			wind_width, wind_height, SDL_WINDOW_SHOWN);
		if (wind == nullptr)
		{
			std::cout << "SDL_CreateWindow Failure, errorcode: " << SDL_GetError() << std::endl;
			return 1;
		}

		SDL_Renderer* rend = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED);
		if (rend == nullptr)
		{
			std::cout << "SDL_CreateRederer Failure, errorcode: " << SDL_GetError() << std::endl;
			return 1;
		}

		SDL_Surface* surf = SDL_GetWindowSurface(wind);
		if (surf == nullptr)
		{
			std::cout << "SDL_GetWindowSurface Failur, errorcode: " << SDL_GetError() << std::endl;
			return 1;
		}



		// Finally we enter our main loop.
		while (1) {
			// In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
			// In this case, we wish to update our display 20 times a second, so we run for 1000/20 milliseconds.
			hub.run(1000 / 20);
			// After processing events, we call the print() member function we defined above to print out the values we've
			// obtained from any events that have occurred.
			collector.print();
			SDL_UnlockSurface(surf);
			Uint32* pix;

			put_pixel(surf, wind_width, wind_height, wind_width*(collector.m_yaw/(18.0)), wind_height*(collector.m_pitch/18.0), 0x00ffffff);

			SDL_LockSurface(surf);
			SDL_UpdateWindowSurface(wind);
		}
		SDL_Quit();
		// If a standard exception occurred, we print out its message and exit.
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
}
