#define _USE_MATH_DEFINES
#include<iostream>
#include<cmath>
#include<algorithm>
#include<myo/myo.hpp>
#include "MyoCon.h"
#include<OVR.h>

void move_forward(int speed)
{
	std::cout << "f." << speed << std::endl;
}

void turn_right(int speed)
{
	std::cout << "r." << speed << std::endl;
}

void turn_left(int speed)
{
	std::cout << "l." << speed << std::endl;
}

void move_back(int speed)
{
	std::cout << "b." << speed << std::endl;
}

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
		// Convert the floating point angles in radians to a scale from 0 to 18.
		pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
		yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);
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



int main(int argc, char* argv[])
{
	std::cout << "starting\n";
	myo::Hub hub;

	std::cout << "Trying to find a Myo...\n";
	myo::Myo* myo = hub.waitForMyo(10000);

	if (myo == nullptr)
	{
		std::cout << "Unable to find a Myo!\n";
		exit(1);
	}
	std::cout << "Connected to Myo\n";

	tank_listener list;

	hub.addListener(&list);

	bool quit = false;
	while (!quit)
	{
		hub.run(1000 / 20);

		myo::Pose pose = list.currentPose;
		myo::Arm arm = list.whichArm;

		if (pose == myo::Pose::fist)
		{
			move_forward(list.pitch_w); // currently, has values 0 - 18
		}
		else if (pose == myo::Pose::fingersSpread)
		{
			move_back(list.pitch_w);
		}
		else if ((pose == myo::Pose::waveIn && arm == myo::Arm::armLeft)||
				(pose == myo::Pose::waveOut && arm == myo::Arm::armRight))
		{
			turn_right(18-list.yaw_w);
		}
		else if ((pose == myo::Pose::waveIn && arm == myo::Arm::armRight) ||
			(pose == myo::Pose::waveOut && arm == myo::Arm::armLeft))
		{
			turn_left(list.yaw_w);
		}
		else if (pose == myo::Pose::thumbToPinky)
		{
			quit = true;
		}
	}

	return 0;
}
