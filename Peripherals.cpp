#define _USE_MATH_DEFINES
#include<iostream>
#include<cmath>
#include<algorithm>
#include<myo/myo.hpp>
#include<OVR_CAPI.h>
#include<OVR_Math.h>
#include<math.h>
#include"Peripherals.h"
#include <sstream>

///////////////////////////////////////
//Tank functions
///////////////////////////////////////
std::string tank_forward(int speed)
{
	std::stringstream ss;
	ss << "tf:" << speed << std::endl;
	std::string out;
	ss >> out;
	return out;
}

std::string tank_backward(int speed)
{
	std::stringstream ss;
	ss << "tb:" << speed << std::endl;
	std::string out;
	ss >> out;
	return out;
}


std::string tank_right(int speed)
{
	std::stringstream ss;
	ss << "tr:" << speed << std::endl;
	std::string out;
	ss >> out;
	return out;
}

std::string tank_left(int speed)
{
	std::stringstream ss;
	ss << "tl:" << speed << std::endl;
	std::string out;
	ss >> out;
	return out;
}

//void tank_back(int speed)
//{
//	std::cout << "tb:" << speed << std::endl;
//}

///////////////////////////////////////
//Camera Functions
///////////////////////////////////////

std::string cam_rot(float speed)// will be +/- and magnitude matters
{
	std::stringstream ss;
	ss << "cr:" << speed << std::endl;
	std::string out;
	ss >> out;
	return out;
}

std::string cam_pitch(float speed)
{
	std::stringstream ss;
	ss << "cp:" << speed << std::endl;
	std::string out;
	ss >> out;
	return out;
}

std::string cam_screenshot()
{
	std::stringstream ss;
	ss << "sc" << std::endl;
	std::string out;
	ss >> out;
	return out;
}

///////////////////////////////////////
//Myo
///////////////////////////////////////

	// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
	void tank_listener::onUnpair(myo::Myo* myo, uint64_t timestamp)
	{
		// We've lost a Myo.
		// Let's clean up some leftover state.
		pitch_w = 0;
		yaw_w = 0;
		onArm = false;
	}
	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void tank_listener::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;
		// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
		// Convert the floating point angles in radians to a scale from 0 to 127.
		pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 127);
		yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 127);
	}
	// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
	// making a fist, or not making a fist anymore.
	void tank_listener::onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
	{
		currentPose = pose;
		// Vibrate the Myo whenever we've detected that the user has made a fist.
		if (pose == myo::Pose::fist) {
			myo->vibrate(myo::Myo::vibrationMedium);
		}
	}
	// onArmRecognized() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
	// arm. This lets Myo know which arm it's on and which way it's facing.
	void tank_listener::onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
	{
		onArm = true;
		whichArm = arm;
	}
	// onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
	// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
	// when Myo is moved around on the arm.
	void tank_listener::onArmLost(myo::Myo* myo, uint64_t timestamp)
	{
		onArm = false;
	}

////////////////////////////////////////////////
//Oculus
////////////////////////////////////////////////

void init_OVR()
{
	ovr_Initialize();
}

void quit_OVR()
{
	ovr_Shutdown();
}

///////////////////////////////////////////////////
//Main
///////////////////////////////////////////////////

peripherals::peripherals(){
	std::cout << "starting\n";

	//Initialize head mount device
	std::cout << "Initializing OVR\n";
	ovr_Initialize();

	hmd = ovrHmd_Create(0);
	if (!hmd)
	{
		std::cout << "failed to create head mount device!\n";
		exit(1);
	}

	//configure ovrHmd for tracking -> we have three suppotrted tracking capabilities, one req'd
	bool ovrHmd_config_tracking_success =
		ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
		ovrTrackingCap_Position |
		ovrTrackingCap_MagYawCorrection, 0);
	if (!ovrHmd_config_tracking_success)
	{
		std::cout << "failed to configure required capabilities (MagYawCorrection)\n";
		exit(1);
	}

	std::cout << "Trying to find a Myo...\n";
	myo = hub.waitForMyo(10000);

	if (myo == nullptr)
	{
		std::cout << "Unable to find a Myo!\n";
		exit(1);
	}
	std::cout << "Connected to Myo\n";

	hub.addListener(&list);

	last_shot = 0;
}

peripherals::~peripherals(){
	ovrHmd_Destroy(hmd);
	ovr_Shutdown();
}

std::string peripherals::loop(){
	//update the hub, 1 sec/ 20 times 
	hub.run(1000 / 20);
	//get data from myolistener
	myo::Pose myoPose = list.currentPose;
	myo::Arm myoArm = list.whichArm;
	std::string out = "";

	//do stuff with myo data
	if (myoPose == myo::Pose::fist)
	{
		out = tank_forward(list.pitch_w); // currently, has values 0 - 18
	}
	else if (myoPose == myo::Pose::fingersSpread)
	{
		out = tank_backward(list.pitch_w);
	}
	else if ((myoPose == myo::Pose::waveIn && myoArm == myo::Arm::armLeft) ||
		(myoPose == myo::Pose::waveOut && myoArm == myo::Arm::armRight))
	{
		out = tank_right(18 - list.yaw_w);
	}
	else if ((myoPose == myo::Pose::waveIn && myoArm == myo::Arm::armRight) ||
		(myoPose == myo::Pose::waveOut && myoArm == myo::Arm::armLeft))
	{
		out = tank_left(list.yaw_w);
	}
	else if (myoPose == myo::Pose::thumbToPinky && last_shot > 50)
	{	
		out = cam_screenshot();
		last_shot = 0;
	}
	if (last_shot < 2000)
		last_shot++;

	ovrTrackingState trackState = ovrHmd_GetTrackingState(hmd,
		ovr_GetTimeInSeconds());
	OVR::Quatf ovrPose = trackState.HeadPose.ThePose.Orientation;
	float yaw, pitch, roll;
	ovrPose.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);

	out += ";" + cam_rot(yaw) + ";" + cam_pitch(pitch);
	return out;
}
