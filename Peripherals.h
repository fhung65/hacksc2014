#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#include<myo/myo.hpp>
#include <OVR_CAPI.h>

///////////////////////////////////////
//Tank functions
///////////////////////////////////////
std::string tank_forward(int speed);

std::string tank_right(int speed);

std::string tank_left(int speed);

//void tank_back(int speed);

///////////////////////////////////////
//Camera Functions
///////////////////////////////////////

std::string cam_rot(float speed);

std::string cam_screenshot();

///////////////////////////////////////
//Myo
///////////////////////////////////////
class tank_listener : public myo::DeviceListener
{
public:
	// These values are set by onArmRecognized() and onArmLost() above.
	bool onArm;
	myo::Arm whichArm;
	// These values are set by onOrientationData() and onPose() above.
	int pitch_w, yaw_w;
	myo::Pose currentPose;

	tank_listener() :
		onArm(false), pitch_w(0), yaw_w(0), currentPose() {}

	// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
	void onUnpair(myo::Myo* myo, uint64_t timestamp);

	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat);

	// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
	// making a fist, or not making a fist anymore.
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose);

	// onArmRecognized() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
	// arm. This lets Myo know which arm it's on and which way it's facing.
	void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection);

	// onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
	// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
	// when Myo is moved around on the arm.
	void onArmLost(myo::Myo* myo, uint64_t timestamp);

};

////////////////////////////////////////////////
//Oculus
////////////////////////////////////////////////

void init_OVR();

void quit_OVR();

///////////////////////////////////////////////////
//Main
///////////////////////////////////////////////////

class peripherals{
public:
	peripherals();
	~peripherals();

	std::string loop();
private:
	ovrHmd hmd;
	myo::Hub hub;
	myo::Myo* myo;
	tank_listener list;
	int last_shot;

};

#endif //PERIPHERALS_H_